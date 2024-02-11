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
#include "digishow_pixel_player.h"

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

        // load media and initialize pixel players
        m_players.clear();
        QVariantList mediaList = cleanMediaList();
        for (int n=0 ; n<mediaList.length() ; n++) initPlayer(mediaList[n].toMap());
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsArtnetInterface::closeInterface()
{
    // release pixel players
    QStringList playerNames = m_players.keys();
    int playerCount = playerNames.length();
    for (int n=0 ; n<playerCount ; n++) {
        QString key = playerNames[n];
        DigishowPixelPlayer* player = m_players[key];
        player->stop();
        delete player;
        m_players.remove(key);
    }

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

    if (m_endpointInfoList[endpointIndex].type == ENDPOINT_ARTNET_DIMMER) {

        // dimmer control

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        int value = 255 * data.aValue / (data.aRange==0 ? 255 : data.aRange);
        if (value<0 || value>255) return ERR_INVALID_DATA;

        int universe = m_endpointInfoList[endpointIndex].unit;
        int channel = m_endpointInfoList[endpointIndex].channel;

        // update dmx data buffer
        if (!m_dataAll.contains(universe)) m_dataAll[universe] = QByteArray(512, 0x00);
        m_dataAll[universe].data()[channel] = static_cast<unsigned char>(value);

    } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_ARTNET_DIMMER2) {

        // 16-bit dimmer control

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        int value = int(65535 * uint32_t(data.aValue) / uint32_t(data.aRange==0 ? 65535 : data.aRange));
        if (value<0 || value>65535) return ERR_INVALID_DATA;

        int universe = m_endpointInfoList[endpointIndex].unit;
        int channel = m_endpointInfoList[endpointIndex].channel;

        // update dmx data buffer
        if (!m_dataAll.contains(universe)) m_dataAll[universe] = QByteArray(512, 0x00);
        m_dataAll[universe].data()[channel] = static_cast<unsigned char>(value >> 8);   // MSB

        channel++;
        if (channel < 512)
        m_dataAll[universe].data()[channel] = static_cast<unsigned char>(value & 0xff); // LSB

    } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_ARTNET_MEDIA) {

        // media playback control

        int control = m_endpointInfoList[endpointIndex].control;
        QVariantMap endpointOptions = m_endpointOptionsList[endpointIndex];
        QString media = endpointOptions.value("media").toString();

        if (control == CONTROL_MEDIA_START) {

            DigishowPixelPlayer *player = m_players.value(media, nullptr);
            if (player == nullptr) return ERR_DEVICE_NOT_READY;

            if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
            if (data.bValue) {

                // set player pixel mapping
                setupPlayerPixelMapping(player, media);

                // set player playback attributes
                player->setFadeIn(endpointOptions.value("mediaFadeIn").toInt());
                player->setVolume(endpointOptions.value("mediaVolume", QVariant(10000)).toInt() / 10000.0);
                player->setSpeed(endpointOptions.value("mediaSpeed", QVariant(10000)).toInt() / 10000.0);
                player->setPosition(endpointOptions.value("mediaPosition").toInt());
                player->setDuration(endpointOptions.value("mediaDuration").toInt());
                player->setRepeat(endpointOptions.value("mediaRepeat").toBool());

                if (endpointOptions.value("mediaAlone", QVariant(true)).toBool()) stopAll();
                player->play();
            }

        } else if (control == CONTROL_MEDIA_STOP) {

            DigishowPixelPlayer *player = m_players.value(media, nullptr);
            if (player == nullptr) return ERR_DEVICE_NOT_READY;

            if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
            if (data.bValue) player->stop();

        } else if (control == CONTROL_MEDIA_STOP_ALL) {

            if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
            if (data.bValue) stopAll();
        }
    }

    return ERR_NONE;
}

int DgsArtnetInterface::loadMedia(const QVariantMap &mediaOptions)
{
    int r = DigishowInterface::loadMedia(mediaOptions);
    if ( r != ERR_NONE) return r;

    bool done = initPlayer(mediaOptions);
    if (!done) return ERR_INVALID_DATA;

    return ERR_NONE;
}

bool DgsArtnetInterface::initPlayer(const QVariantMap &mediaOptions)
{
    QString name = mediaOptions.value("name").toString();
    QString type = mediaOptions.value("type").toString();
    QString url  = mediaOptions.value("url" ).toString();

    int mediaType = DigishowPixelPlayer::MediaUnknown;
    if (type == "image") mediaType = DigishowPixelPlayer::MediaImage; else
    if (type == "video") mediaType = DigishowPixelPlayer::MediaVideo; else
    if (type == "ini"  ) mediaType = DigishowPixelPlayer::MediaImageSequence;

    bool done = false;
    if (!name.isEmpty() && !url.isEmpty() && mediaType != DigishowPixelPlayer::MediaUnknown) {
        DigishowPixelPlayer *player = new DigishowPixelPlayer();
        if (player->load(url, mediaType)) {
            m_players[name] = player;
            connect(player, SIGNAL(frameUpdated()), this, SLOT(onPlayerFrameUpdated()));
        } else {
            delete player;
        }
    }

    return done;
}

