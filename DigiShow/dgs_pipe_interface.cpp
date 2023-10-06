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

#include "dgs_pipe_interface.h"
#include "digishow_slot.h"

#ifdef DIGISHOW_CLOUD
#include "digishow_cloud.h"
#else
#include "digishow_cloud_dummy.h"
#endif

DgsPipeInterface::DgsPipeInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "pipe";

    m_websocketServer = nullptr;
    m_websocketServerConnections.clear();
    m_websocketServerMultiple = false;

    m_websocketClientConnection = nullptr;
}

DgsPipeInterface::~DgsPipeInterface()
{
    closeInterface();
}

int DgsPipeInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // start a websocket server for local pipe (accepted remote link)
    int acceptRemote = m_interfaceOptions.value("acceptRemote").toInt();
    if (m_interfaceInfo.mode == INTERFACE_PIPE_LOCAL && acceptRemote) {

        m_websocketServerMultiple = (acceptRemote==1 ? false : true);
        bool done = startWebsocketServer();
        if (!done) return ERR_DEVICE_NOT_READY;
    }

    // start a websocket client for remote pipe
    if (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE ||
        m_interfaceInfo.mode == INTERFACE_PIPE_CLOUD ) {

        bool done = startWebsocketClient();
        if (!done) return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsPipeInterface::closeInterface()
{
    // stop the websocket server for local pipe (accepted remote link)
    stopWebsocketServer();

    // stop the websocket client for remote pipe
    stopWebsocketClient();

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsPipeInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // send websocket message if remote link enabled
    sendWebsocketMessage(signalToMessage(data, m_endpointInfoList[endpointIndex].channel));

    // for local pipe
    emit dataReceived(endpointIndex, data);
    return ERR_NONE;
}


bool DgsPipeInterface::startWebsocketServer()
{
    if (m_websocketServer != nullptr) return false;

    m_websocketServer = new QWebSocketServer(QString("digishow"), QWebSocketServer::NonSecureMode);
    m_websocketServerConnections.clear();

    int websocketPort = m_interfaceOptions.value("tcpPort").toInt();
    if (!m_websocketServer->listen(QHostAddress::Any, websocketPort)) return false;

    connect(m_websocketServer, SIGNAL(newConnection()), this, SLOT(onWebsocketServerNewConnection()));
    return true;
}

void DgsPipeInterface::stopWebsocketServer()
{
    foreach (QWebSocket *websocket, m_websocketServerConnections) {
        websocket->close();
        delete websocket;
    }
    m_websocketServerConnections.clear();

    if (m_websocketServer != nullptr) {
        m_websocketServer->close();
        delete m_websocketServer;
        m_websocketServer = nullptr;
    }
}

bool DgsPipeInterface::startWebsocketClient()
{
    if (m_websocketClientConnection != nullptr) return false;

    QString url;
    if (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE) {

        url = QString("ws://%1:%2")
              .arg(m_interfaceOptions.value("tcpHost").toString())
              .arg(m_interfaceOptions.value("tcpPort").toInt());

    } else if (m_interfaceInfo.mode == INTERFACE_PIPE_CLOUD) {

        DigishowCloud cloud;
        QString pipeId = m_interfaceOptions.value("pipeId").toString();
        url = cloud.getCloudPipeUrl(pipeId);
    }
    if (url.isEmpty()) return false;

    QWebSocket *websocket = new QWebSocket();

    connect(websocket, SIGNAL(connected()), this, SLOT(onWebsocketClientConnected()));
    connect(websocket, SIGNAL(disconnected()), this, SLOT(onWebsocketClientDisconnected()));
    connect(websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWebsocketClientMessageReceived(QString)));

    websocket->open(QUrl(url));

    // wait and confirm the websocket is connected
    QElapsedTimer timer;
    timer.start();
    while (!timer.hasExpired(3000)) {
        qApp->processEvents(QEventLoop::AllEvents);
        if (m_websocketClientConnection == websocket) return true;
    }

    return false;
}

