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

#include "dgs_midi_interface.h"
#include "digishow_environment.h"
#include "rtmidi/RtMidi.h"

DgsMidiInterface::DgsMidiInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "midi";
    m_midiin = nullptr;
    m_midiout = nullptr;
}

DgsMidiInterface::~DgsMidiInterface()
{
    closeInterface();
}

int DgsMidiInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get parameters to open midi interface
    bool isMidiOutput = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
    bool isMidiInput  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
    if (!isMidiOutput && !isMidiInput) return ERR_INVALID_OPTION;

    QString midiPortName = m_interfaceOptions.value("port").toString();
    if (midiPortName.isEmpty()) return ERR_INVALID_OPTION;

    // try to find and open the midi port
    bool done = false;
    try {

        // for midi output port
        if (isMidiOutput) {
            m_midiout = new RtMidiOut();
            unsigned int nPorts = m_midiout->getPortCount();

            for (unsigned int n=0 ; n<nPorts ; n++) {
                if (getUniqueMidiPortName(m_midiout, n) == midiPortName) {
                    m_midiout->openPort(n);
                    done = true;
                    break;
                }
            }
        }

        // for midi input port
        if (isMidiInput) {
            m_midiin = new RtMidiIn();
            unsigned int nPorts = m_midiin->getPortCount();

            for (unsigned int n=0 ; n<nPorts ; n++) {
                if (getUniqueMidiPortName(m_midiin, n) == midiPortName) {
                    m_midiin->openPort(n);
                    done = true;
                    break;
                }
            }

            if (done) {

                // set callback for receving incoming messages
                m_midiin->setCallback(&_callbackRtMidiIn, this);

                // ignore sysex, timing, or active sensing messages.
                m_midiin->ignoreTypes(true, true, true);
            }
        }

    } catch ( RtMidiError &error ) {
        Q_UNUSED(error)
        //error.printMessage();
        done = false;
    }

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsMidiInterface::closeInterface()
{    
    if (m_midiin != nullptr) {
        delete m_midiin;
        m_midiin = nullptr;
    }

    if (m_midiout != nullptr) {
        delete m_midiout;
        m_midiout = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsMidiInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // confirm midi out interface is opened
    if (m_midiout == nullptr) return ERR_DEVICE_NOT_READY;

    // confirm midi channel number is valid
    int channel = m_endpointInfoList[endpointIndex].channel;
    if (channel<0 || channel>15) return ERR_INVALID_OPTION;

    // send note, control or program message
    int type = m_endpointInfoList[endpointIndex].type;
    if (type == ENDPOINT_MIDI_NOTE) {

        // send note message

        int note = m_endpointInfoList[endpointIndex].note;
        if (note<0 || note>127) return ERR_INVALID_OPTION;

        if (data.signal != DATA_SIGNAL_NOTE) return ERR_INVALID_DATA;

        int velocity = 127 * data.aValue / (data.aRange==0 ? 127 : data.aRange);
        if (velocity<0 || velocity>127) return ERR_INVALID_DATA;

        // call rtmidi to send midi message
        std::vector<unsigned char> message;
        message.push_back((data.bValue ? 0x90 : 0x80) +          // note on/off
                          static_cast<unsigned char>(channel));  // channel number
        message.push_back(static_cast<unsigned char>(note));     // note number
        message.push_back(static_cast<unsigned char>(velocity)); // note velocity

        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_note:" << message[0] << message[1] << message[2];

    } else if (type == ENDPOINT_MIDI_CONTROL) {

        // send control message

        int control = m_endpointInfoList[endpointIndex].control;
        if (control<0 || control>127) return ERR_INVALID_OPTION;

        int value = 127 * data.aValue / (data.aRange==0 ? 127 : data.aRange);
        if (value<0 || value>127) return ERR_INVALID_DATA;

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        // call rtmidi to send midi message
        std::vector<unsigned char> message;
        message.push_back(0xB0 +                                 // control change
                          static_cast<unsigned char>(channel));  // channel number
        message.push_back(static_cast<unsigned char>(control));  // control number
        message.push_back(static_cast<unsigned char>(value));    // control value

        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_control:" << message[0] << message[1] << message[2];

    } else if (type == ENDPOINT_MIDI_PROGRAM) {

        // send program message
        int value = 127 * data.aValue / (data.aRange==0 ? 127 : data.aRange);
        if (value<0 || value>127) return ERR_INVALID_DATA;

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        // call rtmidi to send midi message
        std::vector<unsigned char> message;
        message.push_back(0xC0 +                                 // program change
                          static_cast<unsigned char>(channel));  // channel number
        message.push_back(static_cast<unsigned char>(value));    // program value

        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_program:" << message[0] << message[1];

    } else if (type == ENDPOINT_MIDI_PITCH) {

        // send pitch message
        int value = 16383 * data.aValue / (data.aRange==0 ? 16383 : data.aRange);
        if (value<0 || value>16383) return ERR_INVALID_DATA;

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        // call rtmidi to send midi message
        std::vector<unsigned char> message;
        message.push_back(0xE0 +                                   // pitch bend
                          static_cast<unsigned char>(channel));    // channel number
        message.push_back(static_cast<unsigned char>(value&0x7F)); // value lsb (7 bits)
        message.push_back(static_cast<unsigned char>(value>>7));   // value msb (7 bits)

        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_program:" << message[0] << message[1] << message[2];

    } else if (type == ENDPOINT_MIDI_CC_PULSE) {

        // send cc pulse messages
        int value = 127 * data.aValue / (data.aRange==0 ? 127 : data.aRange);
        if (value<0 || value>127) return ERR_INVALID_DATA;

        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        // call rtmidi to send midi message
        std::vector<unsigned char> message;
        message.push_back(0xB0 +                                 // control change
                          static_cast<unsigned char>(channel));  // channel number
        message.push_back(static_cast<unsigned char>(value));    // control number
        message.push_back(127);
        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_control:" << message[0] << message[1] << message[2];

        message.clear();
        message.push_back(0xB0 +                                 // control change
                          static_cast<unsigned char>(channel));  // channel number
        message.push_back(static_cast<unsigned char>(value));    // control number
        message.push_back(0);
        m_midiout->sendMessage(&message);
        //qDebug() << "midiout_control:" << message[0] << message[1] << message[2];

    } else return ERR_INVALID_OPTION;

    return ERR_NONE;
}

int DgsMidiInterface::findEndpointWidthMidiNote(int channel, int note)
{
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_MIDI_NOTE &&
            m_endpointInfoList[n].channel == channel &&
            m_endpointInfoList[n].note == note) return n;
    }
    return -1;
}

