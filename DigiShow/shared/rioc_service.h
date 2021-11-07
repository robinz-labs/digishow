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

#ifndef RIOCSERVICE_H
#define RIOCSERVICE_H

#include <QtCore>
#include <QElapsedTimer>

class ComHandler;

class RiocService : public QObject
{
    Q_OBJECT
public:
    explicit RiocService(QObject *parent = 0);
    virtual ~RiocService();

    void setLoggerEnabled(bool enable) { _isLoggerEnanbled = enable; }

    bool addSerialConnection(const QString & serialPort, int serialBaud = 9600);
    void clearSerialConnections();
    int getSerialCount();

    void sendRiocMessage(unsigned char fromID, unsigned char toID, const QByteArray & data);
    bool sendRiocMessageAndWaitResponse(unsigned char fromID, unsigned char toID,
                                        const QByteArray & dataOut, QByteArray & dataIn,
                                        double timeout = 0.6);

signals:
    void riocMessageReceived(unsigned char fromID, unsigned char toID, const QByteArray & data);

private slots:
    void handleSerialBytesReceived(ComHandler* serial);

private:
    bool _isLoggerEnanbled;
    QList<ComHandler*> _serials;

    void sendSerialMessage(const QByteArray & message);

    // for receiving specific message
    bool _isSpecificIncomingMessageReceived;
    unsigned char _specificIncomingMessageFromID;
    QByteArray _specificIncomingMessageFilter;
    QByteArray _specificIncomingMessageReceived;

    QElapsedTimer _elapsedTimer;
    double getCurrentSecond();

    unsigned char checksum(unsigned char *bytes, int length);
};

#endif // RIOCSERVICE_H
