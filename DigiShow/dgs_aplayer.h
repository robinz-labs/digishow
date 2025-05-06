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

#ifndef DGSAPLAYER_H
#define DGSAPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QTimer>

class DgsAPlayer : public QObject
{
    Q_OBJECT
public:
    explicit DgsAPlayer(QObject *parent = nullptr);
    ~DgsAPlayer();

    bool load(const QString &url);
    bool setPort(const QString &port);
    void setPosition(int msec) { m_position = msec; }
    void setDuration(int msec) { m_duration = msec; }
    void setRepeat(bool repeat) { m_repeat = repeat; }
    void setVolume(double volume); // 0 ~ 1.0
    void setSpeed(double speed);   // 0 ~ 4.0
    void play();
    void stop();

    bool isLoaded() { return m_isLoaded; }
    bool isPlaying() { return m_isPlaying; }

private slots:
    void onTimerFired();

private:
    QMediaPlayer *m_player;
    QTimer *m_timer;
    bool m_isLoaded;
    bool m_isPlaying;
    int m_position; // millisecond
    int m_duration; // millisecond
    bool m_repeat;
};

#endif // DGSAPLAYER_H
