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

#include <ctype.h>
#include <string.h>
#include <QDataStream>

#include "modbus_tcp_server.h"

ModbusTcpServer::ModbusTcpServer(QObject *parent) :
    QTcpServer(parent)
{
    _isStarted = false;
    clearRegisters();
}

ModbusTcpServer::~ModbusTcpServer()
{
    if (_isStarted) stop();
}

bool ModbusTcpServer::start(int port)
{
    if (_isStarted) stop();

    //clearRegisters();

    _isStarted = listen(QHostAddress::Any, port);

    if (_isStarted) {
        // send the sign-in command
    }
    return _isStarted;
}

void ModbusTcpServer::stop()
{
    close();

    qDeleteAll(_clients.begin(), _clients.end());
    _clients.clear();

    _isStarted = false;
}

bool ModbusTcpServer::readCoils(uint8_t device_address, uint16_t coil_address, uint16_t quantity, bool* pdata)
{
    Q_UNUSED(device_address)

    if (int(coil_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        pdata[n] = _registerBits[coil_address + n];
    }

    return true;
}

bool ModbusTcpServer::readInputs(uint8_t device_address, uint16_t input_address, uint16_t quantity, bool* pdata)
{
    Q_UNUSED(device_address)

    if (int(input_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        pdata[n] = _registerBits[input_address + n];
    }

    return true;
}

bool ModbusTcpServer::writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data)
{
    Q_UNUSED(device_address)

    if (!_isStarted) return false;

    _registerBits[coil_address] = data;

    return true;
}

bool ModbusTcpServer::writeCoils(uint8_t device_address, uint16_t coil_address, uint16_t quantity, bool* pdata)
{
    Q_UNUSED(device_address)

    if (int(coil_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        _registerBits[coil_address + n] = pdata[n];
    }

    return true;
}

bool ModbusTcpServer::readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata)
{
    Q_UNUSED(device_address)

    if (int(register_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        pdata[n] = _registerWords[register_address + n];
    }

    return true;
}

bool ModbusTcpServer::readInputRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata)
{
    Q_UNUSED(device_address)

    if (int(register_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        pdata[n] = _registerWords[register_address + n];
    }

    return true;
}

bool ModbusTcpServer::writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data)
{
    Q_UNUSED(device_address)

    _registerWords[register_address] = data;

    return true;
}

bool ModbusTcpServer::writeMultipleRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata)
{
    Q_UNUSED(device_address)

    if (int(register_address) + quantity > 0x10000) return false;

    for (int n=0 ; n<quantity ; n++) {
        _registerWords[register_address + n] = pdata[n];
    }

    return true;
}

void ModbusTcpServer::clearRegisters()
{
    for (int n=0 ; n<0x10000 ; n++) _registerBits[n] = false;
    for (int n=0 ; n<0x10000 ; n++) _registerWords[n] = 0;
}

void ModbusTcpServer::incomingConnection(qintptr socketId)
{
    static int sessionId = 0;
    sessionId++;

    qDebug() << "modbus tcp session created: " << sessionId;

    ModbusTcpSocket *socket = new ModbusTcpSocket(this);
    socket->setSocketDescriptor(socketId);
    socket->setSessionId(sessionId);
    QObject::connect(socket, SIGNAL(sessionClosed(int)), this, SLOT(onSessionClosed(int)));

    _clients << socket;
}

void ModbusTcpServer::onSessionClosed(int sessionId)
{
    foreach (ModbusTcpSocket *client, _clients) {

        if (client->sessionId() == sessionId) {

            _clients.removeAll(client);
            client->deleteLater();

            qDebug() << "modbus tcp session closed: " << sessionId;

            break;
        }
    }
}

