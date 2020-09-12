#include "dgs_modbus_interface.h"
#include "shared/modbus_handler.h"
#include <QSerialPortInfo>

DgsModbusInterface::DgsModbusInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "modbus";
    m_modbus = nullptr;

    m_thread = new DgsModbusThread(this);
    m_run = false;
    m_threadStatus = DgsModbusThread::Standby;
    m_threadError = 0;

    // create a timer
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
    m_timer->setSingleShot(false);
    m_timer->setInterval(10);
}

DgsModbusInterface::~DgsModbusInterface()
{
    closeInterface();

    delete m_timer;
    delete m_thread;
}

int DgsModbusInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get modbus options for port open
    m_modbusMode = m_interfaceInfo.mode;
    if (m_modbusMode == INTERFACE_MODBUS_RTU) {

        m_modbusComPort   = m_interfaceOptions.value("comPort").toString();
        m_modbusComBaud   = m_interfaceOptions.value("comBaud").toInt();

        m_modbusComParity = 0;
        QString comParty = m_interfaceOptions.value("comParity").toString();
        if (comParty == "8N1") m_modbusComParity = CH_SETTING_8N1; else
        if (comParty == "8E1") m_modbusComParity = CH_SETTING_8E1; else
        if (comParty == "8O1") m_modbusComParity = CH_SETTING_8O1; else
        if (comParty == "7E1") m_modbusComParity = CH_SETTING_7E1; else
        if (comParty == "7O1") m_modbusComParity = CH_SETTING_7O1;

    } else
    if (m_modbusMode == INTERFACE_MODBUS_TCP ||
        m_modbusMode == INTERFACE_MODBUS_RTUOVERTCP) {

        m_modbusTcpHost = m_interfaceOptions.value("tcpHost").toString();
        m_modbusTcpPort = m_interfaceOptions.value("tcpPort").toInt();

    } else return ERR_INVALID_OPTION;

    // reset realtime data of all modbus io endpoints
    resetEpData();

    // start a worker thread
    m_threadStatus = DgsModbusThread::Starting;
    m_thread->start();

    // wait until the modbus port is opened in the worker thread
    while (m_threadStatus==DgsModbusThread::Starting) {
        QThread::msleep(100);
    }

    // return error code if failed to open the modbus port
    if (m_threadError) return m_threadError;

    // start a timer in main thread
    m_timer->start();

    // succeeded to open the modbus port
    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsModbusInterface::closeInterface()
{
    m_timer->stop();

    m_run = false;
    m_thread->wait(3000);

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsModbusInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // confirm modbus interface is opened
    if (m_modbus == nullptr || m_run == false) return ERR_DEVICE_NOT_READY;

    // confirm modbus station number is valid
    int unit = m_endpointInfoList[endpointIndex].unit;
    if (unit<0 || unit>255) return ERR_INVALID_OPTION;

    // confirm modbus address number is valid
    int channel = m_endpointInfoList[endpointIndex].channel;
    if (channel<0 || channel>65535) return ERR_INVALID_OPTION;

    // prepare for writing data to coil or holding register
    int value = 0;
    int type = m_endpointInfoList[endpointIndex].type;
    if (type == ENDPOINT_MODBUS_COIL_OUT) {

        // binary --> coil
        if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;

        value = (data.bValue ? 1 : 0);

    } else if (type == ENDPOINT_MODBUS_HOLDING_OUT) {

        // analog --> holding
        if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

        value = int(65535 * uint32_t(data.aValue) / uint32_t(data.aRange==0 ? 65535 : data.aRange));
        if (value<0 || value>65535) return ERR_INVALID_DATA;

    } else return ERR_INVALID_OPTION;

    // make a request for writing data in the worker thread
    updateEpData(endpointIndex, uint16_t(value));

    return ERR_NONE;
}

void DgsModbusInterface::run() {

    m_threadError = 0;

    // create a modbus handler
    bool done = false;
    m_modbus = new ModbusHandler();

    // set options and open modbus port
    if (m_modbusMode == INTERFACE_MODBUS_RTU) {

        done = m_modbus->openRtu(m_modbusComPort.toLocal8Bit(), m_modbusComBaud, m_modbusComParity);

    } else if (m_interfaceInfo.mode == INTERFACE_MODBUS_TCP) {

        done = m_modbus->openTcp(m_modbusTcpHost.toLocal8Bit(), m_modbusTcpPort);

    } else if (m_interfaceInfo.mode == INTERFACE_MODBUS_RTUOVERTCP) {

        done = m_modbus->openRtuOverTcp(m_modbusTcpHost.toLocal8Bit(), m_modbusTcpPort);
    }

    // halt the thread if failed to open the interface
    if (!done) {
        delete m_modbus;
        m_modbus = nullptr;
        m_threadError = ERR_DEVICE_NOT_READY;
        m_threadStatus = DgsModbusThread::Standby;
        return;
    }

    // enter the working loop
    m_run = true;
    m_threadStatus = DgsModbusThread::Working;
    while (m_run) {

        processEpDataOut();
        processEpDataIn();

        QThread::msleep(5);
    }

    // finish the thread
    m_threadStatus = DgsModbusThread::Stopping;

    if (m_modbus != nullptr) {
        m_modbus->close();
        delete m_modbus;
        m_modbus = nullptr;
    }

    // the thread is over
    m_threadStatus = DgsModbusThread::Standby;
}

