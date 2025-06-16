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

#include "dgs_aplay_interface.h"
#include "dgs_aplayer.h"
#include "digishow_environment.h"

#include <QAudioOutput>

DgsAPlayInterface::DgsAPlayInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "aplay";
    m_players.clear();
    m_volumes.clear();
    m_volumeMaster = 1.0;
}

DgsAPlayInterface::~DgsAPlayInterface()
{
    closeInterface();
}

int DgsAPlayInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    if (m_interfaceInfo.mode==INTERFACE_APLAY_DEFAULT) {

        m_players.clear();
        m_volumes.clear();
        m_volumeMaster = 1.0;

        // load media and initialize players
        QVariantList mediaList = cleanMediaList();
        for (int n=0 ; n<mediaList.length() ; n++) initPlayer(mediaList[n].toMap());

    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsAPlayInterface::closeInterface()
{
    if (m_interfaceInfo.mode==INTERFACE_APLAY_DEFAULT) {

        // release players
        QStringList playerNames = m_players.keys();
        int playerCount = playerNames.length();
        for (int n=0 ; n<playerCount ; n++) {
            QString key = playerNames[n];
            DgsAPlayer* player = m_players[key];
            player->stop();
            delete player;
            m_players.remove(key);
        }

        m_volumes.clear();
        m_volumeMaster = 1.0;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsAPlayInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (m_interfaceInfo.mode==INTERFACE_APLAY_DEFAULT) {

        if (m_endpointInfoList[endpointIndex].type == ENDPOINT_APLAY_MEDIA) {

            // play or stop media playback

            int control = m_endpointInfoList[endpointIndex].control;
            QVariantMap endpointOptions = m_endpointOptionsList[endpointIndex];
            QString media = endpointOptions.value("media").toString();

            if (control == CONTROL_MEDIA_START) {

                DgsAPlayer *player = m_players.value(media, nullptr);
                if (player == nullptr) return ERR_DEVICE_NOT_READY;

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {

                    double volume = endpointOptions.value("mediaVolume", QVariant(10000)).toInt() / 10000.0;
                    m_volumes[media] = volume;

                    player->setVolume(volume * m_volumeMaster);
                    player->setSpeed(endpointOptions.value("mediaSpeed", QVariant(10000)).toInt() / 10000.0);
                    player->setPosition(endpointOptions.value("mediaPosition").toInt());
                    player->setDuration(endpointOptions.value("mediaDuration").toInt());
                    player->setRepeat(endpointOptions.value("mediaRepeat").toBool());

                    if (endpointOptions.value("mediaAlone", QVariant(true)).toBool()) stopAll();
                    player->play();
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_STOP) {

                DgsAPlayer *player = m_players.value(media, nullptr);
                if (player == nullptr) return ERR_DEVICE_NOT_READY;

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {

                    player->stop();
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_STOP_ALL) {

                if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
                if (data.bValue) {

                    // stop all players
                    stopAll();
                }

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_VOLUME) {

                DgsAPlayer *player = m_players.value(media, nullptr);
                if (player == nullptr) return ERR_DEVICE_NOT_READY;

                if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

                double volume = (double)data.aValue / (double)data.aRange;
                m_volumes[media] = volume;
                player->setVolume(volume * m_volumeMaster);

                return ERR_NONE;

            } else if (control == CONTROL_MEDIA_MASTER) {

                if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

                m_volumeMaster = (double)data.aValue / (double)data.aRange;

                QStringList playerNames = m_players.keys();
                int playerCount = playerNames.length();
                for (int n=0 ; n<playerCount ; n++) {
                    QString key = playerNames[n];
                    DgsAPlayer* player = m_players[key];
                    player->setVolume(m_volumes[key] * m_volumeMaster);
                }

                return ERR_NONE;
            }
        }
    }

    return ERR_NONE;
}

int DgsAPlayInterface::loadMedia(const QVariantMap &mediaOptions)
{
    int r = DigishowInterface::loadMedia(mediaOptions);
    if ( r != ERR_NONE) return r;

    bool done = initPlayer(mediaOptions);
    if (!done) return ERR_INVALID_DATA;

    return ERR_NONE;
}

bool DgsAPlayInterface::initPlayer(const QVariantMap &mediaOptions)
{
    QString port = m_interfaceOptions.value("port").toString();

    QString name = mediaOptions.value("name").toString();
    QString type = mediaOptions.value("type").toString();
    QString url  = mediaOptions.value("url" ).toString();

    bool done = false;
    if (type == "audio" && !name.isEmpty() && !url.isEmpty()) {
        DgsAPlayer *player = new DgsAPlayer();
        if (player->load(url)) {
            player->setPort(port);
            m_players[name] = player;
            m_volumes[name] = 1.0;
            done = true;
        } else {
            delete player;
        }
    }

    return done;
}

void DgsAPlayInterface::stopAll()
{
    QStringList playerNames = m_players.keys();
    for (int n=0 ; n<playerNames.count() ; n++) {
        QString key = playerNames[n];
        DgsAPlayer *player = m_players.value(key, nullptr);
        if (player != nullptr) player->stop();
    }
}

void DgsAPlayInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_APLAY;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_APLAY_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = false;

    // Set interface label
    m_interfaceInfo.label = tr("Audio Player") + " " + m_interfaceOptions.value("port").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if (typeName == "media") endpointInfo.type = ENDPOINT_APLAY_MEDIA;

        // Set endpoint properties
        endpointInfo.output = true;
        endpointInfo.labelEPT = tr("Audio Clip");
        endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);

        switch (endpointInfo.control) {
            case CONTROL_MEDIA_START:
            case CONTROL_MEDIA_STOP:
            case CONTROL_MEDIA_STOP_ALL:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                break;
            case CONTROL_MEDIA_VOLUME:
            case CONTROL_MEDIA_MASTER:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 10000;
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}

