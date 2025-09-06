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

#include "digishow_environment.h"
#include "dgs_rioc_interface.h"
#include "rioc_controller.h"
#include "rioc_aladdin2560_def.h"

#include <QSerialPortInfo>

DgsRiocInterface::DgsRiocInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "rioc";
    m_rioc = nullptr;

    m_timerStarting = nullptr;
    m_startedUnitCount = 0;
}

DgsRiocInterface::~DgsRiocInterface()
{
    closeInterface();
}

int DgsRiocInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // create a rioc controller
    m_rioc = new RiocController(nullptr, false);
    m_rioc->setLoggerEnabled(false);

    // connect rioc units
    QString comPort = m_interfaceOptions.value("comPort").toString();
    bool done = false;
    if (m_rioc->connect(comPort)) {

        // set callback
        connect(m_rioc, SIGNAL(unitStarted(unsigned char)), this, SLOT(onUnitStarted(unsigned char)));
        connect(m_rioc, SIGNAL(digitalInValueUpdated(unsigned char, unsigned char, bool)), this, SLOT(onDigitalInValueUpdated(unsigned char, unsigned char, bool)));
        connect(m_rioc, SIGNAL(analogInValueUpdated(unsigned char, unsigned char, int)), this, SLOT(onAnalogInValueUpdated(unsigned char, unsigned char, int)));
        connect(m_rioc, SIGNAL(encoderValueUpdated(unsigned char, unsigned char, int)), this, SLOT(onEncoderValueUpdated(unsigned char, unsigned char, int)));
        connect(m_rioc, SIGNAL(userChannelValueUpdated(unsigned char, unsigned char, int)), this, SLOT(onUserChannelValueUpdated(unsigned char, unsigned char, int)));

        // reset all rioc units
        m_rioc->resetUnit(0xff);

        // need to check after a few seconds
        // whether the rioc units are started
        m_timerStarting = new QTimer();
        connect(m_timerStarting, SIGNAL(timeout()), this, SLOT(onStartingTimeout()));
        m_timerStarting->setSingleShot(true);
        m_timerStarting->setInterval(6000);
        m_timerStarting->start();
        m_startedUnitCount = 0;

        done = true;
    }

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsRiocInterface::closeInterface()
{
    if (m_timerStarting != nullptr) {
        m_timerStarting->stop();
        delete m_timerStarting;
        m_timerStarting = nullptr;
        m_startedUnitCount = 0;
    }

    if (m_rioc != nullptr) {
        delete m_rioc;
        m_rioc = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsRiocInterface::init()
{
    // nothing to do here
    // we will set initial signal values for all endpoints after unit started
    return ERR_NONE;
}

int DgsRiocInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if  (r != ERR_NONE) return r;

    // confirm the endpoint is ready
    dgsEndpointInfo endpointInfo = m_endpointInfoList[endpointIndex];
    if (endpointInfo.ready == false) return ERR_DEVICE_NOT_READY;

    unsigned char unit = static_cast<unsigned char>(endpointInfo.unit);
    unsigned char channel = static_cast<unsigned char>(endpointInfo.channel);

    int type = endpointInfo.type;
    if (type == ENDPOINT_RIOC_DIGITAL_OUT) {

        // write digital out channel
        if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
        m_rioc->digitalOutWrite(unit, channel, data.bValue);

    } else if (type == ENDPOINT_RIOC_ANALOG_OUT) {

        // write analog out channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int value = 0xffff * data.aValue / (data.aRange==0 ? 0xffff : data.aRange);
        if (value<0 || value>0xffff) return ERR_INVALID_DATA;
        m_rioc->analogOutWrite(unit, channel, value);

    } else if (type == ENDPOINT_RIOC_PWM_OUT) {

        // write pwm out channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int value = 255 * data.aValue / (data.aRange==0 ? 255 : data.aRange);
        if (value<0 || value>255) return ERR_INVALID_DATA;
        m_rioc->digitalOutWritePwm(unit, channel, value);

    } else if (type == ENDPOINT_RIOC_PFM_OUT) {

        // write pfm out channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int range = endpointInfo.range;
        int value = (qint64)range * data.aValue / (data.aRange==0 ? range : data.aRange);
        if (value<0 || value>range) return ERR_INVALID_DATA;
        if (value>0)
            m_rioc->tonePlay(unit, channel, value);
        else
            m_rioc->toneStop(unit, channel);

    } else if (type == ENDPOINT_RIOC_RUDDER_OUT) {

        // write rudder out channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int value = 180 * data.aValue / (data.aRange==0 ? 180 : data.aRange);
        if (value<0 || value>180) return ERR_INVALID_DATA;
        m_rioc->rudderSetAngle(unit, channel, value);

    } else if (type == ENDPOINT_RIOC_STEPPER_OUT) {

        // write stepper out channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int range = endpointInfo.range;
        int value = (qint64)range * data.aValue / (data.aRange==0 ? range : data.aRange);
        if (value<0 || value>range) return ERR_INVALID_DATA;
        m_rioc->stepperGoto(unit, channel, value);

    } else if (type == ENDPOINT_RIOC_STEPPER_SET) {

        // set stepper parameters
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int range = endpointInfo.range;
        int control = endpointInfo.control;
        int value = (qint64)range * data.aValue / (data.aRange==0 ? range : data.aRange);
        if (value<0 || value>range) return ERR_INVALID_DATA;

        switch (control) {
        case CONTROL_MOTION_POSITION:
            m_rioc->stepperSetPosition(unit, channel, value);
            break;
        case CONTROL_MOTION_SPEED:
            m_rioc->stepperSetSpeed(unit, channel, value);
            break;
        }

    } else if (type == ENDPOINT_RIOC_ENCODER_IN) {

        // write encoder (set encoder value)
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int range = endpointInfo.range;
        int value = (qint64)range * data.aValue / (data.aRange==0 ? range : data.aRange);
        if (value<0 || value>range) return ERR_INVALID_DATA;
        m_rioc->encoderWrite(unit, channel, value);

    } else if (type == ENDPOINT_RIOC_USER_CHANNEL) {

        // write user channel
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;
        int range = endpointInfo.range;
        int value = (qint64)range * data.aValue / (data.aRange==0 ? range : data.aRange);
        if (value<0 || value>range) return ERR_INVALID_DATA;
        m_rioc->userChannelWrite(unit, channel, value);
    }

    return ERR_NONE;
}

int DgsRiocInterface::findEndpoint(int unit, int type, int channel)
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        if (m_endpointInfoList[n].unit == unit &&
            m_endpointInfoList[n].type == type &&
            m_endpointInfoList[n].channel == channel) return n;
    }

    return -1;
}

