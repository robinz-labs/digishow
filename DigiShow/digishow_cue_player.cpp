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

#include "digishow_cue_player.h"
#include "digishow_slot.h"

#define CUE_PLAYER_FREQ 40

DigishowCuePlayer::DigishowCuePlayer(QObject *parent)
    : QObject{parent}
    , m_isPlaying(false)
    , m_position(0)
    , m_duration(0)
    , m_repeat(false)
    , m_timer(nullptr)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1000/CUE_PLAYER_FREQ);
    connect(m_timer, &QTimer::timeout, this, &DigishowCuePlayer::onTimerFired);
}

DigishowCuePlayer::~DigishowCuePlayer()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

// Set the cue data
void DigishowCuePlayer::setCueData(const QVariantMap& options, const QVariantList& details)
{
    m_options = options;
    m_details = details;

    m_duration = options["cueDuration"].toInt();
    m_repeat = options["cueRepeat"].toBool();

    if (m_duration == 0) {
        // Find the curve with the longest time span
        double maxTime = 0.0;
        for (const QVariant& detail : details) {
            QVariantList points = detail.toMap()["points"].toList();
            if (!points.isEmpty()) {
                double lastTime = points.last().toMap()["time"].toDouble();
                maxTime = qMax(maxTime, lastTime);
            }
        }
        // Convert seconds to milliseconds
        m_duration = static_cast<int>(maxTime * 1000);
    }
}

// Start playing the cue
// Returns true if successfully started or already playing
bool DigishowCuePlayer::play()
{
    if (!m_isPlaying) {
        m_isPlaying = true;
        m_elapsedTimer.start();
        m_timer->start();
    }
    return true;
}

// Stop playing the cue
void DigishowCuePlayer::stop()
{
    if (m_isPlaying) {
        m_isPlaying = false;
        m_timer->stop();
        m_elapsedTimer.invalidate();
    }
}

// Check if the cue is currently playing
bool DigishowCuePlayer::isPlaying() const
{
    return m_isPlaying;
}

void DigishowCuePlayer::onTimerFired()
{
    if (!m_isPlaying) return;

    // Calculate elapsed time in milliseconds
    qint64 elapsedTime = m_elapsedTimer.elapsed();
    m_position = elapsedTime;

    // Check if elapsed time exceeds the cue duration
    if (elapsedTime >= m_duration) {
        if (m_repeat) {
            // Reset elapsed time for repeating
            m_elapsedTimer.restart();
        } else {
            // Stop playback if not repeating
            stop();
            emit cueFinished();
        }
    }

    // Process each curve in m_details
    for (int n=0 ; n<m_details.size() ; n++) {

        // variable n is the index of the slot for value output
        
        QVariantMap detailMap = m_details[n].toMap();
        if (detailMap.isEmpty()) continue;
        
        QVariantList points = detailMap["points"].toList();
        if (points.isEmpty()) continue;
        
        // Get current value for this curve
        double currentValue = getValueOfCurve(points, elapsedTime / 1000.0);
        
        //qDebug() << "Cue playing at Slot:" << n << "Time:" << elapsedTime << "Value:" << currentValue;

        // Output the current value to the slot
        DigishowSlot *slot = g_app->slotAt(n);
        if (slot) {
            int range = slot->getEndpointOutRange();
            if (range == 0) range = 1;
            slot->setEndpointOutValue(round(currentValue/100*range));
        }
    }
}

double DigishowCuePlayer::getValueOfCurve(const QVariantList& points, double time)
{
    // Find the line segment the time point is located
    for (int i = 0; i < points.size() - 1; i++) {
        QVariantMap p1 = points[i].toMap();
        QVariantMap p2 = points[i + 1].toMap();
        
        double p1Time = p1["time"].toDouble();
        double p2Time = p2["time"].toDouble();
        
        if (time >= p1Time && time <= p2Time) {
            double t = (time - p1Time) / (p2Time - p1Time);
            
            double p1Value = p1["value"].toDouble();
            double p2Value = p2["value"].toDouble();
            
            if (p1["type"].toInt() == static_cast<int>(PointType::BEZIER) || 
                p2["type"].toInt() == static_cast<int>(PointType::BEZIER)) {
                // Bezier curve interpolation
                double cp1y = p1Value;
                double cp2y = p2Value;
                return p1Value * pow(1-t, 3) + 
                       cp1y * 3 * t * pow(1-t, 2) + 
                       cp2y * 3 * pow(t, 2) * (1-t) + 
                       p2Value * pow(t, 3);
            } else {
                // Linear interpolation
                return p1Value + (p2Value - p1Value) * t;
            }
        }
    }
    
    // If time exceeds the last point, return the value of the last point
    if (!points.isEmpty()) {
        return points.last().toMap()["value"].toDouble();
    }
    
    return 0.0; // Return default value if points list is empty
}
