#include "dgs_midi_interface.h"
#include "RtMidi.h"

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
        if (endpointIndex != -1) emit dataReceived(endpointIndex, data);

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
        if (endpointIndex != -1) emit dataReceived(endpointIndex, data);
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
