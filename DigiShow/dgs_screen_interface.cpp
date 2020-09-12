#include "dgs_screen_interface.h"
#include <QJsonDocument>
#include <QtQuick/QQuickWindow>
#include <QGuiApplication>
#include <QScreen>

DgsScreenInterface::DgsScreenInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "screen";

    m_qmlEngine = new QQmlEngine();
    m_qmlComponentPlayer = new QQmlComponent(m_qmlEngine);
    m_qmlComponentPlayer->loadUrl(QUrl("qrc:///ScreenPlayer.qml"));
    m_player = nullptr;

    QQuickWindow::setDefaultAlphaBuffer(true);
}

DgsScreenInterface::~DgsScreenInterface()
{
    closeInterface();

    delete m_qmlComponentPlayer;
    delete m_qmlEngine;
}

int DgsScreenInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    if (m_interfaceInfo.mode==INTERFACE_SCREEN_LOCAL) {

        // initialize local screen controls

        // clear player
        if (m_player != nullptr) {
            delete m_player;
            m_player = nullptr;
        }

        // make a player in the screen
        QString screenName = m_interfaceOptions.value("screen").toString();
        if (!screenName.isEmpty()) {

            // confirm the specified screen is ready
            int screenIndex = -1;
            QList<QScreen*> screens = QGuiApplication::screens();
            for (int n=0 ; n < screens.length() ; n++)
                if (getUniqueScreenName(n) == screenName) { screenIndex = n; break; }
            if (screenIndex == -1) return ERR_DEVICE_NOT_READY;

            // make player (based on qml)
            if (!m_qmlComponentPlayer->isReady()) return ERR_DEVICE_NOT_READY;
            m_player = m_qmlComponentPlayer->create();

            // show player
            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "showInScreen", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, screenIndex));

            // load media
            QVariantList mediaList = cleanMediaList();
            QMetaObject::invokeMethod(
                        m_player, "loadMediaList", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, mediaList));
        }

    } else if (m_interfaceInfo.mode==INTERFACE_SCREEN_REMOTE) {

        // initialize remote screen controls

        // TODO:
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsScreenInterface::closeInterface()
{
    if (m_interfaceInfo.mode==INTERFACE_SCREEN_LOCAL) {

        // close player
        if (m_player != nullptr) {

            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "goodbye", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r));

            delete m_player;
            m_player = nullptr;
        }

    } else if (m_interfaceInfo.mode==INTERFACE_SCREEN_REMOTE) {

        // TODO:
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsScreenInterface::sendData(int endpointIndex, dgsSignalData data)
{    
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (m_interfaceInfo.mode==INTERFACE_SCREEN_LOCAL) {

        if (m_player == nullptr) return ERR_DEVICE_NOT_READY;

        if (m_endpointInfoList[endpointIndex].type == ENDPOINT_SCREEN_LIGHT) {

            // control screen light (based on qml)

            if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

            int control = m_endpointInfoList[endpointIndex].control;
            QByteArray property;
            switch (control) {
                case CONTROL_LIGHT_R: property = "lightR"; break;
                case CONTROL_LIGHT_G: property = "lightG"; break;
                case CONTROL_LIGHT_B: property = "lightB"; break;
                case CONTROL_LIGHT_W: property = "lightW"; break;
                default: return ERR_INVALID_OPTION;
            }

            double value = static_cast<double>(data.aValue) / static_cast<double>(data.aRange==0 ? 255 : data.aRange);
            if (value<0 || value>1) return ERR_INVALID_DATA;

            m_player->setProperty(property, value);

        } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_SCREEN_MEDIA) {

            // show or hide media clips on screen (based on qml)

            int control = m_endpointInfoList[endpointIndex].control;
            QString media = m_endpointOptionsList[endpointIndex]["media"].toString();

            if (control == CONTROL_MEDIA_SHOW) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {
                    QVariant r;
                    QMetaObject::invokeMethod(
                                m_player, "showMedia", Qt::DirectConnection,
                                Q_RETURN_ARG(QVariant, r),
                                Q_ARG(QVariant, media),
                                Q_ARG(QVariant, m_endpointOptionsList[endpointIndex]));
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_HIDE) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {
                    QVariant r;
                    QMetaObject::invokeMethod(
                                m_player, "hideMedia", Qt::DirectConnection,
                                Q_RETURN_ARG(QVariant, r),
                                Q_ARG(QVariant, media));
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_CLEAR) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {
                    QVariant r;
                    QMetaObject::invokeMethod(
                                m_player, "clearAllMedia", Qt::DirectConnection,
                                Q_RETURN_ARG(QVariant, r));
                }

                return ERR_NONE;
            }

            // control media clip display options (based on qml)
            QString property;
            switch (control) {
                case CONTROL_MEDIA_OPACITY:  property = "opacity";  break;
                case CONTROL_MEDIA_SCALE:    property = "scale";    break;
                case CONTROL_MEDIA_ROTATION: property = "rotation"; break;
                case CONTROL_MEDIA_XOFFSET:  property = "xOffset";  break;
                case CONTROL_MEDIA_YOFFSET:  property = "yOffset";  break;
                default: return ERR_INVALID_OPTION;
            }

            if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

            double value;
            if (control == CONTROL_MEDIA_ROTATION) {
                value = 360 * static_cast<double>(data.aValue) / (data.aRange==0 ? 3600 : data.aRange);
                if (value<0 || value>360) return ERR_INVALID_DATA;
            } else {
                value = static_cast<double>(data.aValue) / static_cast<double>(data.aRange==0 ? 10000 : data.aRange);
                if (value<0 || value>1) return ERR_INVALID_DATA;
            }

            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "setMediaProperty", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r),
                        Q_ARG(QVariant, media),
                        Q_ARG(QVariant, property),
                        Q_ARG(QVariant, value));
        }

    } else if (m_interfaceInfo.mode==INTERFACE_SCREEN_REMOTE) {

        // TODO:
    }

    return ERR_NONE;
}