ModbusTcpSocket::ModbusTcpSocket(QObject *parent) :
    QTcpSocket(parent)
{
    _server = (ModbusTcpServer*)parent;

    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

ModbusTcpSocket::~ModbusTcpSocket()
{

}

void ModbusTcpSocket::onReadyRead()
{
    #ifdef QT_DEBUG
    qDebug() << _sessionId << "ready read";
    #endif

    // read incoming data
    QByteArray dataIn;
    QDataStream in(this);
    char buf[1024];
    int len = 0;
    while ((len = in.readRawData(buf, sizeof(buf)-1)) > 0) {

        dataIn.append(buf, len);
    }

    // confirm incoming data format is valid
    // BYTE 0~1 TRANSACTION ID
    // BYTE 2~3 PROTOCOL ID
    // BYTE 4~5 LENGTH FIELD
    // BYTE 6   UNIT ID
    // BYTE 7   FUNCTION CODE
    // BYTE 8~  MORE DATE ...
    if (dataIn.length() <= 7 ||
        dataIn.at(2) != 0 || dataIn.at(3) != 0 ||
        (int(dataIn.at(4))<<8 | dataIn.at(5)) != dataIn.length()-6)
        return;

    #ifdef QT_DEBUG
    qDebug().noquote() << "DATA IN: " << dataIn.toHex();
    #endif

    // process modbus access
    QByteArray dataOut;
    char func = dataIn.at(7);
    switch (func) {

    case  1: dataOut = processReadCoils(dataIn); break;
    case  2: dataOut = processReadInputs(dataIn); break;
    case  5: dataOut = processWriteSingleCoil(dataIn); break;
    case 15: dataOut = processWriteCoils(dataIn); break;

    case  3: dataOut = processReadHoldingRegisters(dataIn); break;
    case  4: dataOut = processReadInputRegisters(dataIn); break;
    case  6: dataOut = processWriteSingleRegister(dataIn); break;
    case 16: dataOut = processWriteMultipleRegisters(dataIn); break;

    }

    // write outgoing data
    if (dataOut.isEmpty()) return;

    QDataStream out(this);
    out.writeRawData(dataOut.constData(), dataOut.count());

    #ifdef QT_DEBUG
    qDebug().noquote() << "DATA OUT:" << dataOut.toHex();
    #endif
}

void ModbusTcpSocket::onDisconnected()
{
    emit sessionClosed(_sessionId);
}

QByteArray ModbusTcpSocket::processReadCoils(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x10}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x01, 0x02, 0xff, 0xff};       // response
    //  trans       protocol    len         unit  fc    bytes data

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    bool *pdata = new bool[quantity];

    if (_server->readCoils(unit, address, quantity, pdata)) {

        // make response
        int lenBits = quantity;
        int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
        int lenRsp = lenBytes+9;
        int lenPayload = lenBytes+3;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x01;   // function 1 - Read Coils
        rsp[8] = lenBytes;

        // put coil status into the payload
        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = 0;

        bool *bits = pdata;
        for (int n=0 ; n<lenBits ; n++) {
            if (bits[n]) {
                int indexByte = n / 8;
                int indexBit = n % 8;
                bytes[indexByte] |= (0x01 << indexBit);
            }
        }

        for (int n=0 ; n<lenBytes ; n++) rsp[9+n] = bytes[n];
        delete [] bytes;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] pdata;

    return dataOut;
}

QByteArray ModbusTcpSocket::processReadInputs(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x02, 0x00, 0x00, 0x00, 0x10}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x02, 0x02, 0xff, 0xff};       // response
    //  trans       protocol    len         unit  fc    bytes data

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    bool *pdata = new bool[quantity];

    if (_server->readInputs(unit, address, quantity, pdata)) {

        // make response
        int lenBits = quantity;
        int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
        int lenRsp = lenBytes+9;
        int lenPayload = lenBytes+3;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x02;   // function 2 - Read Discrete Inputs
        rsp[8] = lenBytes;

        // put input status into the payload
        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = 0;

        bool *bits = pdata;
        for (int n=0 ; n<lenBits ; n++) {
            if (bits[n]) {
                int indexByte = n / 8;
                int indexBit = n % 8;
                bytes[indexByte] |= (0x01 << indexBit);
            }
        }

        for (int n=0 ; n<lenBytes ; n++) rsp[9+n] = bytes[n];
        delete [] bytes;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] pdata;

    return dataOut;
}

QByteArray ModbusTcpSocket::processWriteSingleCoil(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x00, 0xff, 0x00}; // command
    //  trans       protocol    len         unit  fc    address     on/off

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x00, 0xff, 0x00}; // response
    //  trans       protocol    len         unit  fc    address     on/off

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    bool value = (msg[10] != 0);

    if (_server->writeSingleCoil(unit, address, value)) {

        // make response
        int lenRsp = 12;
        int lenPayload = 6;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x05;   // function 5 - Write Single Coil
        rsp[8] = address >> 8;
        rsp[9] = address & 0xff;
        rsp[10]= (value ? 0xff : 0x00);
        rsp[11]= 0x00;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    return dataOut;
}

QByteArray ModbusTcpSocket::processWriteCoils(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11    12    13    14

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x09, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x02, 0xff, 0xff}; // command
    //  trans       protocol    len         unit  fc    address     quantity    bytes data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x10}; // response
    //  trans       protocol    len         unit  fc    address     quantity

    QByteArray dataOut;

    if (dataIn.length() < 14) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    uint8_t lenBytes = msg[12];

    // confirm data length is ok
    int lenBits = quantity;
    if (dataIn.length() != lenBytes + 13 || lenBits > lenBytes*8) return dataOut;

    // prepare data to write coils
    uint8_t *bytes = new uint8_t[lenBytes];
    for (int n=0 ; n<lenBytes ; n++) bytes[n] = msg[n+13];

    bool *bits = new bool[lenBits];
    for (int n=0 ; n<lenBits ; n++) {
        int indexByte = n / 8;
        int indexBit = n % 8;
        bits[n] = ((bytes[indexByte] & (0x01 << indexBit)) !=0 ? true : false);
    }

    if (_server->writeCoils(unit, address, quantity, bits)) {

        // make response
        int lenRsp = 12;
        int lenPayload = 6;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x0f;   // function 15 - Write Coils
        rsp[8] = address >> 8;
        rsp[9] = address & 0xff;
        rsp[10]= quantity >> 8;
        rsp[11]= quantity & 0xff;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] bits;
    delete [] bytes;

    return dataOut;
}

