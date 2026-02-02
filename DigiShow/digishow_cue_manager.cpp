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

#include "digishow_cue_manager.h"
#include "digishow_cue_player.h"

DigishowCueManager::DigishowCueManager(QObject *parent)
    : QObject{parent}
{

}

DigishowCueManager::~DigishowCueManager()
{
    // Clean up all cue players
    stopAllCues();
}

DigishowCuePlayer* DigishowCueManager::cuePlayer(const QString& name)
{
    // Find the cue player
    DigishowCuePlayer* player = m_cuePlayers.value(name);
    if (player) {
        return player;
    }
    return nullptr;
}

// Play a cue with the specified name
bool DigishowCueManager::playCue(const QString& name)
{
    // Get the cue data from the app
    QVariantMap options = g_app->getCuePlayerOptions(name);
    QVariantList details = g_app->getSlotCuePlayerDetails(name);

    if (options.value("cueAlone").toBool()) {
        // Stop all cues if the Cue Alone option is enabled
        stopAllCues();
    } else {
        // Disable all mutual exclusive tracks of active cues
        for (int index = 0; index < details.size(); index++) {
            if (!details[index].toMap().isEmpty()) {
                disableTrack(index);
            }
        }
        // Stop cues where tracks have been completely disabled
        stopDisabledCues();
    }
    
    // Get or create a cue player for this name
    DigishowCuePlayer* player = m_cuePlayers.value(name);
    if (!player) {
        player = new DigishowCuePlayer(this);
        connect(player, &DigishowCuePlayer::cueFinished, this, &DigishowCueManager::onCueFinished);
        m_cuePlayers.insert(name, player);
    }

    // Stop the cue if it's already playing
    if (player->isPlaying()) {
        player->stop();
    }
    
    // Set the cue data to the player
    player->setCueData(options, details);

    // Start playing the cue
    bool done = player->play();

    emit statusUpdated();
    return done;
}

// Stop a specific cue by its name
void DigishowCueManager::stopCue(const QString& name)
{
    // Find and stop the specified cue player
    DigishowCuePlayer* player = m_cuePlayers.value(name);
    if (player) {
        player->stop();

        // Remove the player from the map
        m_cuePlayers.remove(name);
        player->deleteLater();

        emit statusUpdated();
    }
}

// Stop all currently playing cues
void DigishowCueManager::stopAllCues()
{
    // Iterate through all active cue players, stop and clear them
    for (DigishowCuePlayer* player : qAsConst(m_cuePlayers)) {
        if (player) {
            player->stop();
            player->deleteLater();
        }
    }    
    m_cuePlayers.clear();

    emit statusUpdated();
}

// Check if a specific cue is currently playing
bool DigishowCueManager::isCuePlaying(const QString& name)
{
    // Find the cue player and check its status
    DigishowCuePlayer* player = m_cuePlayers.value(name);
    if (player) {
        return player->isPlaying();
    }
    return false;
}

// Disable a specific track index for all active cues
void DigishowCueManager::disableTrack(int index)
{
    for (DigishowCuePlayer* player : qAsConst(m_cuePlayers)) {
        if (player) {
            player->setTrackEnabled(index, false);
        }
    }
}

// Stop cues where tracks are completely disabled
void DigishowCueManager::stopDisabledCues()
{
    QStringList names = m_cuePlayers.keys();
    for (QString name : names) {
        DigishowCuePlayer* player = m_cuePlayers.value(name);
        if (player && player->checkAllTracksDisabled()) {
            player->stop();
            player->deleteLater();
            m_cuePlayers.remove(name);
            emit statusUpdated();
        }
    }
}

// Handle cue finished signal
void DigishowCueManager::onCueFinished()
{
    // Find the sender cue player
    DigishowCuePlayer* player = qobject_cast<DigishowCuePlayer*>(sender());
    if (!player) return;

    // Find and remove the finished cue player from the map
    QString name = m_cuePlayers.key(player);
    if (!name.isEmpty()) {
        m_cuePlayers.remove(name);
        player->deleteLater();

        emit statusUpdated();
    }
}