void DgsRiocInterface::onStartingTimeout()
{
    if  (m_startedUnitCount == 0) {
        emit errorDetected(
                    1,
                    tr("DigiShow doesn't seem to work with your Arduino yet, \r\nmake sure the DigiShow RIOC sketch is uploaded to the Arduino."));
    }
}

void DgsRiocInterface::onUnitStarted(unsigned char unit)
{
    //qDebug() << "DgsRiocInterface::onUnitStarted" << unit;

    m_startedUnitCount++;

    // dynamically configure all channels when the unit started
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].unit == unit &&
            m_endpointInfoList[n].enabled) {

            int type = m_endpointInfoList[n].type;
            unsigned char channel = static_cast<unsigned char>(m_endpointInfoList[n].channel);

            bool done = false;
            if (type==ENDPOINT_RIOC_DIGITAL_OUT || type==ENDPOINT_RIOC_PWM_OUT) {

                // set up digital out channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                done = m_rioc->setupObject(unit, RO_GENERAL_DIGITAL_OUT, channel,
                                           static_cast<unsigned char>(mode));

            } else if (type==ENDPOINT_RIOC_DIGITAL_IN) {

                // set up digital in channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                int filterLevel = m_endpointOptionsList[n].value("optFilterLevel").toInt();
                int samplingInterval = m_endpointOptionsList[n].value("optSamplingInterval").toInt();

                m_rioc->setupObject(unit, RO_GENERAL_DIGITAL_IN, channel,
                                    static_cast<unsigned char>(mode),
                                    static_cast<unsigned char>(filterLevel),
                                    static_cast<unsigned char>(samplingInterval >> 8),
                                    static_cast<unsigned char>(samplingInterval & 0xff));

                // read value
                bool value;
                if (m_rioc->digitalInRead(unit, channel, &value)) {
                    this->onDigitalInValueUpdated(unit, channel, value);
                }

                done = m_rioc->digitalInSetNotification(unit, channel, true);

            } else if (type==ENDPOINT_RIOC_ANALOG_OUT) {

                // set up analog out channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                done = m_rioc->setupObject(unit, RO_GENERAL_ANALOG_OUT, channel,
                                           static_cast<unsigned char>(mode));

            } else if (type==ENDPOINT_RIOC_ANALOG_IN) {

                // setup analog in channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                int filterLevel = m_endpointOptionsList[n].value("optFilterLevel").toInt();
                int samplingInterval = m_endpointOptionsList[n].value("optSamplingInterval").toInt();

                m_rioc->setupObject(unit, RO_GENERAL_ANALOG_IN, channel,
                                    static_cast<unsigned char>(mode),
                                    static_cast<unsigned char>(filterLevel),
                                    static_cast<unsigned char>(samplingInterval >> 8),
                                    static_cast<unsigned char>(samplingInterval & 0xff));

                // read value
                int value;
                if (m_rioc->analogInRead(unit, channel, &value)) {
                    this->onAnalogInValueUpdated(unit, channel, value);
                }

                int updatingInterval = m_endpointOptionsList[n].value("optUpdatingInterval").toInt();
                if (updatingInterval==0) updatingInterval = 50;

                done = m_rioc->analogInSetNotification(unit, channel, true, updatingInterval);

            } else if (type==ENDPOINT_RIOC_PFM_OUT) {

                // set up pfm out channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                done = m_rioc->setupObject(unit, RO_SOUND_TONE, channel,
                                           static_cast<unsigned char>(mode));

            } else if (type==ENDPOINT_RIOC_ENCODER_IN) {

                // set up encoder in channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                int samplingInterval = m_endpointOptionsList[n].value("optSamplingInterval").toInt();

                int pins = 2;
                unsigned char pin[2] = { 0, 0 };
                if (m_interfaceInfo.mode == INTERFACE_RIOC_ALADDIN ||
                    m_interfaceInfo.mode == INTERFACE_RIOC_PLC1 ||
                    m_interfaceInfo.mode == INTERFACE_RIOC_PLC2 ) {
                    // aladdin or plc pins
                    QList<int>dinPinList = DigishowEnvironment::getRiocPinList(m_interfaceInfo.mode, PIN_TYPE_DI);
                    for (int p=0 ; p<=(dinPinList.length()-pins) ; p++) {
                        if (dinPinList[p] == channel) {
                            for (int i=0 ; i<pins ; i++) pin[i] = dinPinList[p+i];
                            break;
                        }
                    }
                } else {
                    // common arduino pins
                    for (int i=0 ; i<pins ; i++) pin[i] = channel+i;
                }
                m_rioc->setupObject(unit, RO_SENSOR_ENCODER, pin[0], pin[1],
                                    static_cast<unsigned char>(mode),
                                    static_cast<unsigned char>(samplingInterval >> 8),
                                    static_cast<unsigned char>(samplingInterval & 0xff));

                int updatingInterval = m_endpointOptionsList[n].value("optUpdatingInterval").toInt();
                if (updatingInterval==0) updatingInterval = 50;

                // set encoder value range
                int range = m_endpointInfoList[n].range;
                m_rioc->encoderSetRangeLower(unit, channel, 0);
                m_rioc->encoderSetRangeUpper(unit, channel, range);

                // write initial value
                double initial = m_endpointInfoList[n].initial;
                if (initial >= 0) {
                    m_rioc->encoderWrite(unit, channel, round(initial*range));
                }

                // read value
                int value;
                if (m_rioc->encoderRead(unit, channel, &value)) {
                    this->onEncoderValueUpdated(unit, channel, value);
                }

                done = m_rioc->encoderSetNotification(unit, channel, true, updatingInterval);

            } else if (type==ENDPOINT_RIOC_RUDDER_OUT) {

                // set up rudder out channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                m_rioc->setupObject(unit, RO_MOTION_RUDDER, channel,
                                    static_cast<unsigned char>(mode));
                done = m_rioc->rudderSetEnable(unit, channel, false); // keep the servo unlocked

            } else if (type==ENDPOINT_RIOC_STEPPER_OUT) {

                // set up stepper out channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();

                // mode 0: 4 pins (A+ A- B+ B-)
                // mode 1: 2 pins (PUL+ and DIR+)
                // mode 2: 2 pins (PUL- and DIR-)
                int pins = (mode == 0 ? 4 : 2);
                unsigned char pin[4] = { 0, 0, 0, 0 };
                if (m_interfaceInfo.mode == INTERFACE_RIOC_ALADDIN ||
                    m_interfaceInfo.mode == INTERFACE_RIOC_PLC1 ||
                    m_interfaceInfo.mode == INTERFACE_RIOC_PLC2 ) {
                    // aladdin or plc pins
                    QList<int>doutPinList = DigishowEnvironment::getRiocPinList(m_interfaceInfo.mode, PIN_TYPE_DO);
                    for (int p=0 ; p<=(doutPinList.length()-pins) ; p++) {
                        if (doutPinList[p] == channel) {
                            for (int i=0 ; i<pins ; i++) pin[i] = doutPinList[p+i];
                            break;
                        }
                    }
                } else {
                    // common arduino pins
                    for (int i=0 ; i<pins ; i++) pin[i] = channel+i;
                }
                done = m_rioc->setupObject(unit, RO_MOTION_STEPPER, pin[0], pin[1], pin[2], pin[3],
                                           static_cast<unsigned char>(mode));

                // set speed (optional)
                int speed = m_endpointOptionsList[n].value("optSpeed").toInt();
                if (speed > 0) {
                    done = m_rioc->stepperSetSpeed(unit, channel, speed, true);
                }

                // set position (optional)
                int position = m_endpointOptionsList[n].value("optPosition").toInt();
                if (position > 0) {
                    done = m_rioc->stepperSetPosition(unit, channel, position, true);
                }

            } else if (type==ENDPOINT_RIOC_STEPPER_SET) {

                // no more setup for the stepper parameter settings
                done = true;

            } else if (type==ENDPOINT_RIOC_USER_CHANNEL) {

                // set up user channel
                int mode = m_endpointOptionsList[n].value("optMode").toInt();
                m_rioc->setupObject(unit, RO_USER_CHANNEL, channel,
                                    static_cast<unsigned char>(mode),
                                    0, 0, 0);
                // read value
                int value;
                if (m_rioc->userChannelRead(unit, channel, &value)) {
                    this->onUserChannelValueUpdated(unit, channel, value);
                }

                int updatingInterval = m_endpointOptionsList[n].value("optUpdatingInterval").toInt();
                if (updatingInterval==0) updatingInterval = 50;

                done = m_rioc->userChannelSetNotification(unit, channel, true, updatingInterval);
            }

            // set ready flag for this endpoint
            m_endpointInfoList[n].ready =
                    done ||
                    m_interfaceOptions.value("ignoreSetupError").toBool();

            // set initial value for this endpoint
            initEndpointValue(n);
        }
    }

    // enable silence mode
    m_rioc->silence(unit, true);
}

