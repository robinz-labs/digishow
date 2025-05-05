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

#include "digishow_slot.h"
#include "digishow_interface.h"
#include "digishow_scriptable.h"
#include <math.h>
#include <QJsonDocument>

#define MINMAX(v,a,b) ( (v) < (a) ? (a) : ((v) > (b) ? (b) : (v)) )
#define MAP(v,a,b,A,B) ( ((B)-(A)) * ((v)-(a)) / ((b)-(a)) + (A) )

DigishowSlot::DigishowSlot(QObject *parent) : QObject(parent)
{
    m_slotIndex = -1;

    // no source and no destination is assigned
    m_sourceInterface          = nullptr;
    m_destinationInterface     = nullptr;
    m_sourceEndpointIndex      = -1;
    m_destinationEndpointIndex = -1;

    // get ready to run slot
    m_enabled = false;
    m_linked = true;
    m_selected = false;
    m_outputInterval = 0;
    m_dataInProcessingStackLevel = 0;
    m_dataInTimeLastReceived = 0;
    m_dataOutTimeLastSent = 0;
    m_needSendDataOutLater =false;
    m_dataOutTimer.setSingleShot(true);
    connect(&m_dataOutTimer, SIGNAL(timeout()), this, SLOT(onDataOutTimerFired()));

    m_elapsedTimer.start();

    // get ready to run envelope
    m_envelopeTimeOn = 0;
    m_envelopeTimeOff = 0;
    m_envelopeTimeLastUpdated = 0;
    m_envelopeTimer.setInterval(5);
    m_envelopeTimer.setSingleShot(false);
    connect(&m_envelopeTimer, SIGNAL(timeout()), this, SLOT(onEnvelopeTimerFired()));

    // get ready to run smoothing
    m_smoothingTimeStart = 0;
    m_smoothingTimer.setInterval(20);
    m_smoothingTimer.setSingleShot(false);
    connect(&m_smoothingTimer, SIGNAL(timeout()), this, SLOT(onSmoothingTimerFired()));

    // init input and output data
    m_lastDataInPre  = dgsSignalData();
    m_lastDataIn     = dgsSignalData();
    m_lastDataOutPre = dgsSignalData();
    m_lastDataOut    = dgsSignalData();

    // init error flags
    m_trafficError = false;
    m_inputExpressionError = false;
    m_outputExpressionError = false;

    connect(this, SIGNAL(metadataUpdated()), g_app, SLOT(onSlotMetadataUpdated()));

#ifdef QT_DEBUG
    qDebug() << "slot created";
#endif

}

DigishowSlot::~DigishowSlot()
{
#ifdef QT_DEBUG
    qDebug() << "slot released";
#endif
}

int DigishowSlot::setSource(DigishowInterface *interface, int endpointIndex)
{
    // release old source connection
    if (m_sourceInterface != nullptr) {
        disconnect(m_sourceInterface, SIGNAL(metadataUpdated()), this, nullptr);
        disconnect(m_sourceInterface, SIGNAL(dataReceived(int, dgsSignalData)), this, nullptr);
        m_sourceInterface = nullptr;
        m_sourceEndpointIndex = -1;
        m_slotInfo.inputSignal = 0;
        m_slotInfo.inputRange = 0;
    }

    // create new source connection
    if (interface != nullptr) {
        m_sourceInterface = interface;
        m_sourceEndpointIndex = endpointIndex;
        m_slotInfo.inputSignal = interface->endpointInfoList()->at(endpointIndex).signal;
        m_slotInfo.inputRange = interface->endpointInfoList()->at(endpointIndex).range;
        m_lastDataInPre = dgsSignalData();
        m_lastDataIn    = dgsSignalData();

        connect(m_sourceInterface, SIGNAL(metadataUpdated()), this, SLOT(onInterfaceMetadataUpdated()));
        connect(m_sourceInterface, SIGNAL(dataReceived(int, dgsSignalData)), this, SLOT(onDataReceived(int, dgsSignalData)));
    }

    return ERR_NONE;
}

int DigishowSlot::setDestination(DigishowInterface *interface, int endpointIndex)
{
    // release old destination connection
    if (m_destinationInterface != nullptr) {
        disconnect(m_destinationInterface, SIGNAL(metadataUpdated()), this, nullptr);
        disconnect(m_destinationInterface, SIGNAL(dataPrepared(int, dgsSignalData, bool)), this, nullptr);
        disconnect(m_destinationInterface, SIGNAL(dataSent(int, dgsSignalData)), this, nullptr);
        m_destinationInterface = nullptr;
        m_destinationEndpointIndex = -1;
        m_slotInfo.outputSignal = 0;
        m_slotInfo.outputRange = 0;
    }

    // create new destination connection
    if (interface != nullptr) {
        m_destinationInterface = interface;
        m_destinationEndpointIndex = endpointIndex;
        m_slotInfo.outputSignal = interface->endpointInfoList()->at(endpointIndex).signal;
        m_slotInfo.outputRange = interface->endpointInfoList()->at(endpointIndex).range;
        m_lastDataOutPre = dgsSignalData();
        m_lastDataOut    = dgsSignalData();

        // set output interval option
        updateSlotOuputInterval();

        connect(m_destinationInterface, SIGNAL(metadataUpdated()), this, SLOT(onInterfaceMetadataUpdated()));
        connect(m_destinationInterface, SIGNAL(dataPrepared(int, dgsSignalData, bool)), this, SLOT(onDataPrepared(int, dgsSignalData, bool)));
        connect(m_destinationInterface, SIGNAL(dataSent(int, dgsSignalData)), this, SLOT(onDataSent(int, dgsSignalData)));
    }

    return ERR_NONE;
}

