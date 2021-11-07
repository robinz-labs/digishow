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
//#include <unistd.h>

#include "modbus_tcp_handler.h"
#include "tcp_handler.h"

ModbusTcpHandler::ModbusTcpHandler(QObject *parent) :
    QObject(parent)
{
    _isOpen = false;
    _tcp = new TcpHandler();
}

ModbusTcpHandler::~ModbusTcpHandler()
{
    delete _tcp;
}

bool ModbusTcpHandler::open(const char* host, int port)
{
    if (_isOpen) _tcp->close();

    _tcp->setAsyncReceiver(false);
    _isOpen = _tcp->open(host, port, false);

    if (_isOpen) {
        // send the sign-in command
    }
    return _isOpen;
}

void ModbusTcpHandler::close()
{
    _tcp->close();
    _isOpen = false;
}

bool ModbusTcpHandler::readInputs(uint8_t device_address, uint16_t input_address, uint8_t quantity, bool* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x02, 0x00, 0x00, 0x00, 0x10}; // command
    //  unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x02, 0x02, 0x21, 0xa0};       // response
    //  trans       protocol    len         unit  fc    bytes data

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x02; // function 2 - Read Discrete Inputs
    cmd[8] = input_address >> 8;   // high byte of the register address
    cmd[9] = input_address & 0xff; // low  byte of the register address
    cmd[10]= 0x00;
    cmd[11]= quantity; // quantity of inputs

    // send command and receive response
    int lenBits = quantity;
    int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
    int lenRsp = lenBytes+9;

    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        rsp[8] == lenRsp-9) {

        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = rsp[n+9];

        bool *bits = new bool[lenBits];
        for (int n=0 ; n<lenBits ; n++) {
            int indexByte = n / 8;
            int indexBit = n % 8;
            bits[n] = ((bytes[indexByte] & (0x01 << indexBit)) !=0 ? true : false);
        }
        memcpy(pdata, bits, lenBits);

        delete [] bits;
        delete [] bytes;
        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusTcpHandler::readCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x10}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x01, 0x02, 0x21, 0x86};       // response
    //  trans       protocol    len         unit  fc    bytes data

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x01; // function 1 - Read Coils
    cmd[8] = coil_address >> 8;   // high byte of the register address
    cmd[9] = coil_address & 0xff; // low  byte of the register address
    cmd[10]= 0x00;
    cmd[11]= quantity; // quantity of inputs

    // send command and receive response
    int lenBits = quantity;
    int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
    int lenRsp = lenBytes+9;

    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        rsp[8] == lenRsp-9) {

        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = rsp[n+9];

        bool *bits = new bool[lenBits];
        for (int n=0 ; n<lenBits ; n++) {
            int indexByte = n / 8;
            int indexBit = n % 8;
            bits[n] = ((bytes[indexByte] & (0x01 << indexBit)) !=0 ? true : false);
        }
        memcpy(pdata, bits, lenBits);

        delete [] bits;
        delete [] bytes;
        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusTcpHandler::writeCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata)
{
    int lenBits = quantity;
    int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));

    bool *bits = new bool[lenBits];
    uint8_t *bytes = new uint8_t[lenBytes];

    memcpy(bits, pdata, lenBits);
    for (int n=0 ; n<lenBytes ; n++) bytes[n] = 0;

    for (int n=0 ; n<lenBits ; n++) {
        if (bits[n]) {
            int indexByte = n / 8;
            int indexBit = n % 8;
            bytes[indexByte] |= (0x01 << indexBit);
        }
    }

    delete [] bits;

    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11    12    13    14

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x08, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x02, 0x21, 0x86}; // command
    //  trans       protocol    len         unit  fc    address     quantity    bytes data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x10}; // response
    //  trans       protocol    len         unit  fc    address     quantity

    int lenCmd = 13+lenBytes;
    uint8_t *cmd = new uint8_t[lenCmd];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = uint8_t(lenCmd-6);
    cmd[6] = device_address;
    cmd[7] = 0x0f; // function 15 - Write Coils
    cmd[8] = coil_address >> 8;   // high byte of the register address
    cmd[9] = coil_address & 0xff; // low  byte of the register address
    cmd[10]= 0x00;
    cmd[11]= quantity; // quantity of coils
    cmd[12]= lenBytes; // length of bytes

    // dynamic data
    for (int n=0 ; n<lenBytes ; n++) cmd[13+n] = bytes[n];
    delete [] bytes;

    // send command and receive response
    int lenRsp = 12;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, lenCmd, (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        memcmp(cmd+6,rsp+6,6)==0) {

        done = true;
    }

    delete [] cmd;
    delete [] rsp;
    return done;
}