void DgsPipeInterface::stopWebsocketClient()
{
    if (m_websocketClientConnection != nullptr) {
        m_websocketClientConnection->close();
        delete m_websocketClientConnection;
        m_websocketClientConnection = nullptr;
    }
}

void DgsPipeInterface::sendWebsocketMessage(const QString &message)
{
    // for server mode
    foreach (QWebSocket *websocket, m_websocketServerConnections) {
        websocket->sendTextMessage(message);
        websocket->flush();
    }

    // for client mode
    if (m_websocketClientConnection != nullptr) {
        m_websocketClientConnection->sendTextMessage(message);
        m_websocketClientConnection->flush();
    }
}

void DgsPipeInterface::sendWebsocketHelloMessages(QWebSocket *websocket)
{
    // send the app info to the remote side
    qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    websocket->sendTextMessage(
                QString("appinfo,0,%1,%2,%3,%4")
                .arg(g_appname, g_version, g_serial, QString::number(timestamp)));

    // send the preset launcher data to the remote side
    QVariantMap data;
    data["launches"] = g_app->getAllLaunchOptions();

    websocket->sendTextMessage(
                "appdata,0," +
                QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact).toBase64());
}


int DgsPipeInterface::findEndpoint(int type, int channel)
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        if (m_endpointInfoList[n].type == type && m_endpointInfoList[n].channel == channel) return n;
    }

    return -1;
}

void DgsPipeInterface::processReceivedMessage(const QString &message)
{
    // for receiving signal
    static dgsSignalData data;
    static int channel;

    // for receiving others
    static QString slotName;
    static QString optName;
    static QString optValue;
    static bool linked;
    static int launchId;

    if (messageToSignal(message, &data, &channel)) {

        int type = -1;
        switch (data.signal) {
        case 'A': type = ENDPOINT_PIPE_ANALOG; break;
        case 'B': type = ENDPOINT_PIPE_BINARY; break;
        case 'N': type = ENDPOINT_PIPE_NOTE;   break;
        }
        if (type==-1) return;

        int endpointIndex = findEndpoint(type, channel);
        if (endpointIndex==-1) return;

        // fix the analog signal if the value range dosen't match
        if (type == ENDPOINT_PIPE_ANALOG && data.aRange != m_endpointInfoList[endpointIndex].range) {
            data.aRange = m_endpointInfoList[endpointIndex].range;
            data.aValue = qMin(data.aValue, data.aRange);
        }

        // for local pipe
        emit dataReceived(endpointIndex, data);

    } else if (messageToSlotOption(message, slotName, optName, optValue)) {

        DigishowSlot *slot = g_app->slotTitled(slotName);
        if (slot != nullptr) slot->setSlotOption(optName, optValue);

    } else if (messageToSlotLink(message, slotName, &linked)) {

        DigishowSlot *slot = g_app->slotTitled(slotName);
        if (slot != nullptr) slot->setLinked(linked);

    } else if (messageToLaunch(message, &launchId)) {

        if (launchId != 0) g_app->startLaunch("launch" + QString::number(launchId));
    }
}

void DgsPipeInterface::onWebsocketServerNewConnection()
{
    // disconnect old websocket session
    if (!m_websocketServerMultiple) {
        foreach (QWebSocket *websocket, m_websocketServerConnections) {
            websocket->close();
            delete websocket;
        }
        m_websocketServerConnections.clear();
    }

    // accept new websocket session
    while (m_websocketServer->hasPendingConnections()) {
        QWebSocket *websocket = m_websocketServer->nextPendingConnection();

        connect(websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWebsocketServerMessageReceived(QString)));
        connect(websocket, SIGNAL(disconnected()), this, SLOT(onWebsocketServerDisconnected()));

        qDebug("[server] websocket request %s is accepted.",
               websocket->requestUrl().toDisplayString().toLocal8Bit().constData());
        qDebug("[server] websocket %p is connected.", websocket);

        m_websocketServerConnections.append(websocket);

        // send hello messages after connected
        sendWebsocketHelloMessages(websocket);
    }
}

