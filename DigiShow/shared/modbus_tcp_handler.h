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

#ifndef MODBUS_TCP_HANDLER_H
#define MODBUS_TCP_HANDLER_H

#include <QObject>
#include <inttypes.h>

class TcpHandler;

class ModbusTcpHandler : public QObject
{
    Q_OBJECT
public:
    explicit ModbusTcpHandler(QObject *parent = 0);
    ~ModbusTcpHandler();

    bool open(const char* host, int port = 502);
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
    bool writeMultipleRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata);

signals:

public slots:

private:
    bool _isOpen;
    TcpHandler* _tcp;
};

#endif // MODBUS_TCP_HANDLER_H
