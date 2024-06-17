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

#include "digishow_interface.h"
#include "digishow_environment.h"

DigishowInterface::DigishowInterface(QObject *parent) : QObject(parent)
{
    m_isInterfaceOpened = false;
    m_needReceiveRawData = false;
    m_interfaceInfo = dgsInterfaceInfo();

#ifdef QT_DEBUG
    qDebug() << "interface created";
#endif
}

DigishowInterface::~DigishowInterface()
{

#ifdef QT_DEBUG
    qDebug() << "interface released";
#endif
}

int DigishowInterface::setInterfaceOptions(const QVariantMap &options)
{
    m_interfaceOptions = options;
    return ERR_NONE;
}

int DigishowInterface::setInterfaceOption(const QString &name, const QVariant &value)
{
    if (value.isNull())
        m_interfaceOptions.remove(name);
    else
        m_interfaceOptions[name] = value;

    return ERR_NONE;
}

QVariantMap DigishowInterface::getInterfaceInfo()
{
    QVariantMap info;

    info["type"  ] = m_interfaceInfo.type;
    info["mode"  ] = m_interfaceInfo.mode;
    info["output"] = m_interfaceInfo.output;
    info["input" ] = m_interfaceInfo.input;
    info["label" ] = m_interfaceInfo.label;

    return info;
}

int DigishowInterface::addEndpoint(const QVariantMap &options, int *endpointIndex)
{
    m_endpointOptionsList.append(options);
    if (endpointIndex != nullptr) *endpointIndex = m_endpointOptionsList.length()-1;
    return ERR_NONE;
}

int DigishowInterface::setEndpointOptions(int endpointIndex, const QVariantMap &options)
{
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    m_endpointOptionsList[endpointIndex] = options;
    return ERR_NONE;
}

int DigishowInterface::setEndpointOption(int endpointIndex, const QString &name, const QVariant &value)
{
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    m_endpointOptionsList[endpointIndex][name] = value;
    return ERR_NONE;
}

int DigishowInterface::endpointCount()
{
    return m_endpointOptionsList.length();
}

QVariantMap DigishowInterface::getEndpointOptionsAt(int index)
{
    if (index>=0 && index<endpointCount()) return m_endpointOptionsList[index];
    return QVariantMap();
}

QVariantMap DigishowInterface::getEndpointInfoAt(int index)
{
    QVariantMap info;
    if (index>=0 && index<endpointCount()) {

        info["type"    ] = m_endpointInfoList[index].type;
        info["unit"    ] = m_endpointInfoList[index].unit;
        info["channel" ] = m_endpointInfoList[index].channel;
        info["note"    ] = m_endpointInfoList[index].note;
        info["control" ] = m_endpointInfoList[index].control;
        info["signal"  ] = m_endpointInfoList[index].signal;
        info["output"  ] = m_endpointInfoList[index].output;
        info["input"   ] = m_endpointInfoList[index].input;
        info["range"   ] = m_endpointInfoList[index].range;
        info["ready"   ] = m_endpointInfoList[index].ready;
        info["initial" ] = m_endpointInfoList[index].initial;

        info["address" ] = m_endpointInfoList[index].address;

        info["labelEPT"] = m_endpointInfoList[index].labelEPT;
        info["labelEPI"] = m_endpointInfoList[index].labelEPI;
    }

    return info;
}

int DigishowInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;
    updateMetadata();
    m_needReceiveRawData = false;
    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DigishowInterface::closeInterface()
{
    m_needReceiveRawData = false;
    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DigishowInterface::init()
{
    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;

    // set initial value for all endpoints
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) initEndpointValue(n);
    return ERR_NONE;
}

int DigishowInterface::sendData(int endpointIndex, dgsSignalData data)
{
    Q_UNUSED(data)

    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    if (!m_endpointInfoList[endpointIndex].enabled) return ERR_DEVICE_NOT_READY;

    emit dataSent(endpointIndex, data);

    return ERR_NONE;
}

int DigishowInterface::loadMedia(const QVariantMap &mediaOptions)
{
    Q_UNUSED(mediaOptions)

    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;

    return ERR_NONE;
}