void DgsScreenInterface::loadMedia(const QVariantMap &mediaOptions)
{
    QVariant r;
    QMetaObject::invokeMethod(
                m_player, "loadMedia", Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, mediaOptions));
}

QVariantList DgsScreenInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    QList<QScreen*> screens = QGuiApplication::screens();
    for (int n=0 ; n < screens.length() ; n++) {
        info.clear();
        info["screen"] = getUniqueScreenName(n);
        info["width"]  = screens[n]->geometry().width();
        info["height"] = screens[n]->geometry().height();
        info["ratio"]  = screens[n]->devicePixelRatio();
        list.append(info);
    }

    return list;
}

QVariantList DgsScreenInterface::cleanMediaList()
{
    // clean up media list (delete unused media)

    QVariantList mediaList = m_interfaceOptions["media"].toList();
    int mediaCount = mediaList.length();
    for (int n = mediaCount-1 ; n >= 0 ; n--) {
        QString mediaName = mediaList[n].toMap().value("name").toString();
        bool exists = false;
        for (int i = 0 ; i < endpointCount() ; i++) {
            if (m_endpointOptionsList[i].value("media").toString() == mediaName &&
                m_endpointInfoList[i].enabled) {
                exists = true;
                break;
            }
        }
        if (!exists) mediaList.removeAt(n);
    }
    if (mediaList.length() < mediaCount) m_interfaceOptions["media"] = mediaList;

    return mediaList;
}

QString DgsScreenInterface::getUniqueScreenName(int index)
{
    QString screenName;
    QList<QScreen*> screens = QGuiApplication::screens();
    int i = 0;

    if (index >= 0 && index < screens.length()) {

        screenName = screens[index]->name();

        for (int n=0 ; n<index ; n++)
            if (screens[n]->name() == screenName) i++;
    }

    if (i>0) return screenName + " #" + QString::number(i+1);
    return screenName;
}

