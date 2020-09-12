#include <ctype.h>
#include <string.h>
//#include <unistd.h>

#include "modbus_rtu_handler.h"
#include "com_handler.h"
#include "tcp_handler.h"

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

ModbusRtuHandler::ModbusRtuHandler(QObject *parent, bool isOverTcp) :
    QObject(parent)
{
    _isOpen = false;
    _com = NULL;
    _tcp = NULL;

    if (isOverTcp)
        _tcp = new TcpHandler();
    else
        _com = new ComHandler();
}

ModbusRtuHandler::~ModbusRtuHandler()
{
    if (_com != NULL) delete _com;
    if (_tcp != NULL) delete _tcp;
}

bool ModbusRtuHandler::open(const char* port, int baud, int setting)
{
    if (_com == NULL) return false;

    if (_isOpen) _com->close();

    _com->setAsyncReceiver(false);
    _isOpen = _com->open(port, baud, setting);

    if (_isOpen) {
        // send the sign-in command
    }
    return _isOpen;
}

bool ModbusRtuHandler::open(const char* host, int port)
{
    if (_tcp == NULL) return false;

    if (_isOpen) _tcp->close();

    _tcp->setAsyncReceiver(false);
    _isOpen = _tcp->open(host, port, false);

    if (_isOpen) {
        // send the sign-in command
    }
    return _isOpen;
}


void ModbusRtuHandler::close()
{
    if (_com != NULL) _com->close();
    if (_tcp != NULL) _tcp->close();
    _isOpen = false;
}

bool ModbusRtuHandler::readInputs(uint8_t device_address, uint16_t input_address, uint8_t quantity, bool* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7

    // {0x01, 0x02, 0x00, 0x00, 0x00, 0x10, 0x79, 0xc6}; // command
    //  unit  fc    address     quantity    checksum

    // {0x01, 0x02, 0x02, 0x21, 0xa0, 0xa1, 0x90};       // response
    //  unit  fc    bytes data        checksum

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x02; // function 2 - Read Discrete Inputs
    cmd[2] = input_address >> 8;   // high byte of the register address
    cmd[3] = input_address & 0xff; // low  byte of the register address
    cmd[4] = 0x00;
    cmd[5] = quantity; // quantity of inputs

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    int lenBits = quantity;
    int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
    int lenRsp = lenBytes+5;

    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.2, NULL) == lenRsp &&
        rsp[2] == lenBytes &&
        crc16(rsp,lenRsp-2) == (rsp[lenRsp-2]<<8 | rsp[lenRsp-1])) {

        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = rsp[n+3];

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

bool ModbusRtuHandler::readCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata)
{
    // example:
    //  0     1     2     3     4     5     6     7

    // {0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x79, 0xc6}; // command
    //  unit  fc    address     quantity    checksum

    // {0x01, 0x01, 0x02, 0x21, 0x86, 0x20, 0x0e};       // response
    //  unit  fc    bytes data        checksum

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x01; // function 1 - Read Coils
    cmd[2] = coil_address >> 8;   // high byte of the register address
    cmd[3] = coil_address & 0xff; // low  byte of the register address
    cmd[4] = 0x00;
    cmd[5] = quantity; // quantity of coils

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    int lenBits = quantity;
    int lenBytes = ((lenBits % 8) ? (lenBits / 8 + 1) : (lenBits / 8));
    int lenRsp = lenBytes+5;

    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.2, NULL) == lenRsp &&
        rsp[2] == lenBytes &&
        crc16(rsp,lenRsp-2) == (rsp[lenRsp-2]<<8 | rsp[lenRsp-1])) {

        uint8_t *bytes = new uint8_t[lenBytes];
        for (int n=0 ; n<lenBytes ; n++) bytes[n] = rsp[n+3];

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

bool ModbusRtuHandler::writeCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata)
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
    //  0     1     2     3     4     5     6     7     8     9    10

    // {0x01, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x02, 0x21, 0x86, 0x7b, 0xd2}; // command
    //  unit  fc    address     quantity    bytes data        checksum

    // {0x01, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x54, 0x07}; // response
    //  unit  fc    address     quantity    checksum

    int lenCmd = 9+lenBytes;
    uint8_t *cmd = new uint8_t[lenCmd];
    cmd[0] = device_address;
    cmd[1] = 0x0f; // function 15 - Write Coils
    cmd[2] = coil_address >> 8;   // high byte of the register address
    cmd[3] = coil_address & 0xff; // low  byte of the register address
    cmd[4] = 0x00;
    cmd[5] = quantity; // quantity of coils
    cmd[6] = lenBytes; // length of bytes

    // dynamic data
    for (int n=0 ; n<lenBytes ; n++) cmd[7+n] = bytes[n];
    delete [] bytes;

    // make crc-16
    uint16_t crc = crc16(cmd, lenCmd-2);

    cmd[lenCmd-2] = crc >> 8;   //high byte of crc16
    cmd[lenCmd-1] = crc & 0xff; //low byte of crc16

    // send command and receive response
    int lenRsp = 8;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (this->sendAndReceiveBytes((const char*)cmd, lenCmd, (char*)rsp, lenRsp, NULL, 0.2, NULL) == lenRsp &&
        crc16(rsp,lenRsp-2) == (rsp[lenRsp-2]<<8 | rsp[lenRsp-1])) {

        done = true;
    }

    delete [] cmd;
    delete [] rsp;
    return done;
}