int DigishowSlot::setSlotOptions(const QVariantMap &options)
{
    m_slotOptions = options;
    QStringList keys = options.keys();
    for (int n=0 ; n<keys.length() ; n++) {
        updateSlotInfoItem(keys[n], options[keys[n]]);
    }

    return ERR_NONE;
}

int DigishowSlot::setSlotOption(const QString &name, const QVariant &value, bool isMap)
{
    m_slotOptions[name] = isMap ? value.toMap() : value;
    updateSlotInfoItem(name, value);

    return ERR_NONE;
}

int DigishowSlot::clearSlotOption(const QString &name)
{
    m_slotOptions.remove(name);
    updateSlotInfoItem(name, QVariant());

    return ERR_NONE;
}


QVariantMap DigishowSlot::getSlotInfo()
{
    QVariantMap info;

    info["inputExpression" ]= m_slotInputExpression;
    info["outputExpression"]= m_slotOutputExpression;

    info["inputSignal"    ] = m_slotInfo.inputSignal;
    info["outputSignal"   ] = m_slotInfo.outputSignal;
    info["inputRange"     ] = m_slotInfo.inputRange;
    info["outputRange"    ] = m_slotInfo.outputRange;

    info["inputLow"       ] = m_slotInfo.inputLow;
    info["inputHigh"      ] = m_slotInfo.inputHigh;
    info["outputLow"      ] = m_slotInfo.outputLow;
    info["outputHigh"     ] = m_slotInfo.outputHigh;
    info["inputInverted"  ] = m_slotInfo.inputInverted;
    info["outputInverted" ] = m_slotInfo.outputInverted;
    info["outputLowAsZero"] = m_slotInfo.outputLowAsZero;

    info["envelopeAttack" ] = m_slotInfo.envelopeAttack;
    info["envelopeHold"   ] = m_slotInfo.envelopeHold;
    info["envelopeDecay"  ] = m_slotInfo.envelopeDecay;
    info["envelopeSustain"] = m_slotInfo.envelopeSustain;
    info["envelopeRelease"] = m_slotInfo.envelopeRelease;
    info["envelopeInDelay"] = m_slotInfo.envelopeInDelay;
    info["envelopeOutDelay"]= m_slotInfo.envelopeOutDelay;

    info["outputSmoothing"] = m_slotInfo.outputSmoothing;
    info["outputInterval" ] = m_slotInfo.outputInterval;

    return info;
}

int DigishowSlot::getEndpointInRange()
{
    if (m_sourceInterface != nullptr && m_sourceEndpointIndex != -1)
        return m_sourceInterface->endpointInfoList()->at(m_sourceEndpointIndex).range;
    return 0;
}

int DigishowSlot::getEndpointOutRange()
{
    if (m_destinationInterface != nullptr && m_destinationEndpointIndex != -1)
        return m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
    return 0;
}

int DigishowSlot::getEndpointInValue(bool pre)
{
    int value = -1;
    dgsSignalData dataIn = ( pre ? m_lastDataInPre : m_lastDataIn );

    switch (dataIn.signal) {
    case DATA_SIGNAL_ANALOG: value = dataIn.aValue; break;
    case DATA_SIGNAL_BINARY: value = dataIn.bValue; break;
    case DATA_SIGNAL_NOTE:   value = dataIn.bValue ? dataIn.aValue : 0; break;
    }
    return value;
}

int DigishowSlot::getEndpointOutValue(bool pre)
{
    int value = -1;
    dgsSignalData dataOut = ( pre ? m_lastDataOutPre : m_lastDataOut );

    switch (dataOut.signal) {
    case DATA_SIGNAL_ANALOG: value = dataOut.aValue; break;
    case DATA_SIGNAL_BINARY: value = dataOut.bValue; break;
    case DATA_SIGNAL_NOTE:   value = dataOut.bValue ? dataOut.aValue : 0; break;
    }
    return value;
}

bool DigishowSlot::isEndpointInBusy()
{
    return (elapsed()-m_dataInTimeLastReceived < 500);
}

bool DigishowSlot::isEndpointOutBusy()
{
    return (elapsed()-m_dataOutTimeLastSent < 500);
}

