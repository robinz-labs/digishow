#include "dgs_pipe_interface.h"

DgsPipeInterface::DgsPipeInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "pipe";

    m_server = nullptr;
    m_websocketServer = nullptr;
    m_websocketClient = nullptr;
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
    if (m_interfaceInfo.mode == INTERFACE_PIPE_LOCAL &&
        m_interfaceOptions["acceptRemote"].toInt()) {

        bool done = startWebsocketServer();
        if (!done) return ERR_DEVICE_NOT_READY;
    }

    // start a websocket client for remote pipe
    if (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE) {

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
    if (m_websocketServer != nullptr) {
        m_websocketServer->sendTextMessage(signalToMessage(data, m_endpointInfoList[endpointIndex].channel));
        m_websocketServer->flush();
    }

    if (m_websocketClient != nullptr) {
        m_websocketClient->sendTextMessage(signalToMessage(data, m_endpointInfoList[endpointIndex].channel));
        m_websocketClient->flush();
    }

    // for local pipe
    emit dataReceived(endpointIndex, data);
    return ERR_NONE;
}

bool DgsPipeInterface::startWebsocketServer()
{
    if (m_server != nullptr) return false;

    m_server = new QWebSocketServer(QString("digishow"), QWebSocketServer::NonSecureMode);
    m_websocketServer = nullptr;

    int websocketPort = m_interfaceOptions["tcpPort"].toInt();
    if (!m_server->listen(QHostAddress::Any, websocketPort)) return false;

    connect(m_server, SIGNAL(newConnection()), this, SLOT(onWebsocketServerNewConnection()));
    return true;
}

void DgsPipeInterface::stopWebsocketServer()
{
    if (m_websocketServer != nullptr) {
        m_websocketServer->close();
        delete m_websocketServer;
        m_websocketServer = nullptr;
    }

    if (m_server != nullptr) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
}

bool DgsPipeInterface::startWebsocketClient()
{
    if (m_websocketClient != nullptr) return false;

    QWebSocket *websocket = new QWebSocket();

    connect(websocket, SIGNAL(connected()), this, SLOT(onWebsocketClientConnected()));
    connect(websocket, SIGNAL(disconnected()), this, SLOT(onWebsocketClientDisconnected()));
    connect(websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWebsocketClientMessageReceived(QString)));

    QString url = QString("ws://%1:%2")
            .arg(m_interfaceOptions["tcpHost"].toString())
            .arg(m_interfaceOptions["tcpPort"].toInt());

    websocket->open(QUrl(url));

    // wait and confirm the websocket is connected
    QElapsedTimer timer;
    timer.start();
    while (!timer.hasExpired(3000)) {
        qApp->processEvents(QEventLoop::AllEvents);
        if (m_websocketClient == websocket) return true;
    }

    return false;
}

void DgsPipeInterface::stopWebsocketClient()
{
    if (m_websocketClient != nullptr) {
        m_websocketClient->close();
        delete m_websocketClient;
        m_websocketClient = nullptr;
    }
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
    dgsSignalData data;
    int channel;
    if (!messageToSignal(message, &data, &channel)) return;

    int type = -1;
    switch (data.signal) {
    case 'A': type = ENDPOINT_PIPE_ANALOG; break;
    case 'B': type = ENDPOINT_PIPE_BINARY; break;
    case 'N': type = ENDPOINT_PIPE_NOTE;   break;
    }
    if (type==-1) return;

    int endpointIndex = findEndpoint(type, channel);
    if (endpointIndex==-1) return;

    // for local pipe
    emit dataReceived(endpointIndex, data);
}

void DgsPipeInterface::onWebsocketServerNewConnection()
{
    // disconnect old websocket session
    if (m_websocketServer != nullptr) {
        m_websocketServer->close();
        delete m_websocketServer;
        m_websocketServer = nullptr;
    }

    // accept new websocket session
    QWebSocket *websocket = m_server->nextPendingConnection();

    connect(websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWebsocketServerMessageReceived(QString)));
    connect(websocket, SIGNAL(disconnected()), this, SLOT(onWebsocketServerDisconnected()));

    qDebug("[server] websocket request %s is accepted.",
           websocket->requestUrl().toDisplayString().toLocal8Bit().constData());
    qDebug("[server] websocket %p is connected.", websocket);

    m_websocketServer = websocket;
}

void DgsPipeInterface::onWebsocketServerDisconnected()
{
    QWebSocket *websocket = qobject_cast<QWebSocket *>(sender());
    qDebug("[server] websocket %p is disconnected.", websocket);

    if (websocket == m_websocketServer) m_websocketServer = nullptr;
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

    m_websocketClient = websocket;
}

void DgsPipeInterface::onWebsocketClientDisconnected()
{
    QWebSocket *websocket = qobject_cast<QWebSocket *>(sender());
    qDebug("[client] websocket %p is disconnected.", websocket);

    if (websocket == m_websocketClient) m_websocketClient = nullptr;
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