bool ModbusRtuHandler::writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data)
{
    // example:
    //  0     1     2     3     4     5     6     7

    // {0x01, 0x05, 0x00, 0x00, 0xff, 0x00, 0x8c, 0x3a}; // command
    //  unit  fc    address     on/off      checksum

    // {0x01, 0x05, 0x00, 0x00, 0xff, 0x00, 0x8c, 0x3a}; // response
    //  unit  fc    address     on/off      checksum

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x05; // function 5 - Write Single Coil
    cmd[2] = coil_address >> 8;    // high byte of the register address
    cmd[3] = coil_address & 0xff;  // low  byte of the register address
    cmd[4] = (data ? 0xff : 0x00); // on / off
    cmd[5] = 0x00;

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    uint8_t rsp[8];
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, sizeof(rsp), NULL, 0.2, NULL) == sizeof(rsp) &&
        memcmp(cmd,rsp,sizeof(rsp))==0) {

        return true;
    }

    return false;
}

bool ModbusRtuHandler::readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata)
{
    // example:
    // {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xca}; // command
    // {0x01, 0x03, 0x02, 0x00, 0x01, 0x79, 0x84};       // response

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x03; // function 3 - Read Holding Registers
    cmd[2] = register_address >> 8;   // high byte of the register address
    cmd[3] = register_address & 0xff; // low  byte of the register address
    cmd[4] = 0x00;
    cmd[5] = quantity; // quantity of registers 1~125

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    int lenRsp = quantity*2+5;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.2, NULL) == lenRsp &&
        rsp[2] == lenRsp-5 &&
        crc16(rsp,lenRsp-2) == (rsp[lenRsp-2]<<8 | rsp[lenRsp-1])) {

        for (int n=3 ; n<3+quantity*2 ;n+=2) {
            *pdata = rsp[n]<<8 | rsp[n+1];
            pdata++;
        }

        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusRtuHandler::readInputRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata)
{
    // example:
    // {0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x31, 0xca}; // command
    // {0x01, 0x04, 0x02, 0x19, 0x00, 0xb2, 0xa0};       // response

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x04; // function 4 - Read Inputer Registers
    cmd[2] = register_address >> 8;   // high byte of the register address
    cmd[3] = register_address & 0xff; // low  byte of the register address
    cmd[4] = 0x00;
    cmd[5] = quantity; // quantity of registers 1~125

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    int lenRsp = quantity*2+5;
    uint8_t *rsp = new uint8_t[lenRsp];
    bool done = false;
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, lenRsp, NULL, 0.2, NULL) == lenRsp &&
        rsp[2] == lenRsp-5 &&
        crc16(rsp,lenRsp-2) == (rsp[lenRsp-2]<<8 | rsp[lenRsp-1])) {

        for (int n=3 ; n<3+quantity*2 ;n+=2) {
            *pdata = rsp[n]<<8 | rsp[n+1];
            pdata++;
        }

        done = true;
    }

    delete [] rsp;
    return done;
}

bool ModbusRtuHandler::writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data)
{
    // example:
    // {0x01, 0x06, 0x00, 0x02, 0x00, 0x01, 0xe9, 0xca}; // command
    // {0x01, 0x06, 0x00, 0x02, 0x00, 0x01, 0xe9, 0xca}; // response

    uint8_t cmd[8];
    cmd[0] = device_address;
    cmd[1] = 0x06; // function 6 - Write Single Register
    cmd[2] = register_address >> 8;   // high byte of the register address
    cmd[3] = register_address & 0xff; // low  byte of the register address
    cmd[4] = data >> 8;   // high byte of the data
    cmd[5] = data & 0xff; // low byte of the data

    // make crc-16
    uint16_t crc = crc16(cmd, sizeof(cmd)-2);

    cmd[6] = crc >> 8;   //high byte of crc16
    cmd[7] = crc & 0xff; //low byte of crc16

    // send command and receive response
    uint8_t rsp[8];
    if (this->sendAndReceiveBytes((const char*)cmd, sizeof(cmd), (char*)rsp, sizeof(rsp), NULL, 0.2, NULL) == sizeof(rsp) &&
        memcmp(cmd,rsp,sizeof(rsp))==0) {

        return true;
    }

    return false;
}

int ModbusRtuHandler::sendAndReceiveBytes(const char* bufSend, int lenBufSend,
                        char* bufReceive, int lenBufReceive,
                        const char* szEnding, double timeout,
                        int* err)
{
    if (_com != NULL)
        return _com->sendAndReceiveBytes(bufSend, lenBufSend, bufReceive, lenBufReceive, szEnding, timeout, err);
    else
        return _tcp->sendAndReceiveBytes(bufSend, lenBufSend, bufReceive, lenBufReceive, szEnding, timeout, err);
}
