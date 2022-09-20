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

#ifndef DGSARTNETINTERFACE_H
#define DGSARTNETINTERFACE_H

#include <QObject>
#include <QUdpSocket>
#include "digishow_interface.h"

#ifdef DIGISHOW_EXPERIMENTAL
#define ARTNET_PIXEL_PLAYER_ENABLED
#endif

#ifdef ARTNET_PIXEL_PLAYER_ENABLED
class DigishowPixelPlayer;
#endif

class DgsArtnetInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsArtnetInterface(QObject *parent = nullptr);
    ~DgsArtnetInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

#ifdef ARTNET_PIXEL_PLAYER_ENABLED
    int loadMedia(const QVariantMap &mediaOptions) override;
#endif

signals:

public slots:
    void onUdpDataReceived();    // for input
    void onTimerFired();         // for output

#ifdef ARTNET_PIXEL_PLAYER_ENABLED
    void onPlayerFrameUpdated();
#endif

private:

    QUdpSocket *m_udp;
    QHostAddress m_udpHost;
    int m_udpPort;

    // timer for artnet dmx output
    QTimer *m_timer;

    // buffer for dmx data of all universes
    // key: universe number
    // val: 512 bytes of dmx data
    QMap<int, QByteArray> m_dataAll;

    // sequence number for artnet dmx output ( 0x01 ~ 0xff )
    unsigned char m_sequence;

#ifdef ARTNET_PIXEL_PLAYER_ENABLED

    // players hold all media
    QMap<QString, DigishowPixelPlayer*> m_players;
    bool initPlayer(const QVariantMap &mediaOptions);
    void stopAll();
    void setupPlayerPixelMapping(DigishowPixelPlayer *player, const QString &mediaName);

#endif

};

#endif // DGSARTNETINTERFACE_H
