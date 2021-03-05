#include "digishow_slot.h"
#include "digishow_interface.h"
#include <math.h>
#include <QJsonDocument>

#define MINMAX(v,a,b) ( (v) < (a) ? (a) : ((v) > (b) ? (b) : (v)) )
#define MIN(v,a) ( (v) < (a) ? (a) )
#define MAX(v,b) ( (v) > (b) ? (b) : (v) )
#define MAP(v,a,b,A,B) ( ((B)-(A)) * ((v)-(a)) / ((b)-(a)) + (A) )

DigishowSlot::DigishowSlot(QObject *parent) : QObject(parent)
{
    // no source and no destination is assigned
    m_sourceInterface          = nullptr;
    m_destinationInterface     = nullptr;
    m_sourceEndpointIndex      = -1;
    m_destinationEndpointIndex = -1;

    // get ready to run slot
    m_enabled = false;
    m_linked = true;
    m_outputInterval = 0;
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
    m_lastDataIn = dgsSignalData();
    m_lastDataOut = dgsSignalData();

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
        m_lastDataIn = dgsSignalData();

        connect(m_sourceInterface, SIGNAL(dataReceived(int, dgsSignalData)), this, SLOT(onDataReceived(int, dgsSignalData)));
    }

    return ERR_NONE;
}

