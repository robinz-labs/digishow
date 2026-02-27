/*
    Copyright 2026 Robin Zhang & Labs

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

#include "dgs_lfo_interface.h"
#include "digishow_lfo.h"
#include "digishow_environment.h"

DgsLfoInterface::DgsLfoInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "lfo";
    m_lfos.clear();
}

DgsLfoInterface::~DgsLfoInterface()
{
    closeInterface();
}

int DgsLfoInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // initialize LFOs
    m_lfos.clear();
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        QVariantMap endpointOptions = m_endpointOptionsList[n];

        if (endpointInfo.type == ENDPOINT_LFO_VALUE) {

            // obtain LFO parameters
            DigishowLFO::LfoFunction function = getFunctionType(endpointOptions.value("optFunction", "sine").toString()); 
            int    sampleInterval = endpointOptions.value("optSample", 20).toInt();        // milliseconds
            int    period         = endpointOptions.value("optPeriod", 1000).toInt();      // milliseconds
            double phase          = endpointOptions.value("optPhase", 0.0).toDouble();     // 0 ~ 1.0
            double amplitude      = endpointOptions.value("optAmplitude", 1.0).toDouble(); // 0 ~ 1.0
            bool   run            = endpointOptions.value("optRun", true).toBool();
            
            // create LFO object
            DigishowLFO* lfo = new DigishowLFO(this);

            lfo->setFunction(function);
            lfo->setSampleInterval(sampleInterval);
            lfo->setPeriod(period);
            lfo->setPhase(phase);
            lfo->setAmplitude(amplitude);

            connect(lfo, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));

            if (run) lfo->start();

            m_lfos[endpointInfo.channel] = lfo;
        }
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsLfoInterface::closeInterface()
{
    // release LFOs
    QList<int> keys = m_lfos.keys();
    int lfoCount = keys.length();
    for (int n=0 ; n<lfoCount ; n++) {
        int key = keys[n];
        DigishowLFO* lfo = m_lfos[key];
        lfo->stop();
        delete lfo;
        m_lfos.remove(key);
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsLfoInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    int type    = m_endpointInfoList[endpointIndex].type;
    int channel = m_endpointInfoList[endpointIndex].channel;

    if ((type == ENDPOINT_LFO_PERIOD    && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_LFO_PHASE     && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_LFO_AMPLITUDE && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_LFO_SAMPLE    && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_LFO_RESET     && data.signal != DATA_SIGNAL_BINARY) ||
        (type == ENDPOINT_LFO_RUN       && data.signal != DATA_SIGNAL_BINARY)) return ERR_INVALID_DATA;

    if (!m_lfos.contains(channel)) return ERR_INVALID_INDEX;

    DigishowLFO *lfo = m_lfos[channel];

    switch (type) {
    case ENDPOINT_LFO_PERIOD:
        lfo->setPeriod(data.aValue);
        break;
    case ENDPOINT_LFO_PHASE:
        lfo->setPhase((double)data.aValue/(double)data.aRange);
        break;
    case ENDPOINT_LFO_AMPLITUDE:
        lfo->setAmplitude((double)data.aValue/(double)data.aRange);
        break;
    case ENDPOINT_LFO_SAMPLE:
        lfo->setSampleInterval(data.aValue);
        break;
    case ENDPOINT_LFO_RUN:
        if (data.bValue) lfo->start(); else lfo->stop();
        break;
    case ENDPOINT_LFO_RESET:
        if (data.bValue && lfo->isActive()) { lfo->stop(); lfo->start(); }
        break;
    }

    return ERR_NONE;
}

void DgsLfoInterface::onValueChanged(double value)
{
    // find matched LFO object
    DigishowLFO *lfo = qobject_cast<DigishowLFO*>(sender());
    if (!lfo) return;

    // find matched channel
    int channel = m_lfos.key(lfo, -1);
    if (channel<0) return;

    // find matched endpoint
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        if (endpointInfo.type == ENDPOINT_LFO_VALUE && endpointInfo.channel == channel) {

            // emit signal received event
            dgsSignalData data;
            data.signal = DATA_SIGNAL_ANALOG;
            data.aRange = endpointInfo.range;
            data.aValue = (int)(value * (double)data.aRange);
            emit dataReceived(n, data);

        } else if (endpointInfo.type == ENDPOINT_LFO_TIME && endpointInfo.channel == channel) {
            
            // emit time elapsed event
            dgsSignalData data;
            data.signal = DATA_SIGNAL_ANALOG;
            data.aRange = endpointInfo.range;
            data.aValue = qMin<int>(lfo->timeElapsed(), endpointInfo.range);
            emit dataReceived(n, data);
        }
    }
}

void DgsLfoInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_LFO;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_LFO_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = true;
    m_interfaceInfo.outputSecondary = true;

    // Set interface label
    m_interfaceInfo.label = tr("LFO");

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "value"    ) endpointInfo.type = ENDPOINT_LFO_VALUE;
        else if (typeName == "time"     ) endpointInfo.type = ENDPOINT_LFO_TIME;
        else if (typeName == "reset"    ) endpointInfo.type = ENDPOINT_LFO_RESET;
        else if (typeName == "run"      ) endpointInfo.type = ENDPOINT_LFO_RUN;
        else if (typeName == "period"   ) endpointInfo.type = ENDPOINT_LFO_PERIOD;
        else if (typeName == "phase"    ) endpointInfo.type = ENDPOINT_LFO_PHASE;
        else if (typeName == "amplitude") endpointInfo.type = ENDPOINT_LFO_AMPLITUDE;
        else if (typeName == "sample"   ) endpointInfo.type = ENDPOINT_LFO_SAMPLE;

        endpointInfo.labelEPT = tr("LFO %1").arg(endpointInfo.channel);

        // Set endpoint properties based on type
        switch (endpointInfo.type) {
            case ENDPOINT_LFO_VALUE:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.input = true;
                endpointInfo.range = (endpointInfo.range ? endpointInfo.range : 65535);
                endpointInfo.labelEPI = getFunctionName(m_endpointOptionsList[n].value("optFunction").toString());
                break;
            case ENDPOINT_LFO_TIME:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.input = true;
                endpointInfo.range = (endpointInfo.range ? endpointInfo.range : 1000000000); // millisecond
                endpointInfo.labelEPI = tr("Time");
                break;
            case ENDPOINT_LFO_RESET:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPI = tr("Reset");
                break;
            case ENDPOINT_LFO_RUN:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPI = tr("Run");
                break;
            case ENDPOINT_LFO_PERIOD:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 60000; // millisecond
                endpointInfo.labelEPI = tr("Period");
                break;
            case ENDPOINT_LFO_PHASE:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 1000000;
                endpointInfo.labelEPI = tr("Phase");
                break;
            case ENDPOINT_LFO_AMPLITUDE:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 1000000;
                endpointInfo.labelEPI = tr("Amplitude");
                break;
            case ENDPOINT_LFO_SAMPLE:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range = 60000; // millisecond
                endpointInfo.labelEPI = tr("Sample");
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}

QString DgsLfoInterface::getFunctionName(const QString & func)
{
    QString funcName;
    if      (func == "sine"    ) funcName = tr("Sine");
    else if (func == "square"  ) funcName = tr("Square");
    else if (func == "sawtooth") funcName = tr("Sawtooth");
    else if (func == "triangle") funcName = tr("Triangle");
    else if (func == "random"  ) funcName = tr("Random");
    else                         funcName = tr("None");
    return funcName;
}

DigishowLFO::LfoFunction DgsLfoInterface::getFunctionType(const QString & func)
{
    DigishowLFO::LfoFunction funcType = DigishowLFO::LfoNone;
    if      (func == "sine"    ) funcType = DigishowLFO::LfoSine;
    else if (func == "square"  ) funcType = DigishowLFO::LfoSquare;
    else if (func == "sawtooth") funcType = DigishowLFO::LfoSawtooth;
    else if (func == "triangle") funcType = DigishowLFO::LfoTriangle;
    else if (func == "random"  ) funcType = DigishowLFO::LfoRandom;
    else                         funcType = DigishowLFO::LfoNone;
    return funcType;
}
