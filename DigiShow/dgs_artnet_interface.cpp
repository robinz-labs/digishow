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

#include "dgs_artnet_interface.h"

#define ARTNET_UDP_PORT 6454
#define ARTNET_OUT_FREQ 30

DgsArtnetInterface::DgsArtnetInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "artnet";
    m_udp = nullptr;
    m_timer = nullptr;
    m_udpHost = QHostAddress::LocalHost;
    m_udpPort = ARTNET_UDP_PORT;

    // clear dmx data buffer
    m_dataAll.clear();
    m_sequence = 0;
}


DgsArtnetInterface::~DgsArtnetInterface()
{
    closeInterface();
}

int DgsArtnetInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get artnet interface configuration
    QString udpHost = m_interfaceOptions.value("udpHost").toString();
    int     udpPort = m_interfaceOptions.value("udpPort").toInt();

    m_udpHost = QHostAddress::LocalHost;
    m_udpPort = ARTNET_UDP_PORT;

    if (!udpHost.isEmpty()) m_udpHost = QHostAddress(udpHost);
    if ( udpPort > 0      ) m_udpPort = udpPort;

    // create an udp socket for artnet
    m_udp = new QUdpSocket();
    connect(m_udp, SIGNAL(readyRead()), this, SLOT(onUdpDataReceived()));

    if (m_interfaceInfo.mode == INTERFACE_ARTNET_INPUT) {

        // for artnet receiver

        m_dataAll.clear();
        m_udp->bind(m_udpPort);

    } else if (m_interfaceInfo.mode == INTERFACE_ARTNET_OUTPUT) {

        // for artnet sender

        //m_dataAll.clear();
        m_sequence = 0;

        // broadcast an artnet poll packet
        const unsigned char bytes[] = {
            0x41, 0x72, 0x74, 0x2d, 0x4e, 0x65, 0x74, 0x00,  // Art-Net
            0x00, 0x20, // opcode ARTNET_POLL
            0x00, 0x0e, // version
            0x00, 0x00  // take-to-me
        };
        m_udp->writeDatagram((const char*)bytes, sizeof(bytes),
                             m_udpHost, //QHostAddress::Broadcast,
                             (quint16)m_udpPort);

        // create a timer for sending dmx frames at a particular frequency
        int frequency = m_interfaceOptions.value("frequency").toInt();
        if (frequency == 0) frequency = ARTNET_OUT_FREQ;

        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
        m_timer->setTimerType(Qt::PreciseTimer);
        m_timer->setSingleShot(false);
        m_timer->setInterval(1000 / frequency);
        m_timer->start();
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsArtnetInterface::closeInterface()
{
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    if (m_udp != nullptr) {
        m_udp->close();
        delete m_udp;
        m_udp = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsArtnetInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

    int value = 255 * data.aValue / (data.aRange==0 ? 255 : data.aRange);
    if (value<0 || value>255) return ERR_INVALID_DATA;

    int universe = m_endpointInfoList[endpointIndex].unit;
    int channel = m_endpointInfoList[endpointIndex].channel;

    // update dmx data buffer
    if (!m_dataAll.contains(universe)) m_dataAll[universe] = QByteArray(512, 0x00);
    m_dataAll[universe].data()[channel] = static_cast<unsigned char>(value);

    return ERR_NONE;
}

void DgsArtnetInterface::onUdpDataReceived()
{
    static const unsigned char artnetDmxPacketHeaderBytes[] = {
        0x41, 0x72, 0x74, 0x2d, 0x4e, 0x65, 0x74, 0x00,  // Art-Net
        0x00, 0x50, // opcode ARTNET_DMX
        0x00, 0x0e  // version
    };
    static QByteArray artnetDmxPacketHeader((const char*)artnetDmxPacketHeaderBytes, sizeof(artnetDmxPacketHeaderBytes));

    QByteArray datagram;
    QHostAddress host;
    quint16 port;

    do {
        datagram.resize(int(m_udp->pendingDatagramSize()));
        m_udp->readDatagram(datagram.data(), datagram.size(), &host, &port);

        //#ifdef QT_DEBUG
        //qDebug() << "artnet udp received: " << datagram.toHex();
        //#endif

        const unsigned char* packetBytes = (const unsigned char*)datagram.constData();

        // process artnet dmx packet
        if (datagram.startsWith(artnetDmxPacketHeader) && datagram.size() > 18) {

            int universe = int(packetBytes[14]) + int(packetBytes[15])*0x0100;
            int length   = int(packetBytes[17]) + int(packetBytes[16])*0x0100;

            // confirm the datagram contains valid dmx data
            if (length < 1 || length > 512 || datagram.size() != length+18) break;

            // determine which endpoint value is changed
            for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

                int unit    = m_endpointInfoList[n].unit;
                int channel = m_endpointInfoList[n].channel;

                if (unit == universe && channel < length) {

                    unsigned char value = packetBytes[channel+18];
                    bool changed = !m_dataAll.contains(universe) || m_dataAll[universe].data()[channel] != value;

                    // emit artnet input signal
                    if (changed) {
                        dgsSignalData data;
                        data.signal = DATA_SIGNAL_ANALOG;
                        data.aRange = 255;
                        data.aValue = value;
                        data.bValue = 0;

                        //qDebug() << "artnet_input:" << n << data.signal << data.aValue << data.bValue;
                        emit dataReceived(n, data);
                    }
                }
            }

            // update dmx data buffer
            if (!m_dataAll.contains(universe)) m_dataAll[universe] = QByteArray(512, 0x00);
            memcpy(m_dataAll[universe].data(), packetBytes+18, length);
        }

    } while (m_udp->hasPendingDatagrams());

}

void DgsArtnetInterface::onTimerFired()
{
    if (m_udp->bytesToWrite() > 0) return;

    QMap<int, QByteArray>::iterator d; // a 512-byte dmx data item
    for (d = m_dataAll.begin(); d != m_dataAll.end(); ++d) {

        int universe = d.key();

        // update sequence number
        m_sequence++;
        if (m_sequence == 0) m_sequence = 1;

        // send an artnet dmx output packet
        const unsigned char bytes[] = {
            0x41, 0x72, 0x74, 0x2d, 0x4e, 0x65, 0x74, 0x00,  // Art-Net
            0x00, 0x50, // opcode ARTNET_DMX
            0x00, 0x0e, // version
            m_sequence, // sequence
            0x00,       // physical
            static_cast<unsigned char>(universe & 0xff), // universe LSB
            static_cast<unsigned char>(universe >> 8),   // universe MSB
            0x02, 0x00  // length (512)
        };

        QByteArray datagram = QByteArray((const char*)bytes, sizeof(bytes)).append(d.value()); // header + 512-byte dmx data

        m_udp->writeDatagram(datagram.constData(), datagram.size(),
                             m_udpHost, (quint16)m_udpPort);

    }
}
