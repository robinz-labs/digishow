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

#include "dgs_metronome_interface.h"
#include "digishow_metronome.h"

DgsMetronomeInterface::DgsMetronomeInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "metronome";

    m_metronome = g_app->metronome();
}


DgsMetronomeInterface::~DgsMetronomeInterface()
{
    closeInterface();
}

int DgsMetronomeInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // refresh beat action map
    m_beatActionMap.clear();
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        if (endpointInfo.type == ENDPOINT_METRONOME_BEAT) {

            int beatIndexNoteOn  = endpointInfo.channel-1;
            int beatIndexNoteOff = beatIndexNoteOn + endpointInfo.control;

            BeatAction beatActionNoteOn;
            beatActionNoteOn.endpointIndex = n;
            beatActionNoteOn.noteOn = true;

            BeatAction beatActionNoteOff;
            beatActionNoteOff.endpointIndex = n;
            beatActionNoteOff.noteOn = false;

            m_beatActionMap[beatIndexNoteOn ].append(beatActionNoteOn);
            m_beatActionMap[beatIndexNoteOff].append(beatActionNoteOff);
        }
    }

    connect(m_metronome, SIGNAL(quarterChanged()), this, SLOT(onQuarterChanged()));

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsMetronomeInterface::closeInterface()
{
    disconnect(m_metronome, 0, this, 0);

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsMetronomeInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // TODO:

    return ERR_NONE;
}

void DgsMetronomeInterface::onQuarterChanged()
{
    int beatIndex = floor(m_metronome->phase() * 4);

    processBeatAction(beatIndex + m_metronome->quantum() * 4);
    processBeatAction(beatIndex);
}

void DgsMetronomeInterface::processBeatAction(int beatIndex)
{
    if (m_beatActionMap.contains(beatIndex)) {
        BeatActionList beatActionList = m_beatActionMap[beatIndex];
        foreach (const BeatAction & beatAction, beatActionList) {

            dgsSignalData data;
            data.signal = DATA_SIGNAL_NOTE;
            data.aValue = 127;
            data.aRange = 127;
            data.bValue = beatAction.noteOn;

            emit dataReceived(beatAction.endpointIndex, data);
        }
    }
}