int DigishowSlot::setDestination(DigishowInterface *interface, int endpointIndex)
{
    // release old destination connection
    if (m_destinationInterface != nullptr) {
        disconnect(m_destinationInterface, SIGNAL(dataPrepared(int, dgsSignalData)), this, nullptr);
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
        m_lastDataOut = dgsSignalData();

        // set output interval option
        updateSlotOuputInterval();

        connect(m_destinationInterface, SIGNAL(dataPrepared(int, dgsSignalData)), this, SLOT(onDataPrepared(int, dgsSignalData)));
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

int DigishowSlot::setSlotOption(const QString &name, const QVariant &value)
{
    m_slotOptions[name] = value;
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

    info["outputSmoothing"] = m_slotInfo.outputSmoothing;
    info["outputInterval" ] = m_slotInfo.outputInterval;

    return info;
}

int DigishowSlot::getEndpointInValue()
{
    int value = -1;
    switch (m_lastDataIn.signal) {
    case DATA_SIGNAL_ANALOG: value = m_lastDataIn.aValue; break;
    case DATA_SIGNAL_BINARY: value = m_lastDataIn.bValue; break;
    case DATA_SIGNAL_NOTE:   value = m_lastDataIn.bValue ? m_lastDataIn.aValue : 0; break;
    }
    return value;
}

int DigishowSlot::getEndpointOutValue()
{
    int value = -1;
    switch (m_lastDataOut.signal) {
    case DATA_SIGNAL_ANALOG: value = m_lastDataOut.aValue; break;
    case DATA_SIGNAL_BINARY: value = m_lastDataOut.bValue; break;
    case DATA_SIGNAL_NOTE:   value = m_lastDataOut.bValue ? m_lastDataOut.aValue : 0; break;
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

void DigishowSlot::setEndpointOutValue(int value)
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

    if (dataOut.signal) sendDataOut(dataOut);
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
        m_lastDataIn = dgsSignalData();
        m_lastDataOut = dgsSignalData();

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

    m_dataOutTimer.stop();
    m_enabled = false;
    return ERR_NONE;
}

int DigishowSlot::setLinked(bool linked) {

    m_linked = linked;
    return ERR_NONE;
}

void DigishowSlot::updateSlotInfoItem(const QString &name, const QVariant &value)
{
    static dgsSlotInfo newSlotInfo;

    if      ( name == "inputLow"       ) m_slotInfo.inputLow        = value.isNull() ? newSlotInfo.inputLow        : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "inputHigh"      ) m_slotInfo.inputHigh       = value.isNull() ? newSlotInfo.inputHigh       : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "outputLow"      ) m_slotInfo.outputLow       = value.isNull() ? newSlotInfo.outputLow       : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "outputHigh"     ) m_slotInfo.outputHigh      = value.isNull() ? newSlotInfo.outputHigh      : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "inputInverted"  ) m_slotInfo.inputInverted   = value.isNull() ? newSlotInfo.inputInverted   : value.toBool();
    else if ( name == "outputInverted" ) m_slotInfo.outputInverted  = value.isNull() ? newSlotInfo.outputInverted  : value.toBool();
    else if ( name == "outputLowAsZero") m_slotInfo.outputLowAsZero = value.isNull() ? newSlotInfo.outputLowAsZero : value.toBool();
    else if ( name == "envelopeAttack" ) m_slotInfo.envelopeAttack  = value.isNull() ? newSlotInfo.envelopeAttack  : MINMAX(value.toInt(), 0, 60000);
    else if ( name == "envelopeHold"   ) m_slotInfo.envelopeHold    = value.isNull() ? newSlotInfo.envelopeHold    : MINMAX(value.toInt(), 0, 60000);
    else if ( name == "envelopeDecay"  ) m_slotInfo.envelopeDecay   = value.isNull() ? newSlotInfo.envelopeDecay   : MINMAX(value.toInt(), 0, 60000);
    else if ( name == "envelopeSustain") m_slotInfo.envelopeSustain = value.isNull() ? newSlotInfo.envelopeSustain : MINMAX(value.toDouble(), 0.0, 1.0);
    else if ( name == "envelopeRelease") m_slotInfo.envelopeRelease = value.isNull() ? newSlotInfo.envelopeRelease : MINMAX(value.toInt(), 0, 60000);
    else if ( name == "outputSmoothing") m_slotInfo.outputSmoothing = value.isNull() ? newSlotInfo.outputSmoothing : MINMAX(value.toInt(), 0, 60000);
    else if ( name == "outputInterval" ) m_slotInfo.outputInterval  = value.isNull() ? newSlotInfo.outputInterval  : MINMAX(value.toInt(), 0, 60000);
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
        double lastInputRatio = static_cast<double>(m_lastDataIn.aValue) / static_cast<double>(m_lastDataIn.aRange);
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
    dgsSignalData dataOut; // make a clear data package for ouptut

    bool inputState = dataIn.bValue;
    if (m_slotInfo.inputInverted) inputState = !inputState;

    if (m_slotInfo.outputSignal == DATA_SIGNAL_ANALOG) {

        // B to A
        double outputRatio = (inputState ? m_slotInfo.outputHigh : m_slotInfo.outputLow);
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

        // B to B
        bool outputState = (m_slotInfo.outputInverted ? !inputState : inputState);

        dataOut.signal = DATA_SIGNAL_BINARY;
        dataOut.bValue = outputState;

    } else if (m_slotInfo.outputSignal == DATA_SIGNAL_NOTE) {

        // B to N
        bool lastInputState = m_lastDataIn.bValue;
        if (m_slotInfo.inputInverted) lastInputState = !lastInputState;

        bool isNoteOn = false;
        bool isNoteOff = false;
        if      (lastInputState == false && inputState == true) isNoteOn = true;
        else if (lastInputState == true && inputState == false) isNoteOff = true;

        if (isNoteOn || isNoteOff) {
            dataOut.signal = DATA_SIGNAL_NOTE;
            dataOut.aRange = m_destinationInterface->endpointInfoList()->at(m_destinationEndpointIndex).range;
            dataOut.aValue = static_cast<int>(round((isNoteOn ? m_slotInfo.outputHigh : m_slotInfo.outputLow) * dataOut.aRange));
            dataOut.bValue = isNoteOn;
        }

    }

    return dataOut;
}

