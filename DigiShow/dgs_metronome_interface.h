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

#ifndef DGSMETRONOMEINTERFACE_H
#define DGSMETRONOMEINTERFACE_H

#include "digishow_interface.h"

typedef struct BeatAction {

    int endpointIndex;
    bool noteOn;

} BeatAction;
typedef QList<BeatAction> BeatActionList;
typedef QMap<int, BeatActionList> BeatActionMap;

class DigishowMetronome;

class DgsMetronomeInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsMetronomeInterface(QObject *parent = nullptr);
    ~DgsMetronomeInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

signals:

public slots:

    void onQuarterChanged();

private:

    DigishowMetronome *m_metronome;
    BeatActionMap m_beatActionMap;

    void processBeatAction(int beatIndex);
};

#endif // DGSMETRONOMEINTERFACE_H
