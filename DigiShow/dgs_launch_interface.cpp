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

#include "dgs_launch_interface.h"
#include "digishow_cue_manager.h"
#include "digishow_environment.h"

DgsLaunchInterface::DgsLaunchInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "launch";
}

DgsLaunchInterface::~DgsLaunchInterface()
{
    closeInterface();
}

int DgsLaunchInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    connect(g_app->cueManager(), &DigishowCueManager::statusUpdated, this, &DgsLaunchInterface::onStatusUpdated);

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsLaunchInterface::closeInterface()
{
    disconnect(g_app->cueManager(), nullptr, this, nullptr);

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsLaunchInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
    if (data.bValue) {
        int control = m_endpointInfoList[endpointIndex].control;
        int channel = m_endpointInfoList[endpointIndex].channel;

        if (control == CONTROL_MEDIA_START) {
            g_app->startLaunch("launch" + QString::number(channel), false);
        } else if (control == CONTROL_MEDIA_RESTART) {
            g_app->startLaunch("launch" + QString::number(channel), true);
        } else if  (control == CONTROL_MEDIA_STOP) {
            g_app->cueManager()->stopCue("launch" + QString::number(channel));
        } else if  (control == CONTROL_MEDIA_STOP_ALL) {
            g_app->cueManager()->stopAllCues();
        }
    }

    return ERR_NONE;
}

void DgsLaunchInterface::onStatusUpdated()
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        if (endpointInfo.type == ENDPOINT_LAUNCH_PLAYING) {

            int channel = endpointInfo.channel;
            bool playing = g_app->cueManager()->isCuePlaying("launch" + QString::number(channel));

            // emit playing change event
            dgsSignalData data;
            data.signal = DATA_SIGNAL_BINARY;
            data.bValue = playing;
            emit dataReceived(n, data);
        }
    }
}

void DgsLaunchInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_LAUNCH;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_LAUNCH_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = true;
    m_interfaceInfo.inputSecondary = true;

    // Set interface label
    m_interfaceInfo.label = tr("Preset Launcher");

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "preset") endpointInfo.type = ENDPOINT_LAUNCH_PRESET;
        else if (typeName == "playing") endpointInfo.type = ENDPOINT_LAUNCH_PLAYING;
        else if (typeName == "time") endpointInfo.type = ENDPOINT_LAUNCH_TIME;

        // Set endpoint type based on type
        if (endpointInfo.type == ENDPOINT_LAUNCH_PRESET) {
            
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
        
            switch (endpointInfo.control) {
            case CONTROL_MEDIA_START:
            case CONTROL_MEDIA_RESTART:
            case CONTROL_MEDIA_STOP:
                endpointInfo.labelEPT = tr("Preset") + " " + QString::number(endpointInfo.channel);
                endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control) ;
                break;
            case CONTROL_MEDIA_STOP_ALL:
                endpointInfo.labelEPT = tr("Preset");
                endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);
                break;
            }
            
        } else if (endpointInfo.type == ENDPOINT_LAUNCH_PLAYING) {
            
            endpointInfo.input = true;
            endpointInfo.labelEPT = tr("Preset") + " " + QString::number(endpointInfo.channel);
            endpointInfo.labelEPI = tr("Playing");
            endpointInfo.signal = DATA_SIGNAL_BINARY;

        } else if (endpointInfo.type == ENDPOINT_LAUNCH_TIME) {
            
            endpointInfo.input = true;
            endpointInfo.labelEPT = tr("Preset") + " " + QString::number(endpointInfo.channel);
            endpointInfo.labelEPI = tr("Time");
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000000000); // millisecond
        }

        m_endpointInfoList.append(endpointInfo);
    }
}