void DgsRiocInterface::onDigitalInValueUpdated(unsigned char unit, unsigned char channel, bool value)
{
    //qDebug() << "DgsRiocInterface::onDigitalInValueUpdated" << unit << channel << value;

    dgsSignalData data;
    data.signal = DATA_SIGNAL_BINARY;
    data.aRange = 0;
    data.aValue = 0;
    data.bValue = value;

    int endpointIndex = findEndpoint(unit, ENDPOINT_RIOC_DIGITAL_IN, channel);
    if (endpointIndex != -1) emit dataReceived(endpointIndex, data);
}

void DgsRiocInterface::onAnalogInValueUpdated(unsigned char unit, unsigned char channel, int value)
{
    //qDebug() << "DgsRiocInterface::onAnalogInValueUpdated" << unit << channel << value;

    dgsSignalData data;
    data.signal = DATA_SIGNAL_ANALOG;
    data.aRange = 0xffff;
    data.aValue = value;
    data.bValue = 0;

    int endpointIndex = findEndpoint(unit, ENDPOINT_RIOC_ANALOG_IN, channel);
    if (endpointIndex != -1) emit dataReceived(endpointIndex, data);
}

void DgsRiocInterface::onEncoderValueUpdated(unsigned char unit, unsigned char channel, int value)
{
    //qDebug() << "DgsRiocInterface::onEncoderValueUpdated" << unit << channel << value;

    int endpointIndex = findEndpoint(unit, ENDPOINT_RIOC_ENCODER_IN, channel);
    if (endpointIndex == -1) return;

    int range = m_endpointInfoList[endpointIndex].range;
    if (range == 0) range = 0xffff;

    if (value < 0) {
        value = 0;
        m_rioc->encoderWrite(unit, channel, 0);
    } else if (value > range) {
        value = range;
        m_rioc->encoderWrite(unit, channel, range);
    }

    dgsSignalData data;
    data.signal = DATA_SIGNAL_ANALOG;
    data.aRange = range;
    data.aValue = value;
    data.bValue = 0;

    emit dataReceived(endpointIndex, data);
}

