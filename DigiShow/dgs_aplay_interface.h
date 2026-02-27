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

#ifndef DGSAPLAYINTERFACE_H
#define DGSAPLAYINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class DgsAPlayer;

class DgsAPlayInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsAPlayInterface(QObject *parent = nullptr);
    ~DgsAPlayInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    int loadMedia(const QVariantMap &mediaOptions) override;

signals:

public slots:
    void onPlayingChanged(bool playing);
    void onTimeChanged(int msec);

private:

    // players hold all media
    QMap<QString, DgsAPlayer*> m_players;
    QMap<QString, double> m_volumes; // 0 ~ 1.0
    double m_volumeMaster; // 0 ~ 1.0

    bool initPlayer(const QVariantMap &mediaOptions);
    void setPlayerCallback(const QString &mediaName);
    void stopAll();

    void updateMetadata_() override;

};

#endif // DGSAPLAYINTERFACE_H
