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

    // server mode
    QWebSocketServer *m_websocketServer;
    QList<QWebSocket*> m_websocketServerConnections;
    bool m_websocketServerMultiple;

    bool startWebsocketServer();
    void stopWebsocketServer();

    // client mode
    QWebSocket *m_websocketClientConnection;

    bool startWebsocketClient();
    void stopWebsocketClient();

    // send websocket message
    void sendWebsocketMessage(const QString &message);

    // process received websocket messages
    int findEndpoint(int type, int channel);
    void processReceivedMessage(const QString &message);

    // signal data to text message conversion
    static QString signalToMessage(dgsSignalData data, int channel);
    static bool messageToSignal(const QString &message, dgsSignalData *data, int *channel);

};


#endif // DGSPIPEINTERFACE_H