int DgsMidiInterface::findEndpointWidthMidiControl(int channel, int control)
{
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_MIDI_CONTROL &&
            m_endpointInfoList[n].channel == channel &&
            m_endpointInfoList[n].control == control) return n;
    }
    return -1;
}

int DgsMidiInterface::findEndpointWidthMidiProgram(int channel)
{
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_MIDI_PROGRAM &&
            m_endpointInfoList[n].channel == channel) return n;
    }
    return -1;
}

int DgsMidiInterface::findEndpointWidthMidiPitch(int channel)
{
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_MIDI_PITCH &&
            m_endpointInfoList[n].channel == channel) return n;
    }
    return -1;
}

void DgsMidiInterface::callbackRtMidiIn(double deltatime, std::vector< unsigned char > *message)
{
    Q_UNUSED(deltatime)

    if (message->size()==0) return;

    // get midi message type and channel
    unsigned char byte0 = message->at(0);
    int msgtype = byte0 & 0xf0;
    int channel = byte0 & 0x0f;

    if (msgtype == 0x90 || msgtype == 0x80) {

        // note on/off
        if (message->size()!=3) return;
        int note = message->at(1);
        int velocity = message->at(2);

        dgsSignalData data;
        data.signal = DATA_SIGNAL_NOTE;
        data.aRange = 127;
        data.aValue = velocity;
        data.bValue = (msgtype == 0x90); // note on/off

        int endpointIndex = findEndpointWidthMidiNote(channel, note);

        //qDebug() << "midiin_note:" << endpointIndex << data.signal << data.aValue << data.bValue;
        if (endpointIndex != -1  && m_endpointInfoList[endpointIndex].enabled) emit dataReceived(endpointIndex, data);

        if (m_needReceiveRawData) {
            QVariantMap rawData;
            rawData["event"   ] = (msgtype == 0x90 ? "note_on" : "note_off");
            rawData["channel" ] = channel;
            rawData["note"    ] = note;
            rawData["velocity"] = velocity;
            emit rawDataReceived(rawData);
        }

    } else if (msgtype == 0xB0) {

        // control change
        if (message->size()!=3) return;
        int control = message->at(1);
        int value = message->at(2);

        dgsSignalData data;
        data.signal = DATA_SIGNAL_ANALOG;
        data.aRange = 127;
        data.aValue = value;
        data.bValue = 0;

        int endpointIndex = findEndpointWidthMidiControl(channel, control);

        //qDebug() << "midiin_control:" << endpointIndex << data.signal << data.aValue << data.bValue;
        if (endpointIndex != -1 && m_endpointInfoList[endpointIndex].enabled) emit dataReceived(endpointIndex, data);

        if (m_needReceiveRawData) {
            QVariantMap rawData;
            rawData["event"  ] = "control_change";
            rawData["channel"] = channel;
            rawData["control"] = control;
            rawData["value"  ] = value;
            emit rawDataReceived(rawData);
        }

    } else if (msgtype == 0xC0) {

        // program change
        if (message->size()!=2) return;
        int value = message->at(1);

        dgsSignalData data;
        data.signal = DATA_SIGNAL_ANALOG;
        data.aRange = 127;
        data.aValue = value;
        data.bValue = 0;

        int endpointIndex = findEndpointWidthMidiProgram(channel);

        //qDebug() << "midiin_program:" << endpointIndex << data.signal << data.aValue << data.bValue;
        if (endpointIndex != -1 && m_endpointInfoList[endpointIndex].enabled) emit dataReceived(endpointIndex, data);

        if (m_needReceiveRawData) {
            QVariantMap rawData;
            rawData["event"  ] = "program_change";
            rawData["channel"] = channel;
            rawData["value"  ] = value;
            emit rawDataReceived(rawData);
        }

    } else if (msgtype == 0xE0) {

        // pitch bend
        if (message->size()!=3) return;
        int lsb = message->at(1); // 0 ~ 0x7F
        int msb = message->at(2); // 0 ~ 0x7F
        int value = (msb<<7) | lsb; // 14bit value = 7bit msb + 7bit lsb

        dgsSignalData data;
        data.signal = DATA_SIGNAL_ANALOG;
        data.aRange = 16383; // 14 bits
        data.aValue = value;
        data.bValue = 0;

        int endpointIndex = findEndpointWidthMidiPitch(channel);

        //qDebug() << "midiin_pitch:" << endpointIndex << data.signal << data.aValue << data.bValue;
        if (endpointIndex != -1 && m_endpointInfoList[endpointIndex].enabled) emit dataReceived(endpointIndex, data);

        if (m_needReceiveRawData) {
            QVariantMap rawData;
            rawData["event"  ] = "pitch_bend";
            rawData["channel"] = channel;
            rawData["value"  ] = value;
            emit rawDataReceived(rawData);
        }

    }
}

