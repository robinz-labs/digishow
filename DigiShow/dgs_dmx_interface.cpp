#include "dgs_dmx_interface.h"
#include "shared/com_handler.h"

#include <QSerialPortInfo>

// FTDI USB
#define FTDI_VID 1027
#define FTDI_PID 24577

DgsDmxInterface::DgsDmxInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "dmx";
    m_com = nullptr;
    m_timer = nullptr;

    // clear dmx data buffer
    for (int n=0 ; n<512 ; n++) m_data[n]=0;
}

DgsDmxInterface::~DgsDmxInterface()
{
    closeInterface();
}

int DgsDmxInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get com port configuration
    QString comPort = m_interfaceOptions.value("comPort").toString();
    if (comPort.isEmpty()) comPort = ComHandler::findPort(FTDI_VID, FTDI_PID); // use default port
    if (comPort.isEmpty()) return ERR_INVALID_OPTION;

    // get number of total dmx channels
    int channels = 0;
    for (int n = 0 ; n<m_endpointInfoList.length() ; n++) {
        if (m_endpointInfoList[n].channel+1 > channels) channels = m_endpointInfoList[n].channel+1;
    }

    // create a com handler for the serial communication with the device
    bool done = false;
    m_com = new ComHandler();

    if (m_interfaceInfo.mode == INTERFACE_DMX_ENTTEC_USB) {
        done = enttecDmxOpen(comPort, channels);
    }

    // create a timer for sending dmx frames at a particular frequency
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
    m_timer->setSingleShot(false);
    m_timer->setInterval(20);
    m_timer->start();

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsDmxInterface::closeInterface()
{
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    if (m_com != nullptr) {
        m_com->close();
        delete m_com;
        m_com = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;

}

int DgsDmxInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

    int value = 255 * data.aValue / (data.aRange==0 ? 255 : data.aRange);
    if (value<0 || value>255) return ERR_INVALID_DATA;

    // update dmx data buffer
    int channel = m_endpointInfoList[endpointIndex].channel;
    m_data[channel] = static_cast<unsigned char>(value);

    // send dmx data frame
    //enttecDmxSendDmxFrame(m_data);

    return ERR_NONE;
}

void DgsDmxInterface::onTimerFired()
{
    // send dmx data frame
    enttecDmxSendDmxFrame(m_data);
}

bool DgsDmxInterface::enttecDmxOpen(const QString &port, int channels)
{
    // open serial port of the dmx adapter
    m_com->setAsyncReceiver(true);
    if (!m_com->open(port.toLocal8Bit(), 57600, CH_SETTING_8N1)) return false;

    // clear dmx data buffer
    //for (int n=0 ; n<512 ; n++) m_data[n]=0;

    // set number of channels
    m_channels = (channels+7) / 8 * 8;
    if (m_channels<24) m_channels = 24; else if (m_channels>512) m_channels = 512;

    return true;
}

bool DgsDmxInterface::enttecDmxSendDmxFrame(unsigned char *data)
{
    if (m_channels < 24 || m_channels > 512) return false;
    int length = m_channels;

    unsigned char head[] = {
        0x7e, // 0  leading character
        0x06, // 1  message type (6 = Output Only Send DMX Packet Request)
        0x00, // 2  data length LSB
        0x00, // 3  data length MSB
        0x00  // 4  start code (always zero)
    };
    head[2] = static_cast<unsigned char>((length+1) & 0xff);
    head[3] = static_cast<unsigned char>((length+1) >> 8);

    unsigned char tail[] = { 0xe7 };

    // send frame
    bool done = true;
    if (!m_com->isBusySending()) {
        done &= m_com->sendBytes((const char*)head, sizeof(head));
        done &= m_com->sendBytes((const char*)data, length);
        done &= m_com->sendBytes((const char*)tail, sizeof(tail));
    }

    return done;
}

QVariantList DgsDmxInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

#ifdef Q_OS_MAC
        if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif

        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier() &&
            serialPortInfo.vendorIdentifier()==FTDI_VID && serialPortInfo.productIdentifier()==FTDI_PID) {

            info.clear();
            info["comPort"  ] = serialPortInfo.portName();
            info["mode"     ] = "enttec";
            list.append(info);
        }
    }

    return list;
}

