#include "digishow_interface.h"
#include "digishow_environment.h"

DigishowInterface::DigishowInterface(QObject *parent) : QObject(parent)
{
    m_isInterfaceOpened = false;
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

        info["labelEPT"] = m_endpointInfoList[index].labelEPT;
        info["labelEPI"] = m_endpointInfoList[index].labelEPI;
    }

    return info;
}

int DigishowInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;
    updateMetadata();
    return ERR_NONE;
}

int DigishowInterface::closeInterface()
{
    m_isInterfaceOpened = true;
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
    if      (typeName == "midi"  ) m_interfaceInfo.type = INTERFACE_MIDI;
    else if (typeName == "rioc"  ) m_interfaceInfo.type = INTERFACE_RIOC;
    else if (typeName == "modbus") m_interfaceInfo.type = INTERFACE_MODBUS;
    else if (typeName == "hue"   ) m_interfaceInfo.type = INTERFACE_HUE;
    else if (typeName == "dmx"   ) m_interfaceInfo.type = INTERFACE_DMX;
    else if (typeName == "artnet") m_interfaceInfo.type = INTERFACE_ARTNET;
    else if (typeName == "screen") m_interfaceInfo.type = INTERFACE_SCREEN;
    else if (typeName == "aplay" ) m_interfaceInfo.type = INTERFACE_APLAY;
    else if (typeName == "mplay" ) m_interfaceInfo.type = INTERFACE_MPLAY;
    else if (typeName == "pipe"  ) m_interfaceInfo.type = INTERFACE_PIPE;
    else if (typeName == "launch") m_interfaceInfo.type = INTERFACE_LAUNCH;

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
        break;
    case INTERFACE_MODBUS:
        if      (modeName == "rtu"         ) m_interfaceInfo.mode = INTERFACE_MODBUS_RTU;
        else if (modeName == "tcp"         ) m_interfaceInfo.mode = INTERFACE_MODBUS_TCP;
        else if (modeName == "rtuovertcp"  ) m_interfaceInfo.mode = INTERFACE_MODBUS_RTUOVERTCP;
        break;
    case INTERFACE_HUE:
        if      (modeName == "" ||
                 modeName == "http"        ) m_interfaceInfo.mode = INTERFACE_HUE_HTTP;
        break;
    case INTERFACE_DMX:
        if      (modeName == "enttec"      ) m_interfaceInfo.mode = INTERFACE_DMX_ENTTEC_PRO;
        else if (modeName == "opendmx"     ) m_interfaceInfo.mode = INTERFACE_DMX_ENTTEC_OPEN;
        break;
    case INTERFACE_ARTNET:
        if      (modeName == "input"       ) m_interfaceInfo.mode = INTERFACE_ARTNET_INPUT;
        else if (modeName == "output"      ) m_interfaceInfo.mode = INTERFACE_ARTNET_OUTPUT;
        break;
    case INTERFACE_SCREEN:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_SCREEN_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_SCREEN_REMOTE;
        break;
    case INTERFACE_APLAY:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_APLAY_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_APLAY_REMOTE;
        break;
    case INTERFACE_MPLAY:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_MPLAY_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_MPLAY_REMOTE;
        break;
    case INTERFACE_PIPE:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_PIPE_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_PIPE_REMOTE;
        break;
    case INTERFACE_LAUNCH:
        if      (modeName == "local"       ) m_interfaceInfo.mode = INTERFACE_LAUNCH_LOCAL;
        else if (modeName == "remote"      ) m_interfaceInfo.mode = INTERFACE_LAUNCH_REMOTE;
        break;
    }

    // set interface input flag
    if (m_interfaceInfo.mode==0)
        m_interfaceInfo.input = false;
    else if (m_interfaceInfo.mode==INTERFACE_MIDI_OUTPUT ||
             m_interfaceInfo.mode==INTERFACE_ARTNET_OUTPUT ||
             m_interfaceInfo.type==INTERFACE_HUE ||
             m_interfaceInfo.type==INTERFACE_DMX ||
             m_interfaceInfo.type==INTERFACE_SCREEN ||
             m_interfaceInfo.type==INTERFACE_APLAY ||
             m_interfaceInfo.type==INTERFACE_MPLAY ||
             m_interfaceInfo.type==INTERFACE_LAUNCH)
        m_interfaceInfo.input = false;
    else
        m_interfaceInfo.input = true;

    // set interface output flag
    if (m_interfaceInfo.mode==0)
        m_interfaceInfo.output = false;
    else if (m_interfaceInfo.mode==INTERFACE_MIDI_INPUT ||
             m_interfaceInfo.mode==INTERFACE_ARTNET_INPUT)
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
        labelType = (m_interfaceInfo.mode==INTERFACE_RIOC_ALADDIN ? tr("Aladdin") : tr("Arduino"));
        labelIdentity = m_interfaceOptions.value("comPort").toString();
        break;
    case INTERFACE_MODBUS:
        labelType = tr("Modbus");
        labelIdentity = (m_interfaceInfo.mode==INTERFACE_MODBUS_RTU ?
                         m_interfaceOptions.value("comPort").toString() :
                         m_interfaceOptions.value("tcpHost").toString()) ;
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
        labelIdentity = m_interfaceOptions.value("udpHost").toString();
        break;
    case INTERFACE_SCREEN:
        labelType = tr("Screen");
        labelIdentity = m_interfaceOptions.value("screen").toString();
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
            labelIdentity = m_interfaceOptions.value("tcpHost").toString();
        } else {
            labelType = tr("Virtual Pipe");
            labelIdentity = m_interfaceOptions.value("comment").toString();
        }
        break;
    case INTERFACE_LAUNCH:
        labelType = tr("Preset Launch");
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

        // set endpoint type
        typeName = m_endpointOptionsList[n].value("type").toString();
        switch (m_interfaceInfo.type) {
        case INTERFACE_MIDI:
            if      (typeName == "note"       ) endpointInfo.type = ENDPOINT_MIDI_NOTE;
            else if (typeName == "control"    ) endpointInfo.type = ENDPOINT_MIDI_CONTROL;
            else if (typeName == "program"    ) endpointInfo.type = ENDPOINT_MIDI_PROGRAM;
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
            break;
        case INTERFACE_ARTNET:
            if      (typeName == "dimmer"     ) endpointInfo.type = ENDPOINT_ARTNET_DIMMER;
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
        case ENDPOINT_ARTNET_DIMMER:
            endpointInfo.signal = DATA_SIGNAL_ANALOG;
            endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_ARTNET_OUTPUT);
            endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_ARTNET_INPUT);
            endpointInfo.range  = 255;
            endpointInfo.labelEPT = tr("ArtNet");
            endpointInfo.labelEPI = QString("%1 : %2").arg(endpointInfo.unit).arg(endpointInfo.channel + 1);
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
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.range  = 10000;
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
            endpointInfo.labelEPT = (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE ? tr("Remote") : tr("Pipe"));
            endpointInfo.labelEPI = tr("Analog") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_PIPE_BINARY:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.input  = true;
            endpointInfo.labelEPT = (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE ? tr("Remote") : tr("Pipe"));
            endpointInfo.labelEPI = tr("Binary") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_PIPE_NOTE:
            endpointInfo.signal = DATA_SIGNAL_NOTE;
            endpointInfo.output = true;
            endpointInfo.input  = true;
            endpointInfo.range  = 127;
            endpointInfo.labelEPT = (m_interfaceInfo.mode == INTERFACE_PIPE_REMOTE ? tr("Remote") : tr("Pipe"));
            endpointInfo.labelEPI = tr("Note") + " " + QString::number(endpointInfo.channel);
            break;
        case ENDPOINT_LAUNCH_PRESET:
            endpointInfo.signal = DATA_SIGNAL_BINARY;
            endpointInfo.output = true;
            endpointInfo.labelEPT = tr("Launch");
            endpointInfo.labelEPI = tr("Preset") + " " + QString::number(endpointInfo.channel);
            break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
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
            emit dataPrepared(n, data);
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



