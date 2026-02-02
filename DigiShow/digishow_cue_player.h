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

#ifndef DIGISHOWCUEPLAYER_H
#define DIGISHOWCUEPLAYER_H

#include "digishow.h"

enum class PointType {
    LINEAR = 0,
    BEZIER = 1
};

class DigishowCuePlayer : public QObject
{
    Q_OBJECT
public:
    explicit DigishowCuePlayer(QObject *parent = nullptr);
    ~DigishowCuePlayer();

    // Set cue data and enable/disable tracks
    Q_INVOKABLE void setCueData(const QVariantMap& options, const QVariantList& details);
    Q_INVOKABLE void setTrackEnabled(int index, bool enabled);
    Q_INVOKABLE bool isTrackEnabled(int index) { return m_enabled[index]; }
    Q_INVOKABLE bool checkAllTracksDisabled();

    // Playback control
    Q_INVOKABLE bool play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE bool isPlaying(){ return m_isPlaying; }
    Q_INVOKABLE int  position() { return m_position; }
    Q_INVOKABLE int  duration() { return m_duration; }
    Q_INVOKABLE bool repeat()   { return m_repeat; }

signals:
    void cueFinished();

private slots:
    void onTimerFired();

private:
    bool m_isPlaying; // Current playing state
    int  m_position;  // Current position in milliseconds
    int  m_duration;  // Cue duration in milliseconds
    bool m_repeat;    // Repeat flag

    QVariantMap m_options;  // Cue options (such as duration, repeat flag)
    QVariantList m_details; // Cue details (playback curve control points of each track)
    QList<bool> m_enabled;  // Enabled flags of each track

    QTimer* m_timer;  // Timer for playback control
    QElapsedTimer m_elapsedTimer;  // Timer for tracking elapsed time

    static double getValueOfCurve(const QVariantList& points, double time);
};

#endif // DIGISHOWCUEPLAYER_H
