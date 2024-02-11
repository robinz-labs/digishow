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

#include "dgs_dmx_interface.h"
#include "com_handler.h"
#include "digishow_pixel_player.h"

#include <QSerialPortInfo>

// FTDI USB
#define FTDI_VID 1027
#define FTDI_PID 24577

// digishow compatible widget USB
#define DGSW_VID 0xF000
#define DGSW_PID 0x1000

#define DMX_OUT_FREQ 30

DgsDmxInterface::DgsDmxInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "dmx";
    m_com = nullptr;
    m_timer = nullptr;

    // clear dmx data buffer
    for (int n=0 ; n<512 ; n++) m_data[n]=0;
}

DgsDmxInterface::~DgsDmxInterface()
{
    closeInterface();
}

int DgsDmxInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get com port configuration
    QString comPort = m_interfaceOptions.value("comPort").toString();
    if (comPort.isEmpty()) comPort = ComHandler::findPort(DGSW_VID, DGSW_PID); // use default port
    if (comPort.isEmpty()) comPort = ComHandler::findPort(FTDI_VID, FTDI_PID);
    if (comPort.isEmpty()) return ERR_INVALID_OPTION;

    // get number of total dmx channels
    int channels = 0;
    for (int n = 0 ; n<m_endpointInfoList.length() ; n++) {
        int type = m_endpointInfoList[n].type;
        if (type == ENDPOINT_DMX_DIMMER) {
            if (m_endpointInfoList[n].channel+1 > channels) channels = m_endpointInfoList[n].channel+1;
        } else if (type == ENDPOINT_DMX_DIMMER2) {
            if (m_endpointInfoList[n].channel+2 > channels) channels = m_endpointInfoList[n].channel+2;
        } else {
            channels = 512;
            break;
        }
    }
    channels = qMin(channels, 512);

    // create a com handler for the serial communication with the device
    bool done = false;
    m_com = new ComHandler();

    done = enttecDmxOpen(comPort, channels);

    // create a timer for sending dmx frames at a particular frequency
    int frequency = m_interfaceOptions.value("frequency").toInt();
    if (frequency == 0) frequency = DMX_OUT_FREQ;

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

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsDmxInterface::closeInterface()
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

    if (m_com != nullptr) {
        m_com->close();
        delete m_com;
        m_com = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;

}

int DgsDmxInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (m_endpointInfoList[endpointIndex].type == ENDPOINT_DMX_DIMMER) {

        // dimmer control

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        int value = 255 * data.aValue / (data.aRange==0 ? 255 : data.aRange);
        if (value<0 || value>255) return ERR_INVALID_DATA;

        // update dmx data buffer
        int channel = m_endpointInfoList[endpointIndex].channel;
        m_data[channel] = static_cast<unsigned char>(value);


    } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_DMX_DIMMER2) {

        // 16-bit dimmer control

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        int value = int(65535 * uint32_t(data.aValue) / uint32_t(data.aRange==0 ? 65535 : data.aRange));
        if (value<0 || value>65535) return ERR_INVALID_DATA;

        // update dmx data buffer
        int channel = m_endpointInfoList[endpointIndex].channel;
        m_data[channel] = static_cast<unsigned char>(value >> 8);   // MSB

        channel++;
        if (channel < 512)
        m_data[channel] = static_cast<unsigned char>(value & 0xff); // LSB

    } else if (m_endpointInfoList[endpointIndex].type == ENDPOINT_DMX_MEDIA) {

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

int DgsDmxInterface::loadMedia(const QVariantMap &mediaOptions)
{
    int r = DigishowInterface::loadMedia(mediaOptions);
    if ( r != ERR_NONE) return r;

    bool done = initPlayer(mediaOptions);
    if (!done) return ERR_INVALID_DATA;

    return ERR_NONE;
}

bool DgsDmxInterface::initPlayer(const QVariantMap &mediaOptions)
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

void DgsDmxInterface::stopAll()
{
    QStringList playerNames = m_players.keys();
    for (int n=0 ; n<playerNames.count() ; n++) {
        QString key = playerNames[n];
        DigishowPixelPlayer *player = m_players.value(key, nullptr);
        if (player != nullptr) player->stop();
    }
}

void DgsDmxInterface::setupPlayerPixelMapping(DigishowPixelPlayer *player, const QString &mediaName)
{
    player->clearPixelMapping();

    QString interfaceName = m_interfaceOptions.value("name").toString();
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        QVariantMap endpointOptions = m_endpointOptionsList[n];

        // look for all endpoints with the specified media
        if (endpointInfo.type != ENDPOINT_DMX_MEDIA ||
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

        int channel  = endpointInfo.channel;

        // the number of pixels need be mapped to the dmx buffer
        pixelCount = qMin(pixelCount, (512-channel)/bytesPerPixel);

        // map pixels to the dmx buffer
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
        mapping.pDataOut = (uint8_t*)m_data + channel;
        player->addPixelMapping(mapping);
    }
}

void DgsDmxInterface::onTimerFired()
{
    // send dmx data frame
    enttecDmxSendDmxFrame(m_data);
}

void DgsDmxInterface::onPlayerFrameUpdated()
{
    DigishowPixelPlayer *player = qobject_cast<DigishowPixelPlayer*>(sender());
    player->transferFrameAllMappedPixels();
}

bool DgsDmxInterface::enttecDmxOpen(const QString &port, int channels)
{
    // open serial port of the dmx adapter
    m_com->setAsyncReceiver(true);

    bool done = false;
    if (m_interfaceInfo.mode == INTERFACE_DMX_ENTTEC_PRO) {
        done = m_com->open(port.toLocal8Bit(), 115200, CH_SETTING_8N1);
    } else
    if (m_interfaceInfo.mode == INTERFACE_DMX_ENTTEC_OPEN) {

        // !!! experimental only !!!
        // Need to use libFTDI/FTD2XX to replace QSerialPort for synchronous serial communication with Open DMX USB

        done = m_com->open(port.toLocal8Bit(), 250000, CH_SETTING_8N2);
    }
    if (!done) return false;

    // set DTR on
     m_com->serialPort()->setDataTerminalReady(true);

    // clear dmx data buffer
    //for (int n=0 ; n<512 ; n++) m_data[n]=0;

    // set number of channels
    m_channels = (channels+7) / 8 * 8;
    m_channels = qBound(24, m_channels, 512);

    int vid, pid;
    if (getUsbVidPid(&vid, &pid) && vid == DGSW_VID && pid == DGSW_PID) {
        m_channels = qMin(m_channels, 360);
    }

    return true;
}

bool DgsDmxInterface::enttecDmxSendDmxFrame(unsigned char *data)
{
    if (m_channels < 24 || m_channels > 512) return false;

    // send frame
    if (!m_com->isBusySending()) {

        if (m_interfaceInfo.mode == INTERFACE_DMX_ENTTEC_PRO) {

            // send an enttec pro frame
            bool done = true;

            int length = m_channels;
            unsigned char head[] = {
                0x7e, // 0  leading character
                0x06, // 1  message type (6 = Output Only Send DMX Packet Request)
                0x00, // 2  data length LSB
                0x00, // 3  data length MSB
                0x00  // 4  start code (always zero)
            };
            head[2] = static_cast<unsigned char>((length+1) & 0xff);
            head[3] = static_cast<unsigned char>((length+1) >> 8);

            unsigned char tail[] = { 0xe7 };

            done &= m_com->sendBytes((const char*)head, sizeof(head), false);
            done &= m_com->sendBytes((const char*)data, length, false);
            done &= m_com->sendBytes((const char*)tail, sizeof(tail));

            return done;

        } else if (m_interfaceInfo.mode == INTERFACE_DMX_ENTTEC_OPEN) {

            // !!! experimental only !!!
            // Need to use libFTDI/FTD2XX to replace QSerialPort for synchronous serial communication with Open DMX USB

            // send an open dmx frame
            bool done = true;

            // The start-of-packet procedure is a logic zero for more than 22 bit periods (>88usec),
            // followed by a logic 1 for more than 2 bit periods (>8usec).
            m_com->serialPort()->setBreakEnabled(false);
            QThread::usleep(88);
            m_com->serialPort()->setBreakEnabled(true);
            QThread::usleep(8);
            m_com->serialPort()->setBreakEnabled(false);

            m_com->serialPort()->setRequestToSend(true);
            unsigned char head[] = { 0x00 };
            done &= m_com->sendBytes((const char*)head, sizeof(head), false);
            done &= m_com->sendBytes((const char*)data, 512, true);
            m_com->serialPort()->setRequestToSend(false);

            m_com->serialPort()->setBreakEnabled(true);

            return done;
        }

    }

    return false;
}

bool DgsDmxInterface::getUsbVidPid(int *vid, int *pid)
{
    QString portName = m_com->serialPort()->portName();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier() && serialPortInfo.portName() == portName) {
            *vid = serialPortInfo.vendorIdentifier();
            *pid = serialPortInfo.productIdentifier();
            return true;
        }
    }
    return false;
}


QVariantList DgsDmxInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

#ifdef Q_OS_MAC
        if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif

        info.clear();
        info["comPort"] = serialPortInfo.portName();

        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier() && ((
            serialPortInfo.vendorIdentifier()==FTDI_VID && serialPortInfo.productIdentifier()==FTDI_PID ) || (
            serialPortInfo.vendorIdentifier()==DGSW_VID && serialPortInfo.productIdentifier()==DGSW_PID ))) {

            info["mode"] = "enttec";
        } else {
            info["mode"] = "general";
        }

        list.append(info);
    }

    return list;
}