void DigishowSlot::setEndpointOutValue(int value, bool pre)
{
    dgsSignalData dataOut;

    switch (m_slotInfo.outputSignal) {
    case DATA_SIGNAL_ANALOG:
        dataOut.signal = DATA_SIGNAL_ANALOG;
        dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
        dataOut.aValue = value;
        break;
    case DATA_SIGNAL_BINARY:
        dataOut.signal = DATA_SIGNAL_BINARY;
        dataOut.bValue = (value>0 ? true : false);
        break;
    case DATA_SIGNAL_NOTE:
        dataOut.signal = DATA_SIGNAL_NOTE;
        dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
        dataOut.aValue = value;
        dataOut.bValue = (value>0 ? true : false);
        break;
    }

    if (dataOut.signal) {

        // finish the running envelope and smoothing
        if (envelopeIsRunning()) envelopeCancel();
        if (smoothingIsRunning()) smoothingCancel();

        prepareDataOut(dataOut, pre);
    }
}

int DigishowSlot::setEnabled(bool enabled)
{
    if (enabled) {

        // confirm source and destination have been selected properly
        bool sourceIsReady = false;
        if (m_sourceInterface != nullptr &&
            m_sourceEndpointIndex >=0 && m_sourceEndpointIndex < m_sourceInterface->endpointCount() &&
            m_slotInfo.inputSignal == m_sourceInterface->endpointInfoList()->at(m_sourceEndpointIndex).signal) {
            sourceIsReady = true;
        }

        bool destinationIsReady = false;
        if (m_destinationInterface != nullptr &&
            m_destinationEndpointIndex >=0 && m_destinationEndpointIndex < m_destinationInterface->endpointCount() &&
            m_slotInfo.outputSignal == m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).signal ) {
            destinationIsReady = true;
        }

        // set output interval option
        updateSlotOuputInterval();

        // clear input and output data
        m_lastDataInPre  = dgsSignalData();
        m_lastDataIn     = dgsSignalData();
        m_lastDataOutPre = dgsSignalData();
        m_lastDataOut    = dgsSignalData();

        // enable the slot
        if (sourceIsReady && destinationIsReady) {
            m_enabled = true;
            return ERR_NONE;
        }

        return ERR_INVALID_OPTION;
    }

    // finish the running envelope and smoothing
    if (envelopeIsRunning()) envelopeCancel();
    if (smoothingIsRunning()) smoothingCancel();

    // clear error flags
    m_trafficError = false;
    m_inputExpressionError = false;
    m_outputExpressionError = false;

    m_dataOutTimer.stop();
    m_enabled = false;
    return ERR_NONE;
}

int DigishowSlot::setLinked(bool linked) {

    m_linked = linked;
    return ERR_NONE;
}

int DigishowSlot::setSelected(bool selected) {

    m_selected = selected;
    return ERR_NONE;
}

void DigishowSlot::updateSlotInfoItem(const QString &name, const QVariant &value)
{
    static dgsSlotInfo newSlotInfo;

    if      ( name == "inputExpression" ) { m_slotInputExpression  = (value.isNull() || value == "value") ? QString() : value.toString(); m_inputExpressionError = false; }
    else if ( name == "outputExpression") { m_slotOutputExpression = (value.isNull() || value == "value") ? QString() : value.toString(); m_outputExpressionError = false; }

    else if ( name == "inputLow"        ) m_slotInfo.inputLow         = value.isNull() ? newSlotInfo.inputLow         : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "inputHigh"       ) m_slotInfo.inputHigh        = value.isNull() ? newSlotInfo.inputHigh        : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "outputLow"       ) m_slotInfo.outputLow        = value.isNull() ? newSlotInfo.outputLow        : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "outputHigh"      ) m_slotInfo.outputHigh       = value.isNull() ? newSlotInfo.outputHigh       : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "inputInverted"   ) m_slotInfo.inputInverted    = value.isNull() ? newSlotInfo.inputInverted    : value.toBool();
    else if ( name == "outputInverted"  ) m_slotInfo.outputInverted   = value.isNull() ? newSlotInfo.outputInverted   : value.toBool();
    else if ( name == "outputLowAsZero" ) m_slotInfo.outputLowAsZero  = value.isNull() ? newSlotInfo.outputLowAsZero  : value.toBool();
    else if ( name == "envelopeAttack"  ) m_slotInfo.envelopeAttack   = value.isNull() ? newSlotInfo.envelopeAttack   : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "envelopeHold"    ) m_slotInfo.envelopeHold     = value.isNull() ? newSlotInfo.envelopeHold     : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "envelopeDecay"   ) m_slotInfo.envelopeDecay    = value.isNull() ? newSlotInfo.envelopeDecay    : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "envelopeSustain" ) m_slotInfo.envelopeSustain  = value.isNull() ? newSlotInfo.envelopeSustain  : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "envelopeRelease" ) m_slotInfo.envelopeRelease  = value.isNull() ? newSlotInfo.envelopeRelease  : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "envelopeInDelay" ) m_slotInfo.envelopeInDelay  = value.isNull() ? newSlotInfo.envelopeInDelay  : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "envelopeOutDelay") m_slotInfo.envelopeOutDelay = value.isNull() ? newSlotInfo.envelopeOutDelay : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "outputSmoothing" ) m_slotInfo.outputSmoothing  = value.isNull() ? newSlotInfo.outputSmoothing  : MINMAX(value.toInt(), 0, 9000000);
    else if ( name == "outputInterval"  ) m_slotInfo.outputInterval   = value.isNull() ? newSlotInfo.outputInterval   : MINMAX(value.toInt(), 0, 60000);
}

