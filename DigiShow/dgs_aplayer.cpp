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

#include "dgs_aplayer.h"

DgsAPlayer::DgsAPlayer(QObject *parent) : QObject(parent)
{
    m_isLoaded = false;
    m_isPlaying = false;
    m_position = 0;
    m_duration = 0;
    m_repeat = false;

    m_player = new QMediaPlayer();
    m_player->setNotifyInterval(100);
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
}

DgsAPlayer::~DgsAPlayer()
{
    if (m_isPlaying) stop();

    delete m_timer;
    delete m_player;
}

bool DgsAPlayer::load(const QString &strUrl)
{
    if (m_isLoaded) return false;

    QUrl url(strUrl);
    if (!url.isValid()) return false;

    QMediaContent content(url);
    if (content.isNull()) return false;

    m_player->setMedia(content);
    m_isLoaded = true;

    return true;
}

bool DgsAPlayer::setPort(const QString &port)
{
    // TODO: set output device port for the audio player

    Q_UNUSED(port);
    return true;
}

void DgsAPlayer::setVolume(double volume)
{
    m_player->setVolume(volume*100);
}

void DgsAPlayer::setSpeed(double speed)
{
    m_player->setPlaybackRate(speed);
}

void DgsAPlayer::play()
{
    if (!m_isLoaded) return;

    if (m_isPlaying) stop();

    // automatically set the playback duration of specified media
    qint64 mediaDuration = m_player->duration();
    qreal  mediaPlayRate = m_player->playbackRate();
    if (m_duration == 0 && mediaDuration > m_position && mediaPlayRate > 0) {
        m_duration = (mediaDuration - m_position) / mediaPlayRate;
    }

    // start timer
    if (m_duration > 0) {
        m_timer->setInterval(m_duration);
        m_timer->setSingleShot(!m_repeat);
        m_timer->start();
    }

    // start player
    if (m_position > 0) m_player->setPosition(m_position);
    m_player->play();

    m_isPlaying = true;
    emit playingChanged(m_isPlaying);
}

void DgsAPlayer::stop()
{
    if (!m_isLoaded) return;

    m_player->stop();
    m_timer->stop();

    m_isPlaying = false;
    emit playingChanged(m_isPlaying);
}

void DgsAPlayer::onTimerFired()
{
    m_player->stop();

    if (m_repeat) {
        // repeat playing
        if (m_position > 0) m_player->setPosition(m_position);
        m_player->play();
    } else {
        // finish playing
        m_timer->stop();

        m_isPlaying = false;
        emit playingChanged(m_isPlaying);
    }
}

void DgsAPlayer::onPositionChanged(qint64 position)
{
    emit timeChanged(position);
}
