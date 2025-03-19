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

#include "tcp_messager.h"

// Constructor: Initialize TCP socket and setup signal connections
TcpMessager::TcpMessager(QObject *parent)
    : AbstractMessager(parent)
    , m_tcpSocket(new QTcpSocket(this))
{
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &TcpMessager::handleReadyRead);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &TcpMessager::connected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &TcpMessager::disconnected);
}

// Destructor: Ensure proper disconnection
TcpMessager::~TcpMessager()
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->disconnectFromHost();
    }
}

// Open connection to specified host and port
bool TcpMessager::open(const QString &hostName, quint16 port)
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->disconnectFromHost();
    }

    m_tcpSocket->connectToHost(hostName, port);
    return m_tcpSocket->waitForConnected();
}

// Close the current connection
void TcpMessager::close()
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->disconnectFromHost();
    }
}

// Send message through TCP connection
bool TcpMessager::sendMessage(const QByteArray &message)
{
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        return false;
    }
    
    qint64 written = m_tcpSocket->write(message);
    return written == message.size();
}

// Handle incoming data
void TcpMessager::handleReadyRead()
{
    QByteArray newData = m_tcpSocket->readAll();
    emit rawDataReceived(newData);
    
    m_buffer.append(newData);

    bool found = true;
    while (found) {
        found = false;
        int earliestPos = m_buffer.size();
        int foundIndex = -1;

        // Find the earliest message in buffer
        for (int i = 0; i < m_subscribedMessages.size(); ++i) {
            const QByteArray &message = m_subscribedMessages[i];
            int pos = m_buffer.indexOf(message);
            if (pos != -1 && pos < earliestPos) {
                earliestPos = pos;
                foundIndex = i;
                found = true;
            }
        }

        // Process the earliest message if found
        if (found) {
            emit messageReceived(foundIndex);
            const QByteArray &message = m_subscribedMessages[foundIndex];
            m_buffer.remove(earliestPos, message.size());
        }
    }

    // Prevent buffer from growing indefinitely
    if (m_buffer.size() > 1024) {
        m_buffer.remove(0, m_buffer.size() - 1024);
    }
}