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

#ifndef MODBUS_RTU_HANDLER_H
#define MODBUS_RTU_HANDLER_H

#include <QObject>
#include <inttypes.h>

class ComHandler;
class TcpHandler;

class ModbusRtuHandler : public QObject
{
    Q_OBJECT
public:
    ModbusRtuHandler(QObject *parent = 0, bool isOverTcp = false);
    ~ModbusRtuHandler();

    bool open(const char* port, int baud, int setting);
    bool open(const char* host, int port); // over tcp
    void close();

    // bit access
    bool readInputs(uint8_t device_address, uint16_t input_address, uint8_t quantity, bool* pdata);
    bool readCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata);
    bool writeCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata);
    bool writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data);

    // word access
    bool readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata);
    bool readInputRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata);
    bool writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data);

signals:

public slots:

private:
    bool _isOpen;
    ComHandler* _com;
    TcpHandler* _tcp;

    int sendAndReceiveBytes(const char* bufSend, int lenBufSend,
                            char* bufReceive, int lenBufReceive,
                            const char* szEnding, double timeout,
                            int* err);
};

#endif // MODBUS_RTU_HANDLER_H