void DgsMidiInterface::_callbackRtMidiIn(double deltatime, std::vector< unsigned char > *message, void *userData)
{
    static_cast<DgsMidiInterface*>(userData)->callbackRtMidiIn(deltatime, message);
}

QVariantList DgsMidiInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    RtMidiIn  *midiIn  = nullptr;
    RtMidiOut *midiOut = nullptr;

    try {

        // list midi in ports
        midiIn  = new RtMidiIn();
        unsigned int midiInPortCount = midiIn->getPortCount();
        for (unsigned int n=0 ; n<midiInPortCount ; n++) {
            info.clear();
            info["port"] = getUniqueMidiPortName(midiIn, n);
            info["mode"] = "input";
            list.append(info);
        }

        // list midi out ports
        midiOut = new RtMidiOut();
        unsigned int midiOutPortCount = midiOut->getPortCount();
        for (unsigned int n=0 ; n<midiOutPortCount ; n++) {
            info.clear();
            info["port"] = getUniqueMidiPortName(midiOut, n);
            info["mode"] = "output";
            list.append(info);
        }

    } catch ( RtMidiError &error ) {
        Q_UNUSED(error)
        //error.printMessage();
    }

    if (midiIn  != nullptr) delete midiIn;
    if (midiOut != nullptr) delete midiOut;

    return list;
}

