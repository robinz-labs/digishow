/*
    Copyright 2021-2025 Robin Zhang & Labs

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

#include "dgs_audioin_interface.h"
#include "audio_analyzer.h"
#include "digishow_environment.h"

DgsAudioinInterface::DgsAudioinInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "audioin";
    m_analyzer = nullptr;
}


DgsAudioinInterface::~DgsAudioinInterface()
{
    closeInterface();
}

int DgsAudioinInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;
    updateMetadata();

    // request privacy permission to access the microphone.
    if (!AppUtilities::canAccessMicrophone()) return ERR_DEVICE_NOT_READY;

    // confirm the specified audio input device
    QString audioinName = m_interfaceOptions.value("port").toString();
    QAudioDeviceInfo device;
    if (audioinName.isEmpty()) {
        device = QAudioDeviceInfo::defaultInputDevice();
    } else {
        int audioinIndex = -1;
        QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        for (int n=0 ; n < devices.length() ; n++)
            if (getUniqueAudioinName(n) == audioinName) { audioinIndex = n; break; }
        if (audioinIndex == -1) return ERR_DEVICE_NOT_READY;
        device = devices[audioinIndex];
    }

    // initialize audio analyzer
    bool enableUpdateLevel = false;
    bool enableUpdatePeak = false;
    bool enableUpdateSpectrum = false;
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        int type  = m_endpointInfoList[n].type;
        switch (type) {
        case ENDPOINT_AUDIOIN_LEVEL:
        case ENDPOINT_AUDIOIN_LEVEL_DB: enableUpdateLevel    = true; break;
        case ENDPOINT_AUDIOIN_PEAK:
        case ENDPOINT_AUDIOIN_PEAK_DB:  enableUpdatePeak     = true; break;
        case ENDPOINT_AUDIOIN_SPECTRUM: enableUpdateSpectrum = true; break;
        }
    }

    m_analyzer = new AudioAnalyzer(device);
    connect(m_analyzer, SIGNAL(spectrumDataReady(QVector<float>, float, float)),
            this, SLOT(onSpectrumDataReady(QVector<float>, float, float)));
    m_analyzer->setEnableUpdateLevel(enableUpdateLevel);
    m_analyzer->setEnableUpdatePeak(enableUpdatePeak);
    m_analyzer->setEnableUpdateSpectrum(enableUpdateSpectrum);
    m_analyzer->start();

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsAudioinInterface::closeInterface()
{
    if (m_analyzer) {
        m_analyzer->stop();
        delete m_analyzer;
        m_analyzer = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsAudioinInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // TODO:

    return ERR_NONE;
}

void DgsAudioinInterface::onSpectrumDataReady(const QVector<float> &spectrum, float level, float peak)
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        int type  = m_endpointInfoList[n].type;
        int range = m_endpointInfoList[n].range;

        if (type == ENDPOINT_AUDIOIN_LEVEL || type == ENDPOINT_AUDIOIN_LEVEL_DB) {

            int value;
            if (type == ENDPOINT_AUDIOIN_LEVEL_DB) {
                float db = 20.0f * log10(level + 1e-6f); // covert to decibel
                value = (db + 50) * 20000;               // spectrum: -50 to 0 dB
            } else {
                value = range * level;
            }

            dgsSignalData data;
            data.signal = DATA_SIGNAL_ANALOG;
            data.aRange = range;
            data.aValue = qBound<int>(0, value, range);
            emit dataReceived(n, data);

        } else if (type == ENDPOINT_AUDIOIN_PEAK || type == ENDPOINT_AUDIOIN_PEAK_DB) {

            int value;
            if (type == ENDPOINT_AUDIOIN_PEAK_DB) {
                float db = 20.0f * log10(peak + 1e-6f); // covert to decibel
                value = (db + 50) * 20000;              // spectrum: -50 to 0 dB
            } else {
                value = range * peak;
            }

            dgsSignalData data;
            data.signal = DATA_SIGNAL_ANALOG;
            data.aRange = range;
            data.aValue = qBound<int>(0, value, range);;
            emit dataReceived(n, data);

        } else if (type == ENDPOINT_AUDIOIN_SPECTRUM) {

            int channel = m_endpointInfoList[n].channel;
            float db = spectrum.at(channel-1);
            if (channel>=1 && channel<=spectrum.length()) {

                dgsSignalData data;
                data.signal = DATA_SIGNAL_ANALOG;
                data.aRange = range;
                data.aValue = qBound<int>(0, (db + 50) * 10000, range); // spectrum: -50 to 50 dB
                emit dataReceived(n, data);
            }
        }
    }
}

QVariantList DgsAudioinInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (int n=0 ; n < devices.length() ; n++) {
        info.clear();
        info["port"] = getUniqueAudioinName(n);

        QAudioFormat format = devices[n].preferredFormat();
        info["channelCount"] = format.channelCount();
        info["sampleRate"] = format.sampleRate();
        info["sampleSize"] = format.sampleSize();
        info["sampleType"] = format.sampleType();
        list.append(info);
    }

    return list;
}

QString DgsAudioinInterface::getUniqueAudioinName(int index)
{
    QString audioinName;
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    int i = 0;

    if (index >= 0 && index < devices.length()) {

        audioinName = devices[index].deviceName();

        for (int n=0 ; n<index ; n++)
            if (devices[n].deviceName() == audioinName) i++;
    }

    if (i>0) return audioinName + " #" + QString::number(i+1);
    return audioinName;
}

void DgsAudioinInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_AUDIOIN;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_AUDIOIN_DEFAULT;
    m_interfaceInfo.input = true;
    m_interfaceInfo.output = false;

    // Set interface label
    m_interfaceInfo.label = tr("Audio In") + " " + m_interfaceOptions.value("port").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "level"     ) endpointInfo.type = ENDPOINT_AUDIOIN_LEVEL;
        else if (typeName == "level_db"  ) endpointInfo.type = ENDPOINT_AUDIOIN_LEVEL_DB;
        else if (typeName == "peak"      ) endpointInfo.type = ENDPOINT_AUDIOIN_PEAK;
        else if (typeName == "peak_db"   ) endpointInfo.type = ENDPOINT_AUDIOIN_PEAK_DB;
        else if (typeName == "spectrum"  ) endpointInfo.type = ENDPOINT_AUDIOIN_SPECTRUM;

        // Set endpoint properties based on type
        endpointInfo.signal = DATA_SIGNAL_ANALOG;
        endpointInfo.input = true;
        endpointInfo.range = 1000000;
        endpointInfo.labelEPT = tr("Audio");

        switch (endpointInfo.type) {
            case ENDPOINT_AUDIOIN_LEVEL:
                endpointInfo.labelEPI = tr("Level");
                break;
            case ENDPOINT_AUDIOIN_LEVEL_DB:
                endpointInfo.labelEPI = tr("Level") + " dB";
                break;
            case ENDPOINT_AUDIOIN_PEAK:
                endpointInfo.labelEPI = tr("Peak");
                break;
            case ENDPOINT_AUDIOIN_PEAK_DB:
                endpointInfo.labelEPI = tr("Peak") + " dB";
                break;
            case ENDPOINT_AUDIOIN_SPECTRUM:
                endpointInfo.labelEPI = DigishowEnvironment::getSpectrumBandName(endpointInfo.channel)
                                      .split(" ").at(0) + " Hz";
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}