QByteArray ModbusTcpSocket::processReadHoldingRegisters(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x02, 0x00, 0x01}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0xff, 0xff};       // response
    //  trans       protocol    len         unit  fc    bytes data

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    uint16_t *pdata = new uint16_t[quantity];

    if (_server->readHoldingRegisters(unit, address, quantity, pdata)) {

        // make response
        int lenBytes = quantity*2;
        int lenRsp = lenBytes+9;
        int lenPayload = lenBytes+3;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x03;   // function 3 - Read Holding Registers
        rsp[8] = lenBytes;

        // put register values into the payload
        int i = 8;
        for (int n=0 ; n<quantity ; n++) {
            i++; rsp[i] = pdata[n] >> 8;
            i++; rsp[i] = pdata[n] & 0xff;
        }

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] pdata;

    return dataOut;
}

QByteArray ModbusTcpSocket::processReadInputRegisters(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x04, 0x02, 0xff, 0xff};       // response
    //  trans       protocol    len         unit  fc    bytes data

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    uint16_t *pdata = new uint16_t[quantity];

    if (_server->readInputRegisters(unit, address, quantity, pdata)) {

        // make response
        int lenBytes = quantity*2;
        int lenRsp = lenBytes+9;
        int lenPayload = lenBytes+3;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x04;   // function 4 - Read Inputer Registers
        rsp[8] = lenBytes;

        // put register values into the payload
        int i = 8;
        for (int n=0 ; n<quantity ; n++) {
            i++; rsp[i] = pdata[n] >> 8;
            i++; rsp[i] = pdata[n] & 0xff;
        }

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] pdata;

    return dataOut;
}

QByteArray ModbusTcpSocket::processWriteSingleRegister(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x02, 0xff, 0xff}; // command
    //  trans       protocol    len         unit  fc    address     data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x02, 0xff, 0xff}; // response
    //  trans       protocol    len         unit  fc    address     data

    QByteArray dataOut;

    if (dataIn.length() != 12) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t value = uint16_t(msg[10])<<8 | msg[11];

    if (_server->writeSingleRegister(unit, address, value)) {

        // make response
        int lenRsp = 12;
        int lenPayload = 6;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x06;   // function 6 - Write Single Register
        rsp[8] = address >> 8;
        rsp[9] = address & 0xff;
        rsp[10]= value >> 8;
        rsp[11]= value & 0xff;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    return dataOut;
}

QByteArray ModbusTcpSocket::processWriteMultipleRegisters(const QByteArray &dataIn)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x01, 0x00, 0x02, 0x04, 0xff, 0xff, 0xff, 0xff}; // command
    //  trans       protocol    len         unit  fc    address     quantity    bytes data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x01, 0x00, 0x02}; // response
    //  trans       protocol    len         unit  fc    address     quantity

    QByteArray dataOut;

    if (dataIn.length() < 15) return dataOut;

    uint8_t *msg = (uint8_t*)dataIn.constData();

    uint8_t unit = msg[6];
    uint16_t address = uint16_t(msg[8])<<8 | msg[9];
    uint16_t quantity = uint16_t(msg[10])<<8 | msg[11];
    uint8_t lenBytes = msg[12];

    // confirm data length is ok
    if (dataIn.length() != lenBytes + 13 || quantity*2 != lenBytes) return dataOut;

    // prepare data to write registers
    uint16_t *values = new uint16_t[quantity];
    int i = 13;
    for (int n=0 ; n<quantity ; n++) {
        values[n] = uint16_t(msg[i])<<8 | msg[i+1];
        i+=2;
    }

    if (_server->writeMultipleRegisters(unit, address, quantity, values)) {

        // make response
        int lenRsp = 12;
        int lenPayload = 6;

        uint8_t *rsp = new uint8_t[lenRsp];
        rsp[0] = msg[0]; // transaction id
        rsp[1] = msg[1];
        rsp[2] = msg[2]; // protocol id
        rsp[3] = msg[3];
        rsp[4] = lenPayload >> 8;
        rsp[5] = lenPayload & 0xff;
        rsp[6] = unit;   // unit id
        rsp[7] = 0x10;   // function 16 - Write Multiple Registers
        rsp[8] = address >> 8;
        rsp[9] = address & 0xff;
        rsp[10]= quantity >> 8;
        rsp[11]= quantity & 0xff;

        // package response bytes
        dataOut = QByteArray((const char*)rsp, lenRsp);
        delete [] rsp;
    }

    delete [] values;

    return dataOut;
}