QString DgsMidiInterface::getUniqueMidiPortName(RtMidi *midi, unsigned int index)
{
    QString portName;
    unsigned int portCount = midi->getPortCount();
    unsigned int i = 0;

    if (index < portCount) {

        portName = QString::fromStdString(midi->getPortName(index));

        for (unsigned int n=0 ; n<index ; n++)
            if (QString::fromStdString(midi->getPortName(n)) == portName) i++;
    }

    if (i>0) return portName + " #" + QString::number(i+1);
    return portName;
}

void DgsMidiInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_MIDI;

    // Set interface mode
    QString modeName = m_interfaceOptions.value("mode").toString();
    if      (modeName == "input" ) m_interfaceInfo.mode = INTERFACE_MIDI_INPUT;
    else if (modeName == "output") m_interfaceInfo.mode = INTERFACE_MIDI_OUTPUT;

    // Set interface input/output flags
    m_interfaceInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
    m_interfaceInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);

    // Set interface label
    m_interfaceInfo.label = tr("MIDI") + " " + m_interfaceOptions.value("port").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "note"    ) endpointInfo.type = ENDPOINT_MIDI_NOTE;
        else if (typeName == "control" ) endpointInfo.type = ENDPOINT_MIDI_CONTROL;
        else if (typeName == "program" ) endpointInfo.type = ENDPOINT_MIDI_PROGRAM;
        else if (typeName == "pitch"   ) endpointInfo.type = ENDPOINT_MIDI_PITCH;
        else if (typeName == "cc_pulse") endpointInfo.type = ENDPOINT_MIDI_CC_PULSE;

        // Set endpoint properties based on type
        switch (endpointInfo.type) {
            case ENDPOINT_MIDI_NOTE:
                endpointInfo.signal = DATA_SIGNAL_NOTE;
                endpointInfo.range  = 127;
                endpointInfo.labelEPT = tr("MIDI Note");
                endpointInfo.labelEPI = QString("Ch%1 : %2").arg(endpointInfo.channel+1)
                                      .arg(DigishowEnvironment::getMidiNoteName(endpointInfo.note));
                break;
            case ENDPOINT_MIDI_CONTROL:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 127;
                endpointInfo.labelEPT = tr("MIDI CC");
                endpointInfo.labelEPI = QString("Ch%1 : %2").arg(endpointInfo.channel+1)
                                      .arg(endpointInfo.control);
                break;
            case ENDPOINT_MIDI_PROGRAM:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 127;
                endpointInfo.labelEPT = tr("MIDI Prgm");
                endpointInfo.labelEPI = QString("Ch%1").arg(endpointInfo.channel+1);
                break;
            case ENDPOINT_MIDI_PITCH:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 16383;
                endpointInfo.labelEPT = tr("MIDI Pitch");
                endpointInfo.labelEPI = QString("Ch%1").arg(endpointInfo.channel+1);
                break;
            case ENDPOINT_MIDI_CC_PULSE:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = true;
                endpointInfo.range  = 127;
                endpointInfo.labelEPT = tr("MIDI CC");
                endpointInfo.labelEPI = QString("Ch%1").arg(endpointInfo.channel+1);
                break;
        }

        // Set endpoint input/output flags
        if (endpointInfo.type != ENDPOINT_MIDI_CC_PULSE) {
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
        }

        m_endpointInfoList.append(endpointInfo);
    }
}