dgsSignalData DigishowSlot::processInputNote(dgsSignalData dataIn)
{
    dgsSignalData dataOut; // make a clear data package for ouptut

    if (m_slotInfo.outputSignal == DATA_SIGNAL_ANALOG ||
        m_slotInfo.outputSignal == DATA_SIGNAL_BINARY) {

        // N to A or B (envelope)
        if (dataIn.bValue && dataIn.aValue != 0) {
            // note on
            envelopeStart(dataIn);
        } else {
            // note off
            m_envelopeTimeOff = elapsed();
        }

    } else if (m_slotInfo.outputSignal == DATA_SIGNAL_NOTE) {

        // N to N (pass through)
        dataOut = dataIn;
    }

    return dataOut;
}

void DigishowSlot::sendDataOut(dgsSignalData dataOut)
{
    // confirm output data is changed
    if (dataOut.signal == m_lastDataOut.signal) {
        if ((dataOut.signal == DATA_SIGNAL_ANALOG &&
             dataOut.aRange == m_lastDataOut.aRange &&
             dataOut.aValue == m_lastDataOut.aValue) ||
            (dataOut.signal == DATA_SIGNAL_BINARY &&
             dataOut.bValue == m_lastDataOut.bValue)) return;
    }

    m_lastDataOut = dataOut;

    int elapsedSinceLastSent = static_cast<int>(elapsed() - m_dataOutTimeLastSent);

    if (m_outputInterval > 0 &&
        elapsedSinceLastSent < m_outputInterval) {

        // send data later by the timer (with traffic management)
        m_needSendDataOutLater = true;
        if (!m_dataOutTimer.isActive())
            m_dataOutTimer.start(m_outputInterval-elapsedSinceLastSent);

    } else {
        // send data instantly (without traffic management)
        m_dataOutTimeLastSent = elapsed();
        m_destinationInterface->sendData(m_destinationEndpointIndex, dataOut);
    }
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
    m_envelopeTimer.start();
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

dgsSignalData DigishowSlot::envelopeProcessOutputAnalog()
{
    double envelopeRatio = 0.0;
    bool isEnvelopeFinished = false;
    qint64 now = elapsed();

    // make output value in the envelope
    if (m_envelopeTimeOff == 0) {
        if (now < m_envelopeTimeOn + m_slotInfo.envelopeAttack ) {

            // attack
            envelopeRatio = static_cast<double>(now-m_envelopeTimeOn) / static_cast<double>(m_slotInfo.envelopeAttack);

        } else if (now < m_envelopeTimeOn + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold) {

            // hold
            envelopeRatio = 1.0;

        } else if (now < m_envelopeTimeOn + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold + m_slotInfo.envelopeDecay) {

            // decay
            envelopeRatio =
                    static_cast<double>(1.0-m_slotInfo.envelopeSustain) *
                    static_cast<double>(m_envelopeTimeOn + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold + m_slotInfo.envelopeDecay - now) /
                    static_cast<double>(m_slotInfo.envelopeDecay) + m_slotInfo.envelopeSustain;
        } else {

            // sustain
            envelopeRatio = m_slotInfo.envelopeSustain;
        }

        m_envelopeRatio = envelopeRatio;

    } else {

        if (now < m_envelopeTimeOff + m_slotInfo.envelopeRelease) {

            // release
            envelopeRatio =
                    //m_slotInfo.envelopeSustain *
                    m_envelopeRatio *
                    static_cast<double>(m_envelopeTimeOff + m_slotInfo.envelopeRelease - now) /
                    static_cast<double>(m_slotInfo.envelopeRelease);
        } else {

            // finish
            isEnvelopeFinished = true;
        }
    }

    double inputRatio = static_cast<double>(m_envelopeDataIn.aValue) / static_cast<double>(m_envelopeDataIn.aRange);
    double outputRatio;
    if (m_slotInfo.outputLowAsZero && (inputRatio <= m_slotInfo.inputLow || envelopeRatio==0)) {
        outputRatio = 0;
    } else {
        outputRatio = MAP(inputRatio, m_slotInfo.inputLow, m_slotInfo.inputHigh, m_slotInfo.outputLow, m_slotInfo.outputHigh);
        outputRatio = MINMAX(outputRatio, m_slotInfo.outputLow, m_slotInfo.outputHigh);
        outputRatio = envelopeRatio * (outputRatio - m_slotInfo.outputLow) + m_slotInfo.outputLow;
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

        // attack + hold
        if (now > m_envelopeTimeOn + m_slotInfo.envelopeAttack) {
            if (now < m_envelopeTimeOn + m_slotInfo.envelopeAttack + m_slotInfo.envelopeHold)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }

    } else {

        // attack + release
        if (m_envelopeTimeOff == 0) {
            if (now > m_envelopeTimeOn + m_slotInfo.envelopeAttack) envelopeState = true;
        } else {
            if (now < m_envelopeTimeOff + m_slotInfo.envelopeRelease)
                envelopeState = true;
            else
                isEnvelopeFinished = true;
        }
    }

    bool outputState = (m_slotInfo.outputInverted ? !envelopeState : envelopeState);

    // prepare output data package
    dgsSignalData dataOut;
    if (outputState != m_lastDataOut.bValue) {

        dataOut.signal = DATA_SIGNAL_BINARY;
        dataOut.bValue = outputState;
    }

    // finish the envelope if need
    if (isEnvelopeFinished) m_envelopeTimer.stop();

    return dataOut;
}

void DigishowSlot::smoothingStart(dgsSignalData dataOut)
{
    if (!m_linked) return;

    m_smoothingTimeStart = elapsed();
    m_smoothingDataOutFrom = m_lastDataOut;
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

void DigishowSlot::onDataReceived(int endpointIndex, dgsSignalData dataIn)
{
    // only process data packages received from the relevant endpoint
    if (m_sourceEndpointIndex != endpointIndex) return;

    if (g_needLogCtl)
        qDebug() << "slot_in:" << m_sourceEndpointIndex << dataIn.signal << dataIn.aValue << dataIn.aRange << dataIn.bValue;

    m_dataInTimeLastReceived = elapsed();

    // confirm a destination has been assigned
    if (!m_enabled || m_destinationInterface == nullptr) {
        m_lastDataIn = dataIn;
        return;
    }

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
    if (!m_linked) return;
    if (dataOut.signal) {
        if (g_needLogCtl) qDebug() << "slot_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;
        sendDataOut(dataOut);
    }
}

void DigishowSlot::onDataPrepared(int endpointIndex, dgsSignalData dataOut)
{
    // only process data packages prepared for the relevant endpoint
    if (m_destinationEndpointIndex != endpointIndex) return;

    if (g_needLogCtl) qDebug() << "slot_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;

    sendDataOut(dataOut);
}

void DigishowSlot::onDataOutTimerFired()
{
    if (m_needSendDataOutLater) {
        m_dataOutTimeLastSent = elapsed();
        m_destinationInterface->sendData(m_destinationEndpointIndex, m_lastDataOut);
        m_needSendDataOutLater = false;
    }
}

void DigishowSlot::onEnvelopeTimerFired()
{
    dgsSignalData dataOut;
    switch (m_slotInfo.outputSignal) {
    case DATA_SIGNAL_ANALOG: dataOut = envelopeProcessOutputAnalog(); break;
    case DATA_SIGNAL_BINARY: dataOut = envelopeProcessOutputBinary(); break;
    }

    // send data package to destination
    if (dataOut.signal) {
        if (g_needLogCtl) qDebug() << "envelope_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;
        sendDataOut(dataOut);
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
        if (g_needLogCtl) qDebug() << "smoothing_out:" << m_destinationEndpointIndex << dataOut.signal << dataOut.aValue << dataOut.aRange << dataOut.bValue;
        sendDataOut(dataOut);
    }
}