void DigishowInterface::updateMetadata()
{
    // reset metadata first
    m_interfaceInfo = dgsInterfaceInfo();
    m_endpointInfoList.clear();

    // set interface type
    QString typeName = m_interfaceOptions.value("type").toString();
    if      (typeName == "midi"     ) m_interfaceInfo.type = INTERFACE_MIDI;
    else if (typeName == "rioc"     ) m_interfaceInfo.type = INTERFACE_RIOC;
    else if (typeName == "modbus"   ) m_interfaceInfo.type = INTERFACE_MODBUS;
    else if (typeName == "hue"      ) m_interfaceInfo.type = INTERFACE_HUE;
    else if (typeName == "dmx"      ) m_interfaceInfo.type = INTERFACE_DMX;
    else if (typeName == "artnet"   ) m_interfaceInfo.type = INTERFACE_ARTNET;
    else if (typeName == "osc"      ) m_interfaceInfo.type = INTERFACE_OSC;
    else if (typeName == "audioin"  ) m_interfaceInfo.type = INTERFACE_AUDIOIN;
    else if (typeName == "screen"   ) m_interfaceInfo.type = INTERFACE_SCREEN;
    else if (typeName == "aplay"    ) m_interfaceInfo.type = INTERFACE_APLAY;
    else if (typeName == "mplay"    ) m_interfaceInfo.type = INTERFACE_MPLAY;
    else if (typeName == "pipe"     ) m_interfaceInfo.type = INTERFACE_PIPE;
    else if (typeName == "launch"   ) m_interfaceInfo.type = INTERFACE_LAUNCH;
    else if (typeName == "hotkey"   ) m_interfaceInfo.type = INTERFACE_HOTKEY;
    else if (typeName == "metronome") m_interfaceInfo.type = INTERFACE_METRONOME;

    // set interface mode
    QString modeName = m_interfaceOptions.value("mode").toString();
    switch (m_interfaceInfo.type) {
    case INTERFACE_MIDI:
        if      (modeName == "input"       ) m_interfaceInfo.mode = INTERFACE_MIDI_INPUT;
        else if (modeName == "output"      ) m_interfaceInfo.mode = INTERFACE_MIDI_OUTPUT;
        break;
    case INTERFACE_RIOC:
        if      (modeName == "general"     ) m_interfaceInfo.mode = INTERFACE_RIOC_GENERAL;
        else if (modeName == "arduino_uno" ) m_interfaceInfo.mode = INTERFACE_RIOC_ARDUINO_UNO;
        else if (modeName == "arduino_mega") m_interfaceInfo.mode = INTERFACE_RIOC_ARDUINO_MEGA;
        else if (modeName == "aladdin"     ) m_interfaceInfo.mode = INTERFACE_RIOC_ALADDIN;
        else if (modeName == "plc1"        ) m_interfaceInfo.mode = INTERFACE_RIOC_PLC1;
        else if (modeName == "plc2"        ) m_interfaceInfo.mode = INTERFACE_RIOC_PLC2;
        break;
    case INTERFACE_MODBUS:
        if      (modeName == "rtu"         ) m_interfaceInfo.mode = INTERFACE_MODBUS_RTU;
        else if (modeName == "tcp"         ) m_interfaceInfo.mode = INTERFACE_MODBUS_TCP;
        else if (modeName == "rtuovertcp"  ) m_interfaceInfo.mode = INTERFACE_MODBUS_RTUOVERTCP;
        break;
    case INTERFACE_HUE:                      m_interfaceInfo.mode = INTERFACE_HUE_DEFAULT;
        break;
    case INTERFACE_DMX:
        if      (modeName == "enttec"      ) m_interfaceInfo.mode = INTERFACE_DMX_ENTTEC_PRO;
        else if (modeName == "opendmx"     ) m_interfaceInfo.mode = INTERFACE_DMX_ENTTEC_OPEN;
        break;
    case INTERFACE_ARTNET:
        if      (modeName == "input"       ) m_interfaceInfo.mode = INTERFACE_ARTNET_INPUT;
        else if (modeName == "output"      ) m_interfaceInfo.mode = INTERFACE_ARTNET_OUTPUT;
        break;
    case INTERFACE_OSC:
        if      (modeName == "input"       ) m_interfaceInfo.mode = INTERFACE_OSC_INPUT;
        else if (modeName == "output"      ) m_interfaceInfo.mode = INTERFACE_OSC_OUTPUT;
        break;
    case INTERFACE_AUDIOIN:                  m_interfaceInfo.mode = INTERFACE_AUDIOIN_DEFAULT;
        break;
    case INTERFACE_SCREEN:                   m_interfaceInfo.mode = INTERFACE_SCREEN_DEFAULT;
        break;
    case INTERFACE_APLAY:                    m_interfaceInfo.mode = INTERFACE_APLAY_DEFAULT;
        break;
    case INTERFACE_MPLAY:                    m_interfaceInfo.mode = INTERFACE_MPLAY_DEFAULT;
        break;
    case INTERFACE_PIPE:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_PIPE_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_PIPE_REMOTE;
        else if (modeName == "cloud"       ) m_interfaceInfo.mode = INTERFACE_PIPE_CLOUD;
        break;
    case INTERFACE_LAUNCH:                   m_interfaceInfo.mode = INTERFACE_LAUNCH_DEFAULT;
        break;
    case INTERFACE_HOTKEY:                   m_interfaceInfo.mode = INTERFACE_HOTKEY_DEFAULT;
        break;
    case INTERFACE_METRONOME:                m_interfaceInfo.mode = INTERFACE_METRONOME_DEFAULT;
        break;
    }

    // set interface input flag
    if (m_interfaceInfo.mode==0)
        m_interfaceInfo.input = false;
    else if (m_interfaceInfo.mode==INTERFACE_MIDI_OUTPUT ||
             m_interfaceInfo.mode==INTERFACE_ARTNET_OUTPUT ||
             m_interfaceInfo.mode==INTERFACE_OSC_OUTPUT ||
             m_interfaceInfo.type==INTERFACE_HUE ||
             m_interfaceInfo.type==INTERFACE_DMX ||
             m_interfaceInfo.type==INTERFACE_SCREEN ||
             m_interfaceInfo.type==INTERFACE_APLAY ||
             m_interfaceInfo.type==INTERFACE_MPLAY ||
             m_interfaceInfo.type==INTERFACE_LAUNCH )
        m_interfaceInfo.input = false;
    else
        m_interfaceInfo.input = true;

    // set interface output flag
    if (m_interfaceInfo.mode==0)
        m_interfaceInfo.output = false;
    else if (m_interfaceInfo.mode==INTERFACE_MIDI_INPUT ||
             m_interfaceInfo.mode==INTERFACE_ARTNET_INPUT ||
             m_interfaceInfo.mode==INTERFACE_OSC_INPUT ||
             m_interfaceInfo.type==INTERFACE_AUDIOIN ||
             m_interfaceInfo.type==INTERFACE_HOTKEY )
        m_interfaceInfo.output = false;
    else
        m_interfaceInfo.output = true;

    // set interface label
    QString labelType = "";
    QString labelIdentity = "";
    switch (m_interfaceInfo.type) {
    case INTERFACE_MIDI:
        labelType = tr("MIDI");
        labelIdentity = m_interfaceOptions.value("port").toString();
        break;
    case INTERFACE_RIOC:
        switch (m_interfaceInfo.mode) {
        case INTERFACE_RIOC_ALADDIN:
            labelType = tr("Aladdin"); break;
        case INTERFACE_RIOC_PLC1:
        case INTERFACE_RIOC_PLC2:
            labelType = tr("Arduino PLC"); break;
        default:
            labelType = tr("Arduino"); break;
        }
        labelIdentity = m_interfaceOptions.value("comPort").toString();
        break;
    case INTERFACE_MODBUS:
        labelType = tr("Modbus");
        labelIdentity = (m_interfaceInfo.mode==INTERFACE_MODBUS_RTU ?
                         m_interfaceOptions.value("comPort").toString() :
                         m_interfaceOptions.value("tcpHost").toString() + ":" +
                         m_interfaceOptions.value("tcpPort").toString());
        break;
    case INTERFACE_HUE:
        labelType = tr("Hue");
        labelIdentity = m_interfaceOptions.value("serial").toString();
        break;
    case INTERFACE_DMX:
        labelType = tr("DMX");
        labelIdentity = m_interfaceOptions.value("comPort").toString();
        break;
    case INTERFACE_ARTNET:
        labelType = tr("ArtNet");
        labelIdentity = m_interfaceOptions.value("udpHost").toString() + ":" +
                        m_interfaceOptions.value("udpPort").toString();
        break;
    case INTERFACE_OSC:
        labelType = tr("OSC");
        labelIdentity = m_interfaceOptions.value("udpHost").toString() + ":" +
                        m_interfaceOptions.value("udpPort").toString();
        break;
    case INTERFACE_AUDIOIN:
        labelType = tr("Audio In");
        labelIdentity = m_interfaceOptions.value("port").toString();;
        break;
    case INTERFACE_SCREEN:
        labelType = tr("Screen");
        switch (int n = m_interfaceOptions.value("screen").toInt()) {
        case -1: labelIdentity = tr("(Preview Window)"); break;
        case  0: labelIdentity = ""; break;
        default: labelIdentity = QString::number(n);
        }
        break;
    case INTERFACE_APLAY:
        labelType = tr("Audio Player");
        labelIdentity = m_interfaceOptions.value("port").toString();
        break;
    case INTERFACE_MPLAY:
        labelType = tr("MIDI Player");
        labelIdentity = m_interfaceOptions.value("port").toString();
        break;
    case INTERFACE_PIPE:
        if (m_interfaceInfo.mode==INTERFACE_PIPE_REMOTE) {
            labelType = tr("Remote Pipe");
            labelIdentity = m_interfaceOptions.value("tcpHost").toString() + ":" +
                            m_interfaceOptions.value("tcpPort").toString();
        } else if (m_interfaceInfo.mode==INTERFACE_PIPE_CLOUD) {
            labelType = tr("Cloud Pipe");
            labelIdentity = m_interfaceOptions.value("pipeId").toString();
        } else {
            labelType = tr("Virtual Pipe");
            labelIdentity = m_interfaceOptions.value("comment").toString();
        }
        break;
    case INTERFACE_LAUNCH:
        labelType = tr("Preset Launcher");
        labelIdentity = "";
        break;
    case INTERFACE_HOTKEY:
        labelType = tr("Hot Key");
        labelIdentity = "";
        break;
    case INTERFACE_METRONOME:
        labelType = tr("Beat Maker");
        labelIdentity = "";
        break;
    }
    m_interfaceInfo.label = labelType + (!labelIdentity.isEmpty() ? " " + labelIdentity : "");

    // set metadata for all endpoints
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {

        dgsEndpointInfo endpointInfo;
        endpointInfo.enabled = m_endpointOptionsList[n].value("enabled", true).toBool();
        endpointInfo.type    = 0;
        endpointInfo.unit    = m_endpointOptionsList[n].value("unit"   ).toInt();
        endpointInfo.channel = m_endpointOptionsList[n].value("channel").toInt();
        endpointInfo.note    = m_endpointOptionsList[n].value("note"   ).toInt();
        endpointInfo.control = m_endpointOptionsList[n].value("control").toInt();
        endpointInfo.signal  = 0;
        endpointInfo.output  = false;
        endpointInfo.input   = false;
        endpointInfo.range   = m_endpointOptionsList[n].value("range"  ).toInt();
        endpointInfo.initial = m_endpointOptionsList[n].value("initial", -1).toDouble();
        endpointInfo.address = m_endpointOptionsList[n].value("address").toString();

        // set endpoint type
        typeName = m_endpointOptionsList[n].value("type").toString();
        switch (m_interfaceInfo.type) {
        case INTERFACE_MIDI:
            if      (typeName == "note"       ) endpointInfo.type = ENDPOINT_MIDI_NOTE;
            else if (typeName == "control"    ) endpointInfo.type = ENDPOINT_MIDI_CONTROL;
            else if (typeName == "program"    ) endpointInfo.type = ENDPOINT_MIDI_PROGRAM;
            else if (typeName == "pitch"      ) endpointInfo.type = ENDPOINT_MIDI_PITCH;
            else if (typeName == "cc_pulse"   ) endpointInfo.type = ENDPOINT_MIDI_CC_PULSE;
            break;
        case INTERFACE_RIOC:
            if      (typeName == "digital_in" ) endpointInfo.type = ENDPOINT_RIOC_DIGITAL_IN;
            else if (typeName == "digital_out") endpointInfo.type = ENDPOINT_RIOC_DIGITAL_OUT;
            else if (typeName == "analog_in"  ) endpointInfo.type = ENDPOINT_RIOC_ANALOG_IN;
            else if (typeName == "analog_out" ) endpointInfo.type = ENDPOINT_RIOC_ANALOG_OUT;
            else if (typeName == "pwm_out"    ) endpointInfo.type = ENDPOINT_RIOC_PWM_OUT;
            else if (typeName == "pfm_out"    ) endpointInfo.type = ENDPOINT_RIOC_PFM_OUT;
            else if (typeName == "encoder_in" ) endpointInfo.type = ENDPOINT_RIOC_ENCODER_IN;
            else if (typeName == "rudder_out" ) endpointInfo.type = ENDPOINT_RIOC_RUDDER_OUT;
            else if (typeName == "stepper_out") endpointInfo.type = ENDPOINT_RIOC_STEPPER_OUT;
            break;
        case INTERFACE_MODBUS:
            if      (typeName == "discrete_in") endpointInfo.type = ENDPOINT_MODBUS_DISCRETE_IN;
            else if (typeName == "coil_out"   ) endpointInfo.type = ENDPOINT_MODBUS_COIL_OUT;
            else if (typeName == "coil_in"    ) endpointInfo.type = ENDPOINT_MODBUS_COIL_IN;
            else if (typeName == "register_in") endpointInfo.type = ENDPOINT_MODBUS_REGISTER_IN;
            else if (typeName == "holding_out") endpointInfo.type = ENDPOINT_MODBUS_HOLDING_OUT;
            else if (typeName == "holding_in" ) endpointInfo.type = ENDPOINT_MODBUS_HOLDING_IN;
            break;
        case INTERFACE_HUE:
            if      (typeName == "light"      ) endpointInfo.type = ENDPOINT_HUE_LIGHT;
            else if (typeName == "group"      ) endpointInfo.type = ENDPOINT_HUE_GROUP;
            break;
        case INTERFACE_DMX:
            if      (typeName == "dimmer"     ) endpointInfo.type = ENDPOINT_DMX_DIMMER;
            else if (typeName == "dimmer2x"   ) endpointInfo.type = ENDPOINT_DMX_DIMMER2;
            else if (typeName == "media"      ) endpointInfo.type = ENDPOINT_DMX_MEDIA;
            break;
        case INTERFACE_ARTNET:
            if      (typeName == "dimmer"     ) endpointInfo.type = ENDPOINT_ARTNET_DIMMER;
            else if (typeName == "dimmer2x"   ) endpointInfo.type = ENDPOINT_ARTNET_DIMMER2;
            else if (typeName == "media"      ) endpointInfo.type = ENDPOINT_ARTNET_MEDIA;
            break;
        case INTERFACE_OSC:
            if      (typeName == "int"        ) endpointInfo.type = ENDPOINT_OSC_INT;
            else if (typeName == "float"      ) endpointInfo.type = ENDPOINT_OSC_FLOAT;
            else if (typeName == "bool"       ) endpointInfo.type = ENDPOINT_OSC_BOOL;
            break;
        case INTERFACE_AUDIOIN:
            if      (typeName == "level"      ) endpointInfo.type = ENDPOINT_AUDIOIN_LEVEL;
            break;
        case INTERFACE_SCREEN:
            if      (typeName == "light"      ) endpointInfo.type = ENDPOINT_SCREEN_LIGHT;
            else if (typeName == "media"      ) endpointInfo.type = ENDPOINT_SCREEN_MEDIA;
            else if (typeName == "canvas"     ) endpointInfo.type = ENDPOINT_SCREEN_CANVAS;
            break;
        case INTERFACE_APLAY:
            if      (typeName == "media"      ) endpointInfo.type = ENDPOINT_APLAY_MEDIA;
            break;
        case INTERFACE_MPLAY:
            if      (typeName == "media"      ) endpointInfo.type = ENDPOINT_MPLAY_MEDIA;
            break;
        case INTERFACE_PIPE:
            if      (typeName == "analog"     ) endpointInfo.type = ENDPOINT_PIPE_ANALOG;
            else if (typeName == "binary"     ) endpointInfo.type = ENDPOINT_PIPE_BINARY;
            else if (typeName == "note"       ) endpointInfo.type = ENDPOINT_PIPE_NOTE;
            break;
        case INTERFACE_LAUNCH:
            if      (typeName == "preset"     ) endpointInfo.type = ENDPOINT_LAUNCH_PRESET;
            break;
        case INTERFACE_HOTKEY:
            if      (typeName == "press"      ) endpointInfo.type = ENDPOINT_HOTKEY_PRESS;
            break;
        case INTERFACE_METRONOME:
            if      (typeName == "beat"       ) endpointInfo.type = ENDPOINT_METRONOME_BEAT;
            else if (typeName == "bpm"        ) endpointInfo.type = ENDPOINT_METRONOME_BPM;
            else if (typeName == "quantum"    ) endpointInfo.type = ENDPOINT_METRONOME_QUANTUM;
            else if (typeName == "run"        ) endpointInfo.type = ENDPOINT_METRONOME_RUN;
            else if (typeName == "link"       ) endpointInfo.type = ENDPOINT_METRONOME_LINK;
            else if (typeName == "tap"        ) endpointInfo.type = ENDPOINT_METRONOME_TAP;
            break;
        }

        // set signal type and endpoint label
        switch (endpointInfo.type) {
        case ENDPOINT_MIDI_NOTE:
            endpointInfo.signal = DATA_SIGNAL_NOTE;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = tr("MIDI Note");
            endpointInfo.labelEPI = QString("Ch%1 : %2").arg(endpointInfo.channel+1).arg(DigishowEnvironment::getMidiNoteName(endpointInfo.note));
            break;
        case ENDPOINT_MIDI_CONTROL:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = tr("MIDI CC");
            endpointInfo.labelEPI = QString("Ch%1 : %2").arg(endpointInfo.channel+1).arg(endpointInfo.control);
            break;
        case ENDPOINT_MIDI_PROGRAM:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = tr("MIDI Prgm");
            endpointInfo.labelEPI = QString("Ch%1").arg(endpointInfo.channel+1);
            break;
        case ENDPOINT_MIDI_PITCH:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_MIDI_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_MIDI_INPUT);
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
        case ENDPOINT_RIOC_DIGITAL_IN:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.input  = true;
            endpointInfo.labelEPT = tr("Digital In");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_DIGITAL_OUT:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Digital Out");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_ANALOG_IN:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.input  = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("Analog In");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_ANALOG_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("Analog Out");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_PWM_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 255;
            endpointInfo.labelEPT = tr("PWM Out");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_PFM_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
            endpointInfo.labelEPT = tr("PFM Out");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_ENCODER_IN:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.input  = true;
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
            endpointInfo.labelEPT = tr("Encoder");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_RUDDER_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 180;
            endpointInfo.labelEPT = tr("Servo");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_RIOC_STEPPER_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000);
            endpointInfo.labelEPT = tr("Stepper");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(DigishowEnvironment::getRiocPinName(m_interfaceInfo.mode, endpointInfo.channel));
            break;
        case ENDPOINT_MODBUS_DISCRETE_IN:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.input  = true;
            endpointInfo.labelEPT = tr("Discrete");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_MODBUS_COIL_OUT:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Coil");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_MODBUS_COIL_IN:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.input  = true;
            endpointInfo.labelEPT = tr("Coil");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_MODBUS_REGISTER_IN:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.input  = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("Input");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_MODBUS_HOLDING_OUT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("Holding");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_MODBUS_HOLDING_IN:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.input  = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("Holding");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel);
            break;
        case ENDPOINT_HUE_LIGHT:
        case ENDPOINT_HUE_GROUP:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = (endpointInfo.control==CONTROL_LIGHT_HUE || endpointInfo.control==CONTROL_LIGHT_CT ? 65535 : 255);
            endpointInfo.labelEPT = (endpointInfo.type==ENDPOINT_HUE_GROUP ? tr("Group") : tr("Light")) + " " + QString::number(endpointInfo.channel);
            endpointInfo.labelEPI = DigishowEnvironment::getLightControlName(endpointInfo.control);
            break;
        case ENDPOINT_DMX_DIMMER:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 255;
            endpointInfo.labelEPT = tr("DMX");
            endpointInfo.labelEPI = QString("Ch%1").arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_DMX_DIMMER2:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("DMX");
            endpointInfo.labelEPI = QString("Ch%1 +").arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_DMX_MEDIA:
            endpointInfo.output = true;
            switch (endpointInfo.control) {
            case CONTROL_MEDIA_START:
            case CONTROL_MEDIA_STOP:
            case CONTROL_MEDIA_STOP_ALL:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                break;
            }
            endpointInfo.labelEPT = tr("DMX");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);
            break;
        case ENDPOINT_ARTNET_DIMMER:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_ARTNET_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_ARTNET_INPUT);
            endpointInfo.range  = 255;
            endpointInfo.labelEPT = tr("ArtNet");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_ARTNET_DIMMER2:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_ARTNET_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_ARTNET_INPUT);
            endpointInfo.range  = 65535;
            endpointInfo.labelEPT = tr("ArtNet");
            endpointInfo.labelEPI = QString("%1 : %2 +").arg(endpointInfo.unit).arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_ARTNET_MEDIA:
            endpointInfo.output = true;
            switch (endpointInfo.control) {
            case CONTROL_MEDIA_START:
            case CONTROL_MEDIA_STOP:
            case CONTROL_MEDIA_STOP_ALL:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                break;
            }
            endpointInfo.labelEPT = tr("ArtNet");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);
            break;
        case ENDPOINT_OSC_INT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000000); // 0x7fffffff
            endpointInfo.labelEPT = tr("OSC");
            endpointInfo.labelEPI = tr("Integer") + QString(" %1").arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_OSC_FLOAT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
            endpointInfo.range  = 1000000;
            endpointInfo.labelEPT = tr("OSC");
            endpointInfo.labelEPI = tr("Float") + QString(" %1").arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_OSC_BOOL:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
            endpointInfo.labelEPT = tr("OSC");
            endpointInfo.labelEPI = tr("Bool") + QString(" %1").arg(endpointInfo.channel + 1);
            break;
        case ENDPOINT_AUDIOIN_LEVEL:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.input = true;
            endpointInfo.range  = 1000000;
            endpointInfo.labelEPT = tr("Audio");
            endpointInfo.labelEPI = tr("Level");
            break;
        case ENDPOINT_SCREEN_LIGHT:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 255;
            endpointInfo.labelEPT = tr("Backlight");
            endpointInfo.labelEPI = DigishowEnvironment::getLightControlName(endpointInfo.control);
            break;
        case ENDPOINT_SCREEN_MEDIA:
            endpointInfo.output = true;
            switch (endpointInfo.control) {
            case CONTROL_MEDIA_START:
            case CONTROL_MEDIA_STOP:
            case CONTROL_MEDIA_STOP_ALL:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                break;
            case CONTROL_MEDIA_ROTATION:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 3600;
                break;
            case CONTROL_MEDIA_OPACITY:
            case CONTROL_MEDIA_SCALE:
            case CONTROL_MEDIA_XOFFSET:
            case CONTROL_MEDIA_YOFFSET:
            case CONTROL_MEDIA_VOLUME:
            case CONTROL_MEDIA_SPEED:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 10000;
                break;
            case CONTROL_MEDIA_POSITION:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 100000);
                break;
            }
            endpointInfo.labelEPT = tr("Media Clip");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control, true);
            break;
        case ENDPOINT_SCREEN_CANVAS:
            endpointInfo.output = true;
            switch (endpointInfo.control) {
            case CONTROL_MEDIA_ROTATION:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 3600;
                break;
            case CONTROL_MEDIA_OPACITY:
            case CONTROL_MEDIA_SCALE:
            case CONTROL_MEDIA_XOFFSET:
            case CONTROL_MEDIA_YOFFSET:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 10000;
                break;
            }
            endpointInfo.labelEPT = tr("Canvas");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control, true);
            break;
        case ENDPOINT_APLAY_MEDIA:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Audio Clip");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);
            break;
        case ENDPOINT_MPLAY_MEDIA:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("MIDI Clip");
            endpointInfo.labelEPI = DigishowEnvironment::getMediaControlName(endpointInfo.control);
            break;
        case ENDPOINT_PIPE_ANALOG:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.input  = true;
            endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 65535);
            endpointInfo.labelEPT = DigishowEnvironment::getPipeModeName(m_interfaceInfo.mode);
            endpointInfo.labelEPI = tr("Analog") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_PIPE_BINARY:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.input  = true;
            endpointInfo.labelEPT = DigishowEnvironment::getPipeModeName(m_interfaceInfo.mode);;
            endpointInfo.labelEPI = tr("Binary") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_PIPE_NOTE:
            endpointInfo.signal = DATA_SIGNAL_NOTE;
            endpointInfo.output = true;
            endpointInfo.input  = true;
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = DigishowEnvironment::getPipeModeName(m_interfaceInfo.mode);;
            endpointInfo.labelEPI = tr("Note") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_LAUNCH_PRESET:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Launcher");
            endpointInfo.labelEPI = tr("Preset") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_METRONOME_BEAT:
            endpointInfo.signal = DATA_SIGNAL_NOTE;
            endpointInfo.input = true;
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = tr("Beat Maker");
            endpointInfo.labelEPI = tr("Beat %1").arg((endpointInfo.channel-1) / 4 + 1);
            if ((endpointInfo.channel-1) % 4 != 0) endpointInfo.labelEPI += QString(".%1").arg((endpointInfo.channel-1) % 4 + 1);
            break;
        case ENDPOINT_METRONOME_BPM:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 600;
            endpointInfo.labelEPT = tr("Beat Maker");
            endpointInfo.labelEPI = tr("BPM");
            break;
        case ENDPOINT_METRONOME_QUANTUM:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = true;
            endpointInfo.range  = 12;
            endpointInfo.labelEPT = tr("Beat Maker");
            endpointInfo.labelEPI = tr("Quantum");
            break;
        case ENDPOINT_METRONOME_RUN:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Beat Maker");
            endpointInfo.labelEPI = tr("Run");
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
        case ENDPOINT_HOTKEY_PRESS:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.input = true;

            QStringList hotkey = endpointInfo.address.split("+");
            switch (hotkey.length()) {
            case 1: endpointInfo.labelEPT = tr("Key"); break;
            case 2: endpointInfo.labelEPT = hotkey.first(); break;
            case 3: endpointInfo.labelEPT = hotkey.at(0) + " + " + hotkey.at(1); break;
            }
            endpointInfo.labelEPI = hotkey.last();

