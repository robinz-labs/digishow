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

#ifndef DGSDMXINTERFACE_H
#define DGSDMXINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class ComHandler;
class DigishowPixelPlayer;

class DgsDmxInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsDmxInterface(QObject *parent = nullptr);
    ~DgsDmxInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    // for pixel player
    int loadMedia(const QVariantMap &mediaOptions) override;

    static QVariantList listOnline();

signals:

public slots:
    void onTimerFired();         // for dmx output
    void onPlayerFrameUpdated(); // for pixel player

private:
    ComHandler *m_com;
    QTimer *m_timer;

    int m_channels; // number of channels
    unsigned char m_data[512];

    bool enttecDmxOpen(const QString &port, int channels = 512);
    bool enttecDmxSendDmxFrame(unsigned char *data);
    bool getUsbVidPid(int *vid, int *pid);


    // pixel players hold all media
    QMap<QString, DigishowPixelPlayer*> m_players;
    bool initPlayer(const QVariantMap &mediaOptions);
    void stopAll();
    void setupPlayerPixelMapping(DigishowPixelPlayer *player, const QString &mediaName);

};

#endif // DGSDMXINTERFACE_H