void DigishowSlot::updateSlotOuputInterval()
{
    // read ouput interval option from slot settings
    m_outputInterval = m_slotInfo.outputInterval;

    if (m_destinationInterface != nullptr) {

        // read ouput interval option from endpoint settings
        if (m_destinationEndpointIndex >=0 &&
            m_destinationEndpointIndex < m_destinationInterface->endpointCount() ) {
            m_outputInterval = m_destinationInterface->endpointOptionsList()->at(m_destinationEndpointIndex).value("outputInterval").toInt();
        }

        // read ouput interval option from interface settings
        if (m_outputInterval==0) {
            m_outputInterval = m_destinationInterface->interfaceOptions()->value("outputInterval").toInt();
        }
    }
}

dgsSignalData DigishowSlot::processInputAnalog(dgsSignalData dataIn)
{
    dgsSignalData dataOut; // make a clear data package for ouptut

    double inputRatio = static_cast<double>(dataIn.aValue) / static_cast<double>(dataIn.aRange);
    if (m_slotInfo.inputInverted) inputRatio = 1.0 - inputRatio;

    if (m_slotInfo.outputSignal == DATA_SIGNAL_ANALOG) {

        // A to A
        double outputRatio;
        if (m_slotInfo.outputLowAsZero && inputRatio <= m_slotInfo.inputLow) {
            outputRatio = 0;
        } else {
            outputRatio = MAP(inputRatio, m_slotInfo.inputLow, m_slotInfo.inputHigh, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            outputRatio = MINMAX(outputRatio, m_slotInfo.outputLow, m_slotInfo.outputHigh);
        }
        if (m_slotInfo.outputInverted) outputRatio = 1.0 - outputRatio;

        dataOut.signal = DATA_SIGNAL_ANALOG;
        dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
        dataOut.aValue = static_cast<int>(round(outputRatio * dataOut.aRange));

        if (m_slotInfo.outputSmoothing > 0) {
            // smoothing output
            smoothingStart(dataOut);
            dataOut = dgsSignalData();
        }

    } else if  (m_slotInfo.outputSignal == DATA_SIGNAL_BINARY) {

        // A to B
        bool outputState;
        if (m_slotInfo.inputLow == 0 && m_slotInfo.inputHigh == 1) {
            // zero or none-zero
            outputState = (inputRatio != 0);
        } else {
            // outputState = (inputRatio >= m_slotInfo.inputLow && inputRatio <= m_slotInfo.inputHigh);
            int inputValue     = round(inputRatio * m_slotInfo.inputRange);
            int inputValueLow  = round(m_slotInfo.inputLow * m_slotInfo.inputRange);
            int inputValueHigh = round(m_slotInfo.inputHigh * m_slotInfo.inputRange);
            outputState = (inputValue >= inputValueLow && inputValue <= inputValueHigh);
        }
        if (m_slotInfo.outputInverted) outputState = !outputState;

        dataOut.signal = DATA_SIGNAL_BINARY;
        dataOut.bValue = outputState;

    } else if (m_slotInfo.outputSignal == DATA_SIGNAL_NOTE) {

        // A to N
        double lastInputRatio = m_lastDataIn.signal == 0 ? 0 : static_cast<double>(m_lastDataIn.aValue) / static_cast<double>(m_lastDataIn.aRange);
        if (m_slotInfo.inputInverted) lastInputRatio = 1.0 - lastInputRatio;

        bool isNoteOn = false;
        bool isNoteOff = false;
        if      (lastInputRatio <= m_slotInfo.inputLow && inputRatio > m_slotInfo.inputLow) isNoteOn = true;
        else if (lastInputRatio > m_slotInfo.inputLow && inputRatio <= m_slotInfo.inputLow) isNoteOff = true;

        if (isNoteOn || isNoteOff) {
            double outputRatio = MAP(inputRatio, m_slotInfo.inputLow, m_slotInfo.inputHigh, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            outputRatio = MINMAX(outputRatio, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            dataOut.signal = DATA_SIGNAL_NOTE;
            dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
            dataOut.aValue = static_cast<int>(round(outputRatio * dataOut.aRange));
            dataOut.bValue = isNoteOn;
        }
    }

    return dataOut;
}

dgsSignalData DigishowSlot::processInputBinary(dgsSignalData dataIn)
{
    bool inputState = dataIn.bValue;
    if (m_slotInfo.inputInverted) inputState = !inputState;

    // B to A
    // B to B
    // B to N
    if (inputState) {
        // state on
        envelopeStart(dataIn);
    } else {
        // state off
        m_envelopeTimeOff = elapsed();

        if (m_linked && !envelopeIsRunning()) {
            m_envelopeTimeOn  = -0xffffff;
            m_envelopeTimeOff = -0xffffff;
            onEnvelopeTimerFired();
        }
    }

    return dgsSignalData(); // return an empty data package
}

dgsSignalData DigishowSlot::processInputNote(dgsSignalData dataIn)
{
    // N to A
    // N to B
    // N to N
    if (dataIn.bValue && dataIn.aValue != 0) {
        // note on
        envelopeStart(dataIn);
    } else {
        // note off
        m_envelopeTimeOff = elapsed();

        if (m_linked && !envelopeIsRunning()) {
            m_envelopeTimeOn  = -0xffffff;
            m_envelopeTimeOff = -0xffffff;
            onEnvelopeTimerFired();
        }
    }

    return dgsSignalData(); // return an empty data package
}

void DigishowSlot::prepareDataOut(dgsSignalData dataOut, bool pre)
{
    // confirm output data is changed
    dgsSignalData lastDataOut = (pre ? m_lastDataOutPre : m_lastDataOut);
    if (lastDataOut.signal != 0 &&
        lastDataOut.aValue == dataOut.aValue &&
        lastDataOut.bValue == dataOut.bValue) return;

    if (pre) {

        // send data with preprocessing

        m_lastDataOutPre = dataOut;

        int elapsedSinceLastSent = static_cast<int>(elapsed() - m_dataOutTimeLastSent);
        if (m_outputInterval > 0 &&
            elapsedSinceLastSent < m_outputInterval) {

            // send data later by the timer (with traffic management)
            m_needSendDataOutLater = true;
            if (!m_dataOutTimer.isActive())
                m_dataOutTimer.start(m_outputInterval-elapsedSinceLastSent);

        } else {
            // send data instantly (without traffic management)
            sendDataOut(dataOut);
        }

    } else {

        // send data directly without preprocessing
        sendDataOut(dataOut, false);
    }

}

void DigishowSlot::sendDataOut(dgsSignalData dataOut, bool pre)
{
    // execute expression to preprocess the output data
    if (pre) {
        m_lastDataOutPre = dataOut;

        bool ok = true;
        dataOut = expressionExecute(m_slotOutputExpression, dataOut, SlotOutputEnd, &ok);
        if (dataOut.signal == 0) dataOut = m_lastDataOut;
        m_outputExpressionError = !ok;
    }

    // send data to the interface
    m_lastDataOut = dataOut;
    m_dataOutTimeLastSent = elapsed();
    m_destinationInterface->sendData(m_destinationEndpointIndex, dataOut);

}

void DigishowSlot::envelopeStart(dgsSignalData dataIn)
{
    if (envelopeIsRunning()) envelopeCancel();

    if (!m_linked) return;

    m_envelopeTimeOn = elapsed();
    m_envelopeTimeOff = 0;
    m_envelopeTimeLastUpdated = 0;
    m_envelopeRatio = 0;
    m_envelopeDataIn = dataIn;

    if (envelopeIsSet()) m_envelopeTimer.start();
    onEnvelopeTimerFired();
}

void DigishowSlot::envelopeCancel()
{
    m_envelopeTimeOn = 0;
    m_envelopeTimeOff = -0xffffff;
    m_envelopeTimeLastUpdated = 0;
    onEnvelopeTimerFired(); // it will stop envelope timer and output the last signal
}

bool DigishowSlot::envelopeIsRunning()
{
    return m_envelopeTimer.isActive();
}

bool DigishowSlot::envelopeIsSet()
{
    switch (m_slotInfo.outputSignal) {
    case DATA_SIGNAL_ANALOG:
        if (m_slotInfo.envelopeAttack > 0 ||
            m_slotInfo.envelopeHold > 0 ||
            m_slotInfo.envelopeDecay > 0 ||
            m_slotInfo.envelopeSustain < 1.0 ||
            m_slotInfo.envelopeRelease > 0 ||
            m_slotInfo.envelopeInDelay > 0 ||
            m_slotInfo.envelopeOutDelay > 0) return true;
        break;
    case DATA_SIGNAL_BINARY:
    case DATA_SIGNAL_NOTE:
        if (m_slotInfo.envelopeHold > 0 ||
            m_slotInfo.envelopeInDelay > 0 ||
            m_slotInfo.envelopeOutDelay > 0) return true;
        break;
    }
    return false;
}

dgsSignalData DigishowSlot::envelopeProcessOutputAnalog()
{
    double envelopeRatio = 0.0;
    bool isEnvelopeFinished = false;
    qint64 now = elapsed();

    // make output value in the envelope
    if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay ) {

        // in-delay
        return dgsSignalData();

    } else if (m_envelopeTimeOff == 0 || now < m_envelopeTimeOff + m_slotInfo.envelopeOutDelay) {

        if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeAttack ) {

            // attack
            envelopeRatio = static_cast<double>(now-m_envelopeTimeOn-m_slotInfo.envelopeInDelay) / static_cast<double>(m_slotInfo.envelopeAttack);

        } else if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold) {

            // hold
            envelopeRatio = 1.0;

        } else if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold + m_slotInfo.envelopeDecay) {

            // decay
            envelopeRatio =
                    static_cast<double>(1.0-m_slotInfo.envelopeSustain) *
                    static_cast<double>(m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold + m_slotInfo.envelopeDecay - now) /
                    static_cast<double>(m_slotInfo.envelopeDecay) + m_slotInfo.envelopeSustain;
        } else {

            // sustain
            envelopeRatio = m_slotInfo.envelopeSustain;
        }

        m_envelopeRatio = envelopeRatio;

    } else {

        if (now < m_envelopeTimeOff + m_slotInfo.envelopeOutDelay + m_slotInfo.envelopeRelease) {

            // release
            envelopeRatio =
                    //m_slotInfo.envelopeSustain *
                    m_envelopeRatio *
                    static_cast<double>(m_envelopeTimeOff + m_slotInfo.envelopeOutDelay + m_slotInfo.envelopeRelease - now) /
                    static_cast<double>(m_slotInfo.envelopeRelease);
        } else {

            // finish
            isEnvelopeFinished = true;
        }
    }

    double outputRatio = 0;

    dgsSignalData dataIn = m_envelopeDataIn;
    if (dataIn.signal == DATA_SIGNAL_BINARY) {

        outputRatio = envelopeRatio * (m_slotInfo.outputHigh - m_slotInfo.outputLow) + m_slotInfo.outputLow;

    } else if (dataIn.signal == DATA_SIGNAL_NOTE) {

        double inputRatio = static_cast<double>(dataIn.aValue) / static_cast<double>(dataIn.aRange);
        if (m_slotInfo.outputLowAsZero && (inputRatio <= m_slotInfo.inputLow || envelopeRatio==0)) {
            outputRatio = 0;
        } else {
            outputRatio = MAP(inputRatio, m_slotInfo.inputLow, m_slotInfo.inputHigh, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            outputRatio = MINMAX(outputRatio, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            outputRatio = envelopeRatio * (outputRatio - m_slotInfo.outputLow) + m_slotInfo.outputLow;
        }
    }

    if (m_slotInfo.outputInverted) outputRatio = 1.0 - outputRatio;

    // prepare output data package
    dgsSignalData dataOut;
    if (now - m_envelopeTimeLastUpdated >= 10 || isEnvelopeFinished) {

        dataOut.signal = DATA_SIGNAL_ANALOG;
        dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
        dataOut.aValue = static_cast<int>(round(outputRatio * dataOut.aRange));
        m_envelopeTimeLastUpdated = elapsed();
    }

    // finish the envelope if need
    if (isEnvelopeFinished) m_envelopeTimer.stop();

    return dataOut;
}

dgsSignalData DigishowSlot::envelopeProcessOutputBinary()
{
    bool envelopeState = false;
    bool isEnvelopeFinished = false;
    qint64 now = elapsed();

    // make output value in the envelope
    if (m_slotInfo.envelopeHold > 0) {

        // in-delay + hold
        if (m_envelopeTimeOff == 0) {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay) {
                if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeHold)
                    envelopeState = true;
                else
                    isEnvelopeFinished = true;
            }
        } else {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay &&
                now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeHold)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }

    } else {

        // in-delay + out-delay
        if (m_envelopeTimeOff == 0) {
            if (now >= m_envelopeTimeOn + m_slotInfo.envelopeInDelay) envelopeState = true;
        } else {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay &&
                now < m_envelopeTimeOff + m_slotInfo.envelopeOutDelay)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }
    }

    bool outputState = (m_slotInfo.outputInverted ? !envelopeState : envelopeState);

    // prepare output data package
    dgsSignalData dataOut;
    if (m_lastDataOutPre.signal == 0 || outputState != m_lastDataOutPre.bValue) {
        dataOut.signal = DATA_SIGNAL_BINARY;
        dataOut.bValue = outputState;
    }

    // finish the envelope if need
    if (isEnvelopeFinished) m_envelopeTimer.stop();

    return dataOut;
}

