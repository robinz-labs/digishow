/*
    Copyright 2025 Robin Zhang & Labs

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

/*
    NOTE: 
    The code in this file was co-written by AI (Trae/Claude-3.5-Sonnet).
*/

#ifndef TCP_MESSAGER_H
#define TCP_MESSAGER_H

#include "abstract_messager.h"
#include <QTcpSocket>

class TcpMessager : public AbstractMessager
{
    Q_OBJECT

public:
    explicit TcpMessager(QObject *parent = nullptr);
    ~TcpMessager();

    bool open(const QString &hostName, quint16 port);
    void close() override;
    bool sendMessage(const QByteArray &message) override;

private slots:
    void handleReadyRead();

private:
    QTcpSocket *m_tcpSocket;
    QByteArray m_buffer;
};

#endif // TCP_MESSAGER_H