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

#ifndef MODBUS_TCP_SERVER_H
#define MODBUS_TCP_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <inttypes.h>

class ModbusTcpSocket;

class ModbusTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ModbusTcpServer(QObject *parent = 0);
    ~ModbusTcpServer();

    bool start(int port = 502);
    void stop();

    // bit access
    bool readCoils(uint8_t device_address, uint16_t coil_address, uint16_t quantity, bool* pdata);
    bool readInputs(uint8_t device_address, uint16_t input_address, uint16_t quantity, bool* pdata);
    bool writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data);
    bool writeCoils(uint8_t device_address, uint16_t coil_address, uint16_t quantity, bool* pdata);

    // word access
    bool readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata);
    bool readInputRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata);
    bool writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data);
    bool writeMultipleRegisters(uint8_t device_address, uint16_t register_address, uint16_t quantity, uint16_t* pdata);

signals:

public slots:
    void onSessionClosed(int sessionId);

private:
    bool _isStarted;

    QList<ModbusTcpSocket*> _clients;

    // memory of bit and word registers
    bool     _registerBits [0x10000]; // 64k shared memory for both input registers and coils
    uint16_t _registerWords[0x10000]; // 64k shared memory for both input and holding registers

    void clearRegisters();

protected:
    void incomingConnection(qintptr socketId);

};

class ModbusTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ModbusTcpSocket(QObject *parent = 0);
    ~ModbusTcpSocket();

    void setSessionId(int sessionId) {_sessionId = sessionId;}
    int sessionId() {return _sessionId;}

signals:
    void sessionClosed(int sessionId);

public slots:
    void onReadyRead();
    void onDisconnected();

private:
    ModbusTcpServer* _server;
    int _sessionId;

    // process bit access
    QByteArray processReadCoils(const QByteArray &dataIn);
    QByteArray processReadInputs(const QByteArray &dataIn);
    QByteArray processWriteSingleCoil(const QByteArray &dataIn);
    QByteArray processWriteCoils(const QByteArray &dataIn);

    // process word access
    QByteArray processReadHoldingRegisters(const QByteArray &dataIn);
    QByteArray processReadInputRegisters(const QByteArray &dataIn);
    QByteArray processWriteSingleRegister(const QByteArray &dataIn);
    QByteArray processWriteMultipleRegisters(const QByteArray &dataIn);
};

#endif // MODBUS_TCP_SERVER_H