void DgsPipeInterface::onWebsocketServerDisconnected()
{
    QWebSocket *websocket = qobject_cast<QWebSocket *>(sender());
    qDebug("[server] websocket %p is disconnected.", websocket);

    m_websocketServerConnections.removeAll(websocket);
    websocket->deleteLater();
}

void DgsPipeInterface::onWebsocketServerMessageReceived(const QString & message)
{
    //qDebug() << "onWebsocketServerMessageReceived" << message;
    processReceivedMessage(message);
}

void DgsPipeInterface::onWebsocketClientConnected()
{
    QWebSocket *websocket = qobject_cast<QWebSocket *>(sender());

    qDebug("[client] websocket request %s is accepted.",
           websocket->requestUrl().toDisplayString().toLocal8Bit().constData());
    qDebug("[client] websocket %p is connected.", websocket);

    m_websocketClientConnection = websocket;

    // send hello messages after connected
    sendWebsocketHelloMessages(websocket);
}

void DgsPipeInterface::onWebsocketClientDisconnected()
{
    QWebSocket *websocket = qobject_cast<QWebSocket *>(sender());
    qDebug("[client] websocket %p is disconnected.", websocket);

    if (websocket == m_websocketClientConnection) m_websocketClientConnection = nullptr;
    websocket->deleteLater();
}

void DgsPipeInterface::onWebsocketClientMessageReceived(const QString & message)
{
    //qDebug() << "onWebsocketClientMessageReceived" << message;
    processReceivedMessage(message);
}

QString DgsPipeInterface::signalToMessage(dgsSignalData data, int channel)
{
    return QString("dgss,%1,%2,%3,%4,%5")
            .arg(channel)
            .arg(uint8_t(data.signal))
            .arg(data.aValue)
            .arg(data.aRange)
            .arg(data.bValue);
}

bool DgsPipeInterface::messageToSignal(const QString &message, dgsSignalData *data, int *channel)
{
    // signal message:
    // dgss,<channel>,<signal_type>,<a_value>,<a_range>,<b_value>

    // example:
    // dgss,1,65,1024,65535,0

    QStringList items = message.split(',');
    if (items.length()==6 && items[0]=="dgss") {
        *channel = items[1].toInt();
        data->signal = items[2].toInt();
        data->aValue = items[3].toInt();
        data->aRange = items[4].toInt();
        data->bValue = items[5].toInt();
        return true;
    }

    return false;
}

bool DgsPipeInterface::messageToSlotOption(const QString &message, QString &slotName, QString &optName, QString &optValue)
{
    // slot option message:
    // slot,<slot_name>,<opt_name>,<opt_value>

    // example:
    // slot,Dimmer,outputSmoothing,1000

    QStringList items = message.split(',');
    if (items.length()==4 && items[0]=="slot") {
        slotName = items[1];
        optName = items[2];
        optValue = items[3];
        return true;
    }

    return false;
}

bool DgsPipeInterface::messageToSlotLink(const QString &message, QString &slotName, bool *linked)
{
    // slot link message:
    // link,<slot_name>,<linked>

    // example:
    // link,Dimmer,0

    QStringList items = message.split(',');
    if (items.length()==3 && items[0]=="link") {
        slotName = items[1];
        *linked = items[2].toInt();
        return true;
    }

    return false;
}

bool DgsPipeInterface::messageToLaunch(const QString &message, int *launchId)
{
    // launch message:
    // launch,<launch_id>

    // example:
    // launch,1

    QStringList items = message.split(',');
    if (items.length()==2 && items[0]=="launch") {
        *launchId = items[1].toInt();
        return true;
    }

    return false;
}