void DgsRiocInterface::onUserChannelValueUpdated(unsigned char unit, unsigned char channel, int value)
{
    //qDebug() << "DgsRiocInterface::onUserCannelValueUpdated" << unit << channel << value;

    int endpointIndex = findEndpoint(unit, ENDPOINT_RIOC_USER_CHANNEL, channel);
    if (endpointIndex == -1) return;

    int range = m_endpointInfoList[endpointIndex].range;
    if (range == 0) range = 0xffff;

    if (value < 0) value = 0; else if (value > range) value = range;

    dgsSignalData data;
    data.signal = DATA_SIGNAL_ANALOG;
    data.aRange = range;
    data.aValue = value;
    data.bValue = 0;

    emit dataReceived(endpointIndex, data);
}

QVariantList DgsRiocInterface::listOnline()
{

    QVariantList list;
    QVariantMap info;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

#ifdef Q_OS_MAC
        if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif

        //if (serialPortInfo.hasVendorIdentifier()) {

            uint16_t vid = serialPortInfo.vendorIdentifier();
            uint16_t pid = serialPortInfo.productIdentifier();

            QString mode = guessRiocMode(vid, pid);
            if (mode.isEmpty()) mode = "general";

            info.clear();
            info["comPort"] = serialPortInfo.portName();
            info["mode"   ] = mode;
            list.append(info);
        //}
    }

    return list;
}

