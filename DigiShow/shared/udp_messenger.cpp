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

#include "udp_messenger.h"

// Constructor: Initialize UDP socket
UdpMessenger::UdpMessenger(QObject *parent)
    : AbstractMessenger(parent)
    , m_udpSocket(new QUdpSocket(this))
    , m_remotePort(0)
{
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpMessenger::handleReadyRead);
}

// Destructor: Ensure proper cleanup
UdpMessenger::~UdpMessenger()
{
    close();
}

// Open UDP socket with specified local port and remote endpoint
bool UdpMessenger::open(quint16 localPort, const QString &remoteHost, quint16 remotePort)
{
    close();
    
    m_remoteHost = remoteHost;
    m_remotePort = remotePort;

    if (localPort == 0) return true; // No binding required if local port is not specified

    bool success = m_udpSocket->bind(QHostAddress::Any, localPort);
    if (success) {
        emit connected();
    }
    return success;
}

// Close the UDP socket
void UdpMessenger::close()
{
    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        m_udpSocket->close();
        emit disconnected();
    }
}

// Send message to the configured remote endpoint
bool UdpMessenger::sendMessage(const QByteArray &message)
{
    if (m_remotePort == 0) {
        return false;
    }
    
    qint64 written = m_udpSocket->writeDatagram(message, QHostAddress(m_remoteHost), m_remotePort);
    return written == message.size();
}

// Handle incoming UDP datagrams
void UdpMessenger::handleReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        
        emit rawDataReceived(datagram);
        
        // Compare received datagram with subscribed messages
        for (int i = 0; i < m_subscribedMessages.size(); ++i) {
            if (datagram == m_subscribedMessages[i]) {
                emit messageReceived(i);
                break;
            }
        }
    }
}