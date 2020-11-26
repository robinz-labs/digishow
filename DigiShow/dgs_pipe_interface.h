#ifndef DGSPIPEINTERFACE_H
#define DGSPIPEINTERFACE_H

#include <QWebSocketServer>
#include <QWebSocket>
#include "digishow_interface.h"

class DgsPipeInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsPipeInterface(QObject *parent = nullptr);
    ~DgsPipeInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

public slots:

    void onWebsocketServerNewConnection();
    void onWebsocketServerDisconnected();
    void onWebsocketServerMessageReceived(const QString & message);

    void onWebsocketClientConnected();
    void onWebsocketClientDisconnected();
    void onWebsocketClientMessageReceived(const QString & message);

private:

    QWebSocketServer *m_server;
    QWebSocket *m_websocketServer;
    QWebSocket *m_websocketClient;

    // local mode with a websocket server
    bool startWebsocketServer();
    void stopWebsocketServer();

    // remote mode with a websocket client
    bool startWebsocketClient();
    void stopWebsocketClient();

    // process received websocket messages
    int findEndpoint(int type, int channel);
    void processReceivedMessage(const QString &message);

    // signal data to text message conversion
    static QString signalToMessage(dgsSignalData data, int channel);
    static bool messageToSignal(const QString &message, dgsSignalData *data, int *channel);

};


#endif // DGSPIPEINTERFACE_H
