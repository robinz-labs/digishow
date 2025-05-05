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

#ifndef DIGISHOWCUEMANAGER_H
#define DIGISHOWCUEMANAGER_H

#include "digishow.h"

class DigishowCuePlayer;

class DigishowCueManager : public QObject
{
    Q_OBJECT
public:
    explicit DigishowCueManager(QObject *parent = nullptr);
    ~DigishowCueManager();

    Q_INVOKABLE DigishowCuePlayer* cuePlayer(const QString& name);
    Q_INVOKABLE bool playCue(const QString& name);
    Q_INVOKABLE void stopCue(const QString& name);
    Q_INVOKABLE void stopAllCues();
    Q_INVOKABLE bool isCuePlaying(const QString& name);
    Q_INVOKABLE bool isActivated() { return m_cuePlayers.size() > 0; }

signals:
    void statusUpdated();

public slots:
    void onCueFinished();

private:
    QMap<QString, DigishowCuePlayer*> m_cuePlayers;
    
};

#endif // DIGISHOWCUEMANAGER_H