bool ModbusTcpHandler::writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x00, 0xff, 0x00}; // command
    //  trans       protocol    len         unit  fc    address     on/off

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x00, 0xff, 0x00}; // response
    //  trans       protocol    len         unit  fc    address     on/off

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x05; // function 5 - Write Single Coil
    cmd[8] = coil_address >> 8;   // high byte of the register address
    cmd[9] = coil_address & 0xff; // low  byte of the register address
    cmd[10]= (data ? 0xff : 0x00);    // on / off
    cmd[11]= 0x00;

    // send command and receive response
    uint8_t rsp[12];
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, sizeof(rsp), NULL, 0.6, NULL) == sizeof(rsp) &&
        memcmp(cmd,rsp,sizeof(rsp))==0) {

        return true;
    }

    return false;
}


bool ModbusTcpHandler::readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x02, 0x00, 0x01}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x00, 0x01};       // response
    //  trans       protocol    len         unit  fc    bytes data

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x03; // function 3 - Read Holding Registers
    cmd[8] = register_address >> 8;   // high byte of the register address
    cmd[9] = register_address & 0xff; // low  byte of the register address
    cmd[10] = 0x00;
    cmd[11]= quantity; // quantity of registers 1~125

    // send command and receive response
    int lenRsp = quantity*2+9;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        rsp[8] == lenRsp-9) {

        for (int n=9 ; n<9+quantity*2 ;n+=2) {
            *pdata = rsp[n]<<8 | rsp[n+1];
            pdata++;
        }

        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusTcpHandler::readInputRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01}; // command
    //  trans       protocol    len         unit  fc    address     quantity

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x05, 0x01, 0x04, 0x02, 0x19, 0x00};       // response
    //  trans       protocol    len         unit  fc    bytes data

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x04; // function 4 - Read Inputer Registers
    cmd[8] = register_address >> 8;   // high byte of the register address
    cmd[9] = register_address & 0xff; // low  byte of the register address
    cmd[10]= 0x00;
    cmd[11]= quantity; // quantity of registers 1~125

    // send command and receive response
    int lenRsp = quantity*2+9;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        rsp[8] == lenRsp-9) {

        for (int n=9 ; n<9+quantity*2 ;n+=2) {
            *pdata = rsp[n]<<8 | rsp[n+1];
            pdata++;
        }

        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusTcpHandler::writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x02, 0x00, 0x01}; // command
    //  trans       protocol    len         unit  fc    address     data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x02, 0x00, 0x01}; // response
    //  trans       protocol    len         unit  fc    address     data

    uint8_t cmd[12];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = 0x00; // length
    cmd[5] = 0x06;
    cmd[6] = device_address;
    cmd[7] = 0x06; // function 6 - Write Single Register
    cmd[8] = register_address >> 8;   // high byte of the register address
    cmd[9] = register_address & 0xff; // low  byte of the register address
    cmd[10]= data >> 8;   // high byte of the data
    cmd[11]= data & 0xff; // low byte of the data

    // send command and receive response
    uint8_t rsp[12];
    if (_tcp->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, sizeof(rsp), NULL, 0.6, NULL) == sizeof(rsp) &&
        memcmp(cmd,rsp,sizeof(rsp))==0) {

        return true;
    }

    return false;
}

bool ModbusTcpHandler::writeMultipleRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7     8     9     10    11    12    13

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x01, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02}; // command
    //  trans       protocol    len         unit  fc    address     quantity    bytes data

    // {0x12, 0x34, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x01, 0x00, 0x02}; // response
    //  trans       protocol    len         unit  fc    address     quantity

    int lenBytes = quantity*2;
    int lenCmd = 13+lenBytes;
    uint8_t *cmd = new uint8_t[lenCmd];
    cmd[0] = 0x00; // transaction id
    cmd[1] = 0x00;
    cmd[2] = 0x00; // protocol id
    cmd[3] = 0x00;
    cmd[4] = (lenCmd-6) >> 8; // length
    cmd[5] = (lenCmd-6) & 0xff;
    cmd[6] = device_address;
    cmd[7] = 0x10; // function 16 - Write Multiple Registers
    cmd[8] = register_address >> 8;   // high byte of the register address
    cmd[9] = register_address & 0xff; // low  byte of the register address
    cmd[10]= 0x00;
    cmd[11]= quantity; // quantity of registers
    cmd[12]= lenBytes; // length of bytes

    // dynamic data
    int i = 12;
    for (int n=0 ; n<quantity ; n++) {
        i++; cmd[i] = pdata[n] >> 8;
        i++; cmd[i] = pdata[n] & 0xff;
    }

    // send command and receive response
    int lenRsp = 12;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (_tcp->sendAndReceiveBytes((const char*)cmd, lenCmd, (char*)rsp, lenRsp, NULL, 0.6, NULL) == lenRsp &&
        memcmp(cmd+6,rsp+6,6)==0) {

        done = true;
    }

    delete [] cmd;
    delete [] rsp;
    return done;

}