#ifdef Q_OS_MAC
            endpointInfo.labelEPT.replace("Ctrl", "Cmd" );
            endpointInfo.labelEPT.replace("Alt" , "Opt" );
            endpointInfo.labelEPT.replace("Meta", "Ctrl");
#endif
            break;
        }

        m_endpointInfoList.append(endpointInfo);
    }

    emit metadataUpdated();
}

int DigishowInterface::initEndpointValue(int endpointIndex)
{
    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;

    // set initial value for the endpoint
    int n = endpointIndex;
    if (m_endpointInfoList[n].initial >= 0) {
        dgsSignalData data;
        data.signal = m_endpointInfoList[n].signal;
        data.aRange = m_endpointInfoList[n].range;
        switch (data.signal) {
        case DATA_SIGNAL_ANALOG: data.aValue = round(data.aRange * m_endpointInfoList[n].initial); break;
        case DATA_SIGNAL_BINARY: data.bValue = (m_endpointInfoList[n].initial > 0.5); break;
        }

        if (m_endpointInfoList[n].input) {
            //qDebug() << "dataReceived" << n << data.signal << data.aValue << data.bValue;
            emit dataReceived(n, data);
        }
        if (m_endpointInfoList[n].output) {
            //qDebug() << "dataPrepared" << n << data.signal << data.aValue << data.bValue;
            emit dataPrepared(n, data, true);
        }
    }

    return ERR_NONE;
}

QVariantList DigishowInterface::cleanMediaList()
{
    // clean up media list (delete unused media)

    QVariantList mediaList = m_interfaceOptions.value("media").toList();
    int mediaCount = mediaList.length();
    for (int n = mediaCount-1 ; n >= 0 ; n--) {
        QString mediaName = mediaList[n].toMap().value("name").toString();
        bool exists = false;
        for (int i = 0 ; i < endpointCount() ; i++) {
            if (m_endpointOptionsList[i].value("media").toString() == mediaName &&
                m_endpointInfoList[i].enabled) {
                exists = true;
                break;
            }
        }
        if (!exists) mediaList.removeAt(n);
    }
    if (mediaList.length() < mediaCount) m_interfaceOptions["media"] = mediaList;

    return mediaList;
}