dgsSignalData DigishowSlot::envelopeProcessOutputNote()
{
    bool envelopeState = false;
    bool isEnvelopeFinished = false;
    qint64 now = elapsed();

    // make output value in the envelope
    if (m_slotInfo.envelopeHold > 0) {

        // in-delay + hold
        if (m_envelopeTimeOff == 0) {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay) {
                if (now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeHold)
                    envelopeState = true;
                else
                    isEnvelopeFinished = true;
            }
        } else {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay &&
                now < m_envelopeTimeOn + m_slotInfo.envelopeInDelay + m_slotInfo.envelopeHold)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }

    } else {

        // in-delay + out-delay
        if (m_envelopeTimeOff == 0) {
            if (now >= m_envelopeTimeOn + m_slotInfo.envelopeInDelay) envelopeState = true;
        } else {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeInDelay &&
                now < m_envelopeTimeOff + m_slotInfo.envelopeOutDelay)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }
    }

    bool outputState = (m_slotInfo.outputInverted ? !envelopeState : envelopeState);

    // prepare output data package
    dgsSignalData dataOut;
    if (m_lastDataOutPre.signal == 0 || outputState != m_lastDataOutPre.bValue) {

        dataOut.signal = DATA_SIGNAL_NOTE;
        dataOut.aRange = 127;
        dataOut.bValue = outputState;

        dgsSignalData dataIn = m_envelopeDataIn;

        if (dataIn.signal == DATA_SIGNAL_BINARY) {

            dataOut.aValue = static_cast<int>(round((dataOut.bValue ? m_slotInfo.outputHigh : m_slotInfo.outputLow) * dataOut.aRange));

        } else if (dataIn.signal == DATA_SIGNAL_NOTE) {

            double inputRatio = static_cast<double>(dataIn.aValue) / static_cast<double>(dataIn.aRange);
            double outputRatio;
            outputRatio = MAP(inputRatio, m_slotInfo.inputLow, m_slotInfo.inputHigh, m_slotInfo.outputLow, m_slotInfo.outputHigh);
            outputRatio = MINMAX(outputRatio, m_slotInfo.outputLow, m_slotInfo.outputHigh);

            dataOut.aValue = static_cast<int>(round(outputRatio * dataOut.aRange));
        }
    }

    // finish the envelope if need
    if (isEnvelopeFinished) m_envelopeTimer.stop();

    return dataOut;
}