QString DgsRiocInterface::guessRiocMode(uint16_t vid, uint16_t pid)
{
    // see reference
    // https://github.com/arduino/ArduinoCore-avr/blob/1.8.2/boards.txt

    const uint16_t arduino_vid0 = 0x2341;
    const uint16_t arduino_vid1 = 0x2A03;

    const uint16_t uno_vid0=0x2341;
    const uint16_t uno_pid0=0x0043;
    const uint16_t uno_vid1=0x2341;
    const uint16_t uno_pid1=0x0001;
    const uint16_t uno_vid2=0x2A03;
    const uint16_t uno_pid2=0x0043;
    const uint16_t uno_vid3=0x2341;
    const uint16_t uno_pid3=0x0243;

    const uint16_t mega_vid0=0x2341;
    const uint16_t mega_pid0=0x0010;
    const uint16_t mega_vid1=0x2341;
    const uint16_t mega_pid1=0x0042;
    const uint16_t mega_vid2=0x2A03;
    const uint16_t mega_pid2=0x0010;
    const uint16_t mega_vid3=0x2A03;
    const uint16_t mega_pid3=0x0042;
    const uint16_t mega_vid4=0x2341;
    const uint16_t mega_pid4=0x0210;
    const uint16_t mega_vid5=0x2341;
    const uint16_t mega_pid5=0x0242;

    if (vid==arduino_vid0 || vid==arduino_vid1) {

        if ((vid==uno_vid0 && pid==uno_pid0) ||
            (vid==uno_vid1 && pid==uno_pid1) ||
            (vid==uno_vid2 && pid==uno_pid2) ||
            (vid==uno_vid3 && pid==uno_pid3)) {

            return "arduino_uno";

        } else if (
            (vid==mega_vid0 && pid==mega_pid0) ||
            (vid==mega_vid1 && pid==mega_pid1) ||
            (vid==mega_vid2 && pid==mega_pid2) ||
            (vid==mega_vid3 && pid==mega_pid3) ||
            (vid==mega_vid4 && pid==mega_pid4) ||
            (vid==mega_vid5 && pid==mega_pid5)) {

            return "arduino_mega";

        }

        return "general";
    }

    return "";
}

void DgsRiocInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_RIOC;

    // Set interface mode
    QString modeName = m_interfaceOptions.value("mode").toString();
    if      (modeName == "general"     ) m_interfaceInfo.mode = INTERFACE_RIOC_GENERAL;
    else if (modeName == "arduino_uno" ) m_interfaceInfo.mode = INTERFACE_RIOC_ARDUINO_UNO;
    else if (modeName == "arduino_mega") m_interfaceInfo.mode = INTERFACE_RIOC_ARDUINO_MEGA;
    else if (modeName == "aladdin"     ) m_interfaceInfo.mode = INTERFACE_RIOC_ALADDIN;
    else if (modeName == "plc1"        ) m_interfaceInfo.mode = INTERFACE_RIOC_PLC1;
    else if (modeName == "plc2"        ) m_interfaceInfo.mode = INTERFACE_RIOC_PLC2;

    // Set interface input/output flags
    m_interfaceInfo.input = true;
    m_interfaceInfo.output = true;

    // Set interface label
    switch (m_interfaceInfo.mode) {
        case INTERFACE_RIOC_ALADDIN:
            m_interfaceInfo.label = tr("Aladdin");
            break;
        case INTERFACE_RIOC_PLC1:
        case INTERFACE_RIOC_PLC2:
            m_interfaceInfo.label = tr("Arduino PLC");
            break;
        default:
            m_interfaceInfo.label = tr("Arduino");
            break;
    }
    m_interfaceInfo.label += " " + m_interfaceOptions.value("comPort").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "digital_in" ) endpointInfo.type = ENDPOINT_RIOC_DIGITAL_IN;
        else if (typeName == "digital_out") endpointInfo.type = ENDPOINT_RIOC_DIGITAL_OUT;
        else if (typeName == "analog_in"  ) endpointInfo.type = ENDPOINT_RIOC_ANALOG_IN;
        else if (typeName == "analog_out" ) endpointInfo.type = ENDPOINT_RIOC_ANALOG_OUT;
        else if (typeName == "pwm_out"    ) endpointInfo.type = ENDPOINT_RIOC_PWM_OUT;
        else if (typeName == "pfm_out"    ) endpointInfo.type = ENDPOINT_RIOC_PFM_OUT;
        else if (typeName == "encoder_in" ) endpointInfo.type = ENDPOINT_RIOC_ENCODER_IN;
        else if (typeName == "rudder_out" ) endpointInfo.type = ENDPOINT_RIOC_RUDDER_OUT;
        else if (typeName == "stepper_out") endpointInfo.type = ENDPOINT_RIOC_STEPPER_OUT;
        else if (typeName == "stepper_set") endpointInfo.type = ENDPOINT_RIOC_STEPPER_SET;
        else if (typeName == "user_channel")endpointInfo.type = ENDPOINT_RIOC_USER_CHANNEL;

        // Set common label format for most RIOC endpoints
        QString pinName = DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel);
        endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(pinName);

        // Set endpoint properties based on type
        switch (endpointInfo.type) {
            case ENDPOINT_RIOC_DIGITAL_IN:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.input  = true;
                endpointInfo.labelEPT = tr("Digital In");
                break;
            case ENDPOINT_RIOC_DIGITAL_OUT:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = true;
                endpointInfo.labelEPT = tr("Digital Out");
                break;
            case ENDPOINT_RIOC_ANALOG_IN:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.input  = true;
                endpointInfo.range  = 65535;
                endpointInfo.labelEPT = tr("Analog In");
                break;
            case ENDPOINT_RIOC_ANALOG_OUT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = 65535;
                endpointInfo.labelEPT = tr("Analog Out");
                break;
            case ENDPOINT_RIOC_PWM_OUT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = 255;
                endpointInfo.labelEPT = tr("PWM Out");
                break;
            case ENDPOINT_RIOC_PFM_OUT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
                endpointInfo.labelEPT = tr("Freq Out");
                break;
            case ENDPOINT_RIOC_ENCODER_IN:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.input  = true;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
                endpointInfo.labelEPT = tr("Encoder");
                endpointInfo.labelEPI += " +";
                break;
            case ENDPOINT_RIOC_RUDDER_OUT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = 180;
                endpointInfo.labelEPT = tr("Servo");
                break;
            case ENDPOINT_RIOC_STEPPER_OUT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
                endpointInfo.labelEPT = tr("Stepper");
                endpointInfo.labelEPI += " +";
                break;
            case ENDPOINT_RIOC_STEPPER_SET:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
                endpointInfo.labelEPT = DigishowEnvironment::getMotionControlName(endpointInfo.control);
                endpointInfo.labelEPI += " +";
                break;
            case ENDPOINT_RIOC_USER_CHANNEL:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.input  = true;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000000);
                endpointInfo.labelEPT = tr("Channel");
                endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}