void DgsModbusInterface::processEpDataOut()
{
    if (m_modbus == nullptr) return;

    modbusEndpointData epData;
    while (fetchUpdatedEpDataOut(&epData)) {
        if (epData.enabled) {
            switch (epData.type) {
            case ENDPOINT_MODBUS_COIL_OUT:
                m_modbus->writeSingleCoil(epData.station, epData.address, epData.value);
                break;
            case ENDPOINT_MODBUS_HOLDING_OUT:
                m_modbus->writeSingleRegister(epData.station, epData.address, epData.value);
                break;
            }
        }
    }
}

void DgsModbusInterface::processEpDataIn()
{
    if (m_modbus == nullptr) return;

    int count = getEpDataCount();
    for (int n=0 ; n<count ; n++) {
        modbusEndpointData epData = getEpDataAt(n);
        if (epData.input && epData.enabled) {

            bool state;
            uint16_t value = 0;
            bool done = false;

            switch (epData.type) {
            case ENDPOINT_MODBUS_DISCRETE_IN:
                done = m_modbus->readInputs(epData.station, epData.address, 1, &state);
                if (done) value = state ? 1 : 0;
                break;
            case ENDPOINT_MODBUS_COIL_IN:
                done = m_modbus->readCoils(epData.station, epData.address, 1, &state);
                if (done) value = state ? 1 : 0;
                break;
            case ENDPOINT_MODBUS_REGISTER_IN:
                done = m_modbus->readInputRegisters(epData.station, epData.address, 1, &value);
                break;
            case ENDPOINT_MODBUS_HOLDING_IN:
                done = m_modbus->readHoldingRegisters(epData.station, epData.address, 1, &value);
                break;
            }

            if (done) updateEpData(n, value);
        }
    }
}

void DgsModbusInterface::onTimerFired()
{
    if (m_modbus == nullptr) return;

    modbusEndpointData epData;
    int epIndex;
    while (fetchUpdatedEpDataIn(&epData, &epIndex)) {

        dgsSignalData data;

        switch (epData.type) {
        case ENDPOINT_MODBUS_DISCRETE_IN:
        case ENDPOINT_MODBUS_COIL_IN:
            data.signal = DATA_SIGNAL_BINARY;
            data.bValue = epData.value;
            break;
        case ENDPOINT_MODBUS_REGISTER_IN:
        case ENDPOINT_MODBUS_HOLDING_IN:
            data.signal = DATA_SIGNAL_BINARY;
            data.aRange = 65535;
            data.aValue = epData.value;
            break;
        }

        if (data.signal != 0) emit dataReceived(epIndex, data);
    }
}

void DgsModbusInterface::resetEpData()
{
    m_modbusEpDataLock.lock();
    m_modbusEpDataList.clear();
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        modbusEndpointData epData;
        epData.enabled = m_endpointInfoList[n].enabled;
        epData.type    = m_endpointInfoList[n].type;
        epData.station = uint8_t (m_endpointInfoList[n].unit);
        epData.address = uint16_t(m_endpointInfoList[n].channel);
        epData.output  = m_endpointInfoList[n].output;
        epData.input   = m_endpointInfoList[n].input;
        m_modbusEpDataList.append(epData);
    }
    m_modbusEpDataLock.unlock();

}

int DgsModbusInterface::getEpDataCount()
{
    int count = 0;

    m_modbusEpDataLock.lock();
    count = m_modbusEpDataList.length();
    m_modbusEpDataLock.unlock();

    return count;
}

modbusEndpointData DgsModbusInterface::getEpDataAt(int index)
{
    modbusEndpointData epData;

    m_modbusEpDataLock.lock();
    if (index < m_modbusEpDataList.length()) {
        epData = m_modbusEpDataList[index];
    }
    m_modbusEpDataLock.unlock();

    return epData;
}

void DgsModbusInterface::updateEpData(int index, uint16_t value)
{
    m_modbusEpDataLock.lock();
    if (index < m_modbusEpDataList.length()) {
        m_modbusEpDataList[index].value = value;
        m_modbusEpDataList[index].updated = true;
    }
    m_modbusEpDataLock.unlock();
}

bool DgsModbusInterface::fetchUpdatedEpDataOut(modbusEndpointData *pEpDataOut, int *pEpIndex)
{
    bool fetched = false;

    m_modbusEpDataLock.lock();
    for (int n=0 ; n<m_modbusEpDataList.length() ; n++) {
        if (m_modbusEpDataList[n].updated && m_modbusEpDataList[n].output) {
            *pEpDataOut = m_modbusEpDataList[n];
            if (pEpIndex != nullptr) *pEpIndex = n;
            m_modbusEpDataList[n].updated = false;
            fetched = true;
            break;
        }
    }
    m_modbusEpDataLock.unlock();

    return fetched;
}

bool DgsModbusInterface::fetchUpdatedEpDataIn(modbusEndpointData *pEpDataIn, int *pEpIndex)
{
    bool fetched = false;

    m_modbusEpDataLock.lock();
    for (int n=0 ; n<m_modbusEpDataList.length() ; n++) {
        if (m_modbusEpDataList[n].updated && m_modbusEpDataList[n].input) {
            *pEpDataIn = m_modbusEpDataList[n];
            if (pEpIndex != nullptr) *pEpIndex = n;
            m_modbusEpDataList[n].updated = false;
            fetched = true;
            break;
        }
    }
    m_modbusEpDataLock.unlock();

    return fetched;
}

QVariantList DgsModbusInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

#ifdef Q_OS_MAC
        if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif
        info.clear();
        info["comPort"       ] = serialPortInfo.portName();
        info["comVid"        ] = serialPortInfo.vendorIdentifier();
        info["comPid"        ] = serialPortInfo.productIdentifier();
        info["comFacturer"   ] = serialPortInfo.manufacturer();
        info["comDescription"] = serialPortInfo.description();
        info["mode"          ] = "rtu";
        list.append(info);
    }

    return list;
}