void DigishowSlot::smoothingStart(dgsSignalData dataOut)
{
    if (!m_linked) return;

    if (smoothingIsRunning() && m_smoothingDataOutTo.aValue == dataOut.aValue) return;

    m_smoothingTimeStart = elapsed();
    m_smoothingDataOutFrom = m_lastDataOutPre;
    m_smoothingDataOutTo = dataOut;

    m_smoothingTimer.start();
    onSmoothingTimerFired();
}

void DigishowSlot::smoothingCancel()
{
    m_smoothingTimer.stop();
}

bool DigishowSlot::smoothingIsRunning()
{
    return m_smoothingTimer.isActive();
}

dgsSignalData DigishowSlot::smoothingProcessOutputAnalog()
{
    dgsSignalData dataOut = m_smoothingDataOutTo;

    qint64 smoothingDuration = m_slotInfo.outputSmoothing;
    qint64 smoothingElapsed = elapsed() - m_smoothingTimeStart + m_smoothingTimer.interval();

    double ratio = double(smoothingElapsed) / double(smoothingDuration);
    if (ratio >= 0 && ratio<1) {
        // ongoing
        dataOut.aValue = round((m_smoothingDataOutTo.aValue - m_smoothingDataOutFrom.aValue)*ratio) + m_smoothingDataOutFrom.aValue;
    } else {
        // finished
        m_smoothingTimer.stop();
    }

    return dataOut;
}

