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

    connect(m_metronome, SIGNAL(beatChanged()), this, SLOT(onBeatChanged()));
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

    int type = m_endpointInfoList[endpointIndex].type;

    if ((type == ENDPOINT_METRONOME_BPM     && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_METRONOME_QUANTUM && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_METRONOME_RUN     && data.signal != DATA_SIGNAL_BINARY) ||
        (type == ENDPOINT_METRONOME_LINK    && data.signal != DATA_SIGNAL_BINARY) ||
        (type == ENDPOINT_METRONOME_TAP     && data.signal != DATA_SIGNAL_BINARY) ||
        (type == ENDPOINT_METRONOME_RESET   && data.signal != DATA_SIGNAL_BINARY)) return ERR_INVALID_DATA;

    switch (type) {
    case ENDPOINT_METRONOME_BPM:
        g_app->metronome()->setBpm(qBound(20, data.aValue, 600));
        break;
    case ENDPOINT_METRONOME_QUANTUM:
        g_app->metronome()->setQuantum(qBound(1, data.aValue, 12));
        break;
    case ENDPOINT_METRONOME_RUN:
        g_app->metronome()->setRunning(data.bValue);
        break;
    case ENDPOINT_METRONOME_LINK:
        g_app->metronome()->setLinkEnabled(data.bValue);
        break;
    case ENDPOINT_METRONOME_TAP:
        if (data.bValue) g_app->metronome()->tap();
        break;
    case ENDPOINT_METRONOME_RESET:
        if (data.bValue) g_app->metronome()->resetBeat();
        break;
    }

    return ERR_NONE;
}

void DgsMetronomeInterface::onBeatChanged()
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        int type  = m_endpointInfoList[n].type;
        if (type == ENDPOINT_METRONOME_COUNT) {

            int value = m_metronome->beat();
            int range = m_endpointInfoList[n].range;

            dgsSignalData data;
            data.signal = DATA_SIGNAL_ANALOG;
            data.aRange = range;
            data.aValue = qMin<int>(value, range);
            emit dataReceived(n, data);
        }
    }
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

void DgsMetronomeInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_METRONOME;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_METRONOME_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = true;

    // Set interface label
    m_interfaceInfo.label = tr("Beat Maker");

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "beat"   ) endpointInfo.type = ENDPOINT_METRONOME_BEAT;
        else if (typeName == "count"  ) endpointInfo.type = ENDPOINT_METRONOME_COUNT;
        else if (typeName == "reset"  ) endpointInfo.type = ENDPOINT_METRONOME_RESET;
        else if (typeName == "run"    ) endpointInfo.type = ENDPOINT_METRONOME_RUN;
        else if (typeName == "bpm"    ) endpointInfo.type = ENDPOINT_METRONOME_BPM;
        else if (typeName == "quantum") endpointInfo.type = ENDPOINT_METRONOME_QUANTUM;
        else if (typeName == "link"   ) endpointInfo.type = ENDPOINT_METRONOME_LINK;
        else if (typeName == "tap"    ) endpointInfo.type = ENDPOINT_METRONOME_TAP;

        // Set endpoint properties based on type
        switch (endpointInfo.type) {
            case ENDPOINT_METRONOME_BEAT:
                endpointInfo.signal = DATA_SIGNAL_NOTE;
                endpointInfo.input = true;
                endpointInfo.range = 127;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Beat %1").arg((endpointInfo.channel-1) / 4 + 1);
                if ((endpointInfo.channel-1) % 4 != 0)
                    endpointInfo.labelEPI += QString(".%1").arg((endpointInfo.channel-1) % 4 + 1);
                break;
            case ENDPOINT_METRONOME_COUNT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.input = true;
                endpointInfo.range = (endpointInfo.range ? endpointInfo.range : 1000000);
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Count");
                break;
            case ENDPOINT_METRONOME_RESET:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Reset");
                break;
            case ENDPOINT_METRONOME_RUN:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Run");
                break;
            case ENDPOINT_METRONOME_BPM:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 600;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("BPM");
                break;
            case ENDPOINT_METRONOME_QUANTUM:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 12;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Quantum");
                break;
            case ENDPOINT_METRONOME_LINK:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Link");
                break;
            case ENDPOINT_METRONOME_TAP:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Beat Maker");
                endpointInfo.labelEPI = tr("Tap");
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}


