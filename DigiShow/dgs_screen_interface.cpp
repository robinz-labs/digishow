/*
    Copyright 2021 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#include "dgs_screen_interface.h"
#include "QtCore/qobjectdefs.h"
#include "digishow_environment.h"
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

    //QQuickWindow::setDefaultAlphaBuffer(true);
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

    if (m_interfaceInfo.mode==INTERFACE_SCREEN_DEFAULT) {

        // initialize local screen controls

        // clear player
        if (m_player != nullptr) {
            delete m_player;
            m_player = nullptr;
        }

        // make a player in the screen
        int screen = m_interfaceOptions.value("screen").toInt();

        // confirm the specified screen is ready
        QList<QScreen*> screens = QGuiApplication::screens();
        if (screen < -1 || screen > screens.length()) return ERR_DEVICE_NOT_READY;

        // make player (based on qml)
        if (!m_qmlComponentPlayer->isReady()) return ERR_DEVICE_NOT_READY;
        m_player = m_qmlComponentPlayer->create();

        // show player
        QVariant r;
        QMetaObject::invokeMethod(
                    m_player, "showInScreen", Qt::DirectConnection,
                    Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, screen));

        // load media
        QVariantList mediaList = cleanMediaList();
        QMetaObject::invokeMethod(
                    m_player, "loadMediaList", Qt::DirectConnection,
                    Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, mediaList));

    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsScreenInterface::closeInterface()
{
    if (m_interfaceInfo.mode==INTERFACE_SCREEN_DEFAULT) {

        // close player
        if (m_player != nullptr) {

            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "goodbye", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r));

            delete m_player;
            m_player = nullptr;
        }

    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsScreenInterface::sendData(int endpointIndex, dgsSignalData data)
{    
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (m_interfaceInfo.mode==INTERFACE_SCREEN_DEFAULT) {

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

        } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_SCREEN_CANVAS) {

            // control canvas display options (based on qml)

            int control = m_endpointInfoList[endpointIndex].control;
            QString property = getPropertyName(control);
            if (property.isEmpty()) return ERR_INVALID_OPTION;

            if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
            double value = getPropertyValue(control, data);

            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "setCanvasProperty", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r),
                        Q_ARG(QVariant, property),
                        Q_ARG(QVariant, value));


        } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_SCREEN_MEDIA) {

            // show or hide media clips on screen (based on qml)

            int control = m_endpointInfoList[endpointIndex].control;
            QString media = m_endpointOptionsList[endpointIndex].value("media").toString();

            if (control == CONTROL_MEDIA_START || control == CONTROL_MEDIA_RESTART) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {
                    QVariant r;
                    QMetaObject::invokeMethod(
                                m_player, "showMedia", Qt::DirectConnection,
                                Q_RETURN_ARG(QVariant, r),
                                Q_ARG(QVariant, media),
                                Q_ARG(QVariant, m_endpointOptionsList[endpointIndex]),
                                Q_ARG(QVariant, control == CONTROL_MEDIA_RESTART));
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_STOP) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {
                    QVariant r;
                    QMetaObject::invokeMethod(
                                m_player, "hideMedia", Qt::DirectConnection,
                                Q_RETURN_ARG(QVariant, r),
                                Q_ARG(QVariant, media));
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_STOP_ALL) {

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

            QString property = getPropertyName(control);
            if (property.isEmpty()) return ERR_INVALID_OPTION;

            if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
            double value = getPropertyValue(control, data);

            QVariant r;
            QMetaObject::invokeMethod(
                        m_player, "setMediaProperty", Qt::DirectConnection,
                        Q_RETURN_ARG(QVariant, r),
                        Q_ARG(QVariant, media),
                        Q_ARG(QVariant, property),
                        Q_ARG(QVariant, value));
        }

    }

    return ERR_NONE;
}

int DgsScreenInterface::loadMedia(const QVariantMap &mediaOptions)
{
    int r = DigishowInterface::loadMedia(mediaOptions);
    if ( r != ERR_NONE) return r;

    QVariant rr;
    QMetaObject::invokeMethod(
                m_player, "loadMedia", Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, rr), Q_ARG(QVariant, mediaOptions));

    return ERR_NONE;
}

QVariantList DgsScreenInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    QList<QScreen*> screens = QGuiApplication::screens();
    for (int n=0 ; n < screens.length() ; n++) {
        info.clear();
        info["name"]   = getUniqueScreenName(n);
        info["width"]  = screens[n]->geometry().width();
        info["height"] = screens[n]->geometry().height();
        info["ratio"]  = screens[n]->devicePixelRatio();
        list.append(info);
    }

    return list;
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

    if (screenName.isEmpty()) return "#" + QString::number(i+1);
    if (i>0) return screenName + " #" + QString::number(i+1);
    return screenName;
}

QString DgsScreenInterface::getPropertyName(int control)
{
    QString property;
    switch (control) {
        case CONTROL_MEDIA_OPACITY:  property = "opacity";  break;
        case CONTROL_MEDIA_SCALE:    property = "scale";    break;
        case CONTROL_MEDIA_ROTATION: property = "rotation"; break;
        case CONTROL_MEDIA_XOFFSET:  property = "xOffset";  break;
        case CONTROL_MEDIA_YOFFSET:  property = "yOffset";  break;
        case CONTROL_MEDIA_VOLUME:   property = "volume";   break;
        case CONTROL_MEDIA_SPEED:    property = "speed";    break;
        case CONTROL_MEDIA_POSITION: property = "position"; break;
    }
    return property;
}

double DgsScreenInterface::getPropertyValue(int control, dgsSignalData data)
{
    double value = 0;
    if (control == CONTROL_MEDIA_ROTATION) {
        value = 360 * static_cast<double>(data.aValue) / (data.aRange==0 ? 3600 : data.aRange);
        if (value<0) value=0; else if (value>360) value=360;

    } else if (control == CONTROL_MEDIA_POSITION) {
        value = static_cast<double>(data.aValue);
        if (value<0) value=0;

    } else {
        value = static_cast<double>(data.aValue) / static_cast<double>(data.aRange==0 ? 10000 : data.aRange);
        if (value<0) value=0; else if (value>1) value=1;
        if (control == CONTROL_MEDIA_SCALE || control == CONTROL_MEDIA_SPEED) value = value*2;
    }
    return value;
}

void DgsScreenInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_SCREEN;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_SCREEN_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = false;

    // Set interface label
    QString labelIdentity;
    switch (int n = m_interfaceOptions.value("screen").toInt()) {
    case -1: labelIdentity = tr("(Preview Window)"); break;
    case  0: labelIdentity = ""; break;
    default: labelIdentity = QString::number(n);
    }
    m_interfaceInfo.label = tr("Screen") + " " + labelIdentity;

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "light" ) endpointInfo.type = ENDPOINT_SCREEN_LIGHT;
        else if (typeName == "media" ) endpointInfo.type = ENDPOINT_SCREEN_MEDIA;
        else if (typeName == "canvas") endpointInfo.type = ENDPOINT_SCREEN_CANVAS;

        // Set endpoint properties based on type
        switch (endpointInfo.type) {
            case ENDPOINT_SCREEN_LIGHT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = 255;
                endpointInfo.labelEPT = tr("Backlight");
                endpointInfo.labelEPI = DigishowEnvironment::getLightControlName(endpointInfo.control);
                break;

            case ENDPOINT_SCREEN_MEDIA:
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Media Clip");
                endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control, true);

                switch (endpointInfo.control) {
                    case CONTROL_MEDIA_START:
                    case CONTROL_MEDIA_RESTART:
                    case CONTROL_MEDIA_STOP:
                    case CONTROL_MEDIA_STOP_ALL:
                        endpointInfo.signal = DATA_SIGNAL_BINARY;
                        break;
                    case CONTROL_MEDIA_ROTATION:
                        endpointInfo.signal = DATA_SIGNAL_ANALOG;
                        endpointInfo.range  = 3600;
                        break;
                    case CONTROL_MEDIA_OPACITY:
                    case CONTROL_MEDIA_SCALE:
                    case CONTROL_MEDIA_XOFFSET:
                    case CONTROL_MEDIA_YOFFSET:
                    case CONTROL_MEDIA_VOLUME:
                    case CONTROL_MEDIA_SPEED:
                        endpointInfo.signal = DATA_SIGNAL_ANALOG;
                        endpointInfo.range  = 10000;
                        break;
                    case CONTROL_MEDIA_POSITION:
                        endpointInfo.signal = DATA_SIGNAL_ANALOG;
                        endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 100000);
                        break;
                }
                break;

            case ENDPOINT_SCREEN_CANVAS:
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Canvas");
                endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control, true);

                switch (endpointInfo.control) {
                    case CONTROL_MEDIA_ROTATION:
                        endpointInfo.signal = DATA_SIGNAL_ANALOG;
                        endpointInfo.range  = 3600;
                        break;
                    case CONTROL_MEDIA_OPACITY:
                    case CONTROL_MEDIA_SCALE:
                    case CONTROL_MEDIA_XOFFSET:
                    case CONTROL_MEDIA_YOFFSET:
                        endpointInfo.signal = DATA_SIGNAL_ANALOG;
                        endpointInfo.range  = 10000;
                        break;
                }
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}