dgsSignalData DigishowSlot::expressionExecute(const QString & expression, dgsSignalData dataIn, int slotEnd, bool *ok)
{
    if (expression.isEmpty()) return dataIn;

    DigishowScriptable* scriptable = g_app->scriptable();
    int inputValue = 0, inputRange = 1;
    switch (dataIn.signal) {
    case DATA_SIGNAL_ANALOG: inputValue = dataIn.aValue; inputRange = dataIn.aRange; break;
    case DATA_SIGNAL_BINARY: inputValue = dataIn.bValue; inputRange = 1; break;
    case DATA_SIGNAL_NOTE:   inputValue = dataIn.bValue ? dataIn.aValue : 0; inputRange = 127; break;
    }

    int lastValue = (slotEnd == SlotInputEnd ? getEndpointInValue() : getEndpointOutValue());
    if (lastValue < 0) lastValue = 0;

    int slotIndex = m_slotIndex; //g_app->getSlotIndex(this);

    int outputValue = scriptable->execute(expression, inputValue, inputRange, lastValue, slotIndex, slotEnd, ok);
    if (*ok == false) return dataIn;
    if (outputValue < 0) return dgsSignalData();

    dgsSignalData dataOut = dataIn;
    switch (dataOut.signal) {
    case DATA_SIGNAL_ANALOG: dataOut.aValue = MINMAX(outputValue, 0, dataOut.aRange); break;
    case DATA_SIGNAL_BINARY: dataOut.bValue = (outputValue != 0); break;
    case DATA_SIGNAL_NOTE:   dataOut.aValue = MINMAX(outputValue, 0, dataOut.aRange); dataOut.bValue = (outputValue != 0); break;
    }
    return dataOut;
}