void DgsArtnetInterface::stopAll()
{
    QStringList playerNames = m_players.keys();
    for (int n=0 ; n<playerNames.count() ; n++) {
        QString key = playerNames[n];
        DigishowPixelPlayer *player = m_players.value(key, nullptr);
        if (player != nullptr) player->stop();
    }
}

void DgsArtnetInterface::setupPlayerPixelMapping(DigishowPixelPlayer *player, const QString &mediaName)
{
    player->clearPixelMapping();

    QString interfaceName = m_interfaceOptions.value("name").toString();
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        QVariantMap endpointOptions = m_endpointOptionsList[n];

        // look for all endpoints with the specified media
        if (endpointInfo.type != ENDPOINT_ARTNET_MEDIA ||
            endpointInfo.control != CONTROL_MEDIA_START ||
            endpointOptions.value("media") != mediaName) continue;

        QString endpointName = endpointOptions.value("name").toString();
        if (!g_app->confirmEndpointIsEmployed(interfaceName, endpointName)) continue;

        int pixelMode = DigishowPixelPlayer::pixelMode(endpointOptions.value("mediaPixelMode").toString());
        if (pixelMode == DigishowPixelPlayer::PixelUnknown) continue;
        int bytesPerPixel = (pixelMode == DigishowPixelPlayer::PixelMono ? 1 : 3);

        int pixelCountX = endpointOptions.value("mediaPixelCountX").toInt();
        int pixelCountY = endpointOptions.value("mediaPixelCountY").toInt();
        int pixelCount = pixelCountX * pixelCountY;

        int universe = endpointInfo.unit;
        int channel  = endpointInfo.channel;

        // prepare dmx data buffer for the universe
        if (!m_dataAll.contains(universe)) m_dataAll[universe] = QByteArray(512, 0x00);

        // the number of pixels need be mapped to the universe
        pixelCount = qMin(pixelCount, (512-channel)/bytesPerPixel);

        // map pixels to the universe
        dppPixelMapping mapping;
        mapping.pixelMode = pixelMode;
        mapping.pixelCountX = pixelCountX;
        mapping.pixelCountY = pixelCountY;
        mapping.pixelOffsetX = endpointOptions.value("mediaPixelOffsetX").toInt();
        mapping.pixelOffsetY = endpointOptions.value("mediaPixelOffsetY").toInt();
        mapping.pixelSpacingX = endpointOptions.value("mediaPixelSpacingX").toInt();
        mapping.pixelSpacingY = endpointOptions.value("mediaPixelSpacingY").toInt();
        mapping.mappingMode = endpointOptions.value("mediaMappingMode").toInt();
        mapping.dataOutPixelCount = pixelCount;
        mapping.pDataOut = (uint8_t*)m_dataAll[universe].data() + channel;
        player->addPixelMapping(mapping);
    }
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
                int type    = m_endpointInfoList[n].type;

                if (type == ENDPOINT_ARTNET_DIMMER && unit == universe && channel < length) {

                    unsigned char value = packetBytes[channel+18];
                    bool changed = !m_dataAll.contains(universe) || m_dataAll[universe].data()[channel] != value;

                    // emit artnet input signal
                    if (changed) {
                        dgsSignalData data;
                        data.signal = DATA_SIGNAL_ANALOG;
                        data.aRange = 255;
                        data.aValue = value;
                        data.bValue = 0;

                        emit dataReceived(n, data);
                    }

                } else if (type == ENDPOINT_ARTNET_DIMMER2 && unit == universe && channel < length-1) {

                    unsigned char msb = packetBytes[channel+18];
                    unsigned char lsb = packetBytes[channel+19];
                    bool changed = !m_dataAll.contains(universe) || m_dataAll[universe].data()[channel] != msb || m_dataAll[universe].data()[channel+1] != lsb;

                    // emit artnet input signal
                    if (changed) {
                        int value = (msb << 8) | lsb;
                        dgsSignalData data;
                        data.signal = DATA_SIGNAL_ANALOG;
                        data.aRange = 65535;
                        data.aValue = value;
                        data.bValue = 0;

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

void DgsArtnetInterface::onPlayerFrameUpdated()
{
    DigishowPixelPlayer *player = qobject_cast<DigishowPixelPlayer*>(sender());
    player->transferFrameAllMappedPixels();
}