void DigishowSlot::onInterfaceMetadataUpdated()
{
    m_slotInfo.inputRange = getEndpointInRange();
    m_slotInfo.outputRange = getEndpointOutRange();

    if (m_lastDataIn.signal == DATA_SIGNAL_ANALOG && m_lastDataIn.aRange != m_slotInfo.inputRange) {
        m_lastDataInPre = dgsSignalData();
        m_lastDataIn    = dgsSignalData();
    }

    if (m_lastDataOut.signal == DATA_SIGNAL_ANALOG && m_lastDataOut.aRange != m_slotInfo.outputRange) {
        m_lastDataOutPre = dgsSignalData();
        m_lastDataOut    = dgsSignalData();
    }

    emit metadataUpdated();
}

void DigishowSlot::onDataReceived(int endpointIndex, dgsSignalData dataIn)
{
    // only process data packages received from the relevant endpoint
    if (m_sourceEndpointIndex != endpointIndex) return;

    // confirm input data is changed
    dgsSignalData lastDataIn = m_lastDataInPre;
    if (lastDataIn.signal != 0 &&
        lastDataIn.aValue == dataIn.aValue &&
        lastDataIn.bValue == dataIn.bValue) return;
    m_lastDataInPre = dataIn;

    m_dataInTimeLastReceived = elapsed();

    // execute expression to preprocess the received data
    bool ok = true;
    dataIn = expressionExecute(m_slotInputExpression, dataIn, SlotInputEnd, &ok);
    if (dataIn.signal == 0) dataIn = m_lastDataIn;
    m_inputExpressionError = !ok;

    // confirm a destination has been assigned
    if (!m_enabled || m_destinationInterface == nullptr) {
        m_lastDataIn = dataIn;
        return;
    }

    // stop processing the received data if there is too much traffic
    m_trafficError = (m_dataInProcessingStackLevel > 16);
    if (m_trafficError) return;
    m_dataInProcessingStackLevel += 1;

    // process the data package received from the source
    // to make a data package will be sent to the destination
    dgsSignalData dataOut;
    switch (dataIn.signal) {
    case DATA_SIGNAL_ANALOG: dataOut = processInputAnalog(dataIn); break;
    case DATA_SIGNAL_BINARY: dataOut = processInputBinary(dataIn); break;
    case DATA_SIGNAL_NOTE:   dataOut = processInputNote(dataIn);   break;
    }

    m_lastDataIn = dataIn;

    // send data package to destination
    if (dataOut.signal && m_linked) {
        prepareDataOut(dataOut);
    }

    m_dataInProcessingStackLevel -= 1;
}

void DigishowSlot::onDataPrepared(int endpointIndex, dgsSignalData dataOut, bool pre)
{
    // only process data packages prepared for the relevant endpoint
    if (m_destinationEndpointIndex != endpointIndex) return;

    prepareDataOut(dataOut, pre);
}

void DigishowSlot::onDataSent(int endpointIndex, dgsSignalData dataOut)
{
    // only process data packages prepared for the relevant endpoint
    if (m_destinationEndpointIndex != endpointIndex) return;

    if (m_lastDataOut.signal == 0 ||
        m_lastDataOut.aValue != dataOut.aValue ||
        m_lastDataOut.bValue != dataOut.bValue)
        m_lastDataOutPre = dgsSignalData();

    m_lastDataOut = dataOut;
}

void DigishowSlot::onDataOutTimerFired()
{
    if (m_needSendDataOutLater) {
        sendDataOut(m_lastDataOutPre);
        m_needSendDataOutLater = false;
    }
}

void DigishowSlot::onEnvelopeTimerFired()
{
    dgsSignalData dataOut;
    switch (m_slotInfo.outputSignal) {
    case DATA_SIGNAL_ANALOG: dataOut = envelopeProcessOutputAnalog(); break;
    case DATA_SIGNAL_BINARY: dataOut = envelopeProcessOutputBinary(); break;
    case DATA_SIGNAL_NOTE:   dataOut = envelopeProcessOutputNote();   break;
    }

    // send data package to destination
    if (dataOut.signal) {
        //qDebug() << "envelope_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;

        if (m_slotInfo.outputSignal == DATA_SIGNAL_ANALOG &&
            m_slotInfo.outputSmoothing > 0)
            smoothingStart(dataOut);
        else
            prepareDataOut(dataOut);
    }
}

void DigishowSlot::onSmoothingTimerFired()
{
    dgsSignalData dataOut;
    switch (m_slotInfo.outputSignal) {
    case DATA_SIGNAL_ANALOG: dataOut = smoothingProcessOutputAnalog(); break;
    }

    // send data package to destination
    if (dataOut.signal) {
        //qDebug() << "smoothing_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;
        prepareDataOut(dataOut);
    }
}
