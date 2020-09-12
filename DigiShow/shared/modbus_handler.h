#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <QObject>
#include <inttypes.h>
#include "com_handler.h"
#include "modbus_rtu_handler.h"
#include "modbus_tcp_handler.h"

class ModbusHandler : public QObject
{
    Q_OBJECT

public:

    ModbusHandler(QObject *parent = nullptr) :
        QObject(parent)
    {
        _rtu = nullptr;
        _tcp = nullptr;
    }

    ~ModbusHandler() {
        if (_rtu != nullptr) { delete _rtu; _rtu = nullptr; } else
        if (_tcp != nullptr) { delete _tcp; _tcp = nullptr; }
    }

    bool openRtu(const char* port, int baud, int setting) {
        close();
        _rtu = new ModbusRtuHandler();
        return _rtu->open(port, baud, setting);
    }

    bool openTcp(const char* host, int port) {
        close();
        _tcp = new ModbusTcpHandler();
        return _tcp->open(host, port);
    }

    bool openRtuOverTcp(const char* host, int port) {
        close();
        _rtu = new ModbusRtuHandler(nullptr, true);
        return _rtu->open(host, port);
    }

    void close() {
        if (_rtu != nullptr) { _rtu->close(); _rtu = nullptr; } else
        if (_tcp != nullptr) { _tcp->close(); _tcp = nullptr; }
    }

    // bit access
    bool readInputs(uint8_t device_address, uint16_t input_address, uint8_t quantity, bool* pdata) {
        if (_rtu != nullptr) return _rtu->readInputs(device_address, input_address, quantity, pdata); else
        if (_tcp != nullptr) return _tcp->readInputs(device_address, input_address, quantity, pdata); else return false;
    }

    bool readCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata) {
        if (_rtu != nullptr) return _rtu->readCoils(device_address, coil_address, quantity, pdata); else
        if (_tcp != nullptr) return _tcp->readCoils(device_address, coil_address, quantity, pdata); else return false;
    }

    bool writeCoils(uint8_t device_address, uint16_t coil_address, uint8_t quantity, bool* pdata) {
        if (_rtu != nullptr) return _rtu->writeCoils(device_address, coil_address, quantity, pdata); else
        if (_tcp != nullptr) return _tcp->writeCoils(device_address, coil_address, quantity, pdata); else return false;
    }

    bool writeSingleCoil(uint8_t device_address, uint16_t coil_address, bool data) {
        if (_rtu != nullptr) return _rtu->writeSingleCoil(device_address, coil_address, data); else
        if (_tcp != nullptr) return _tcp->writeSingleCoil(device_address, coil_address, data); else return false;
    }

    // word access
    bool readHoldingRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata) {
        if (_rtu != nullptr) return _rtu->readHoldingRegisters(device_address, register_address, quantity, pdata); else
        if (_tcp != nullptr) return _tcp->readHoldingRegisters(device_address, register_address, quantity, pdata); else return false;
    }

    bool readInputRegisters(uint8_t device_address, uint16_t register_address, uint8_t quantity, uint16_t* pdata) {
        if (_rtu != nullptr) return _rtu->readInputRegisters(device_address, register_address, quantity, pdata); else
        if (_tcp != nullptr) return _tcp->readInputRegisters(device_address, register_address, quantity, pdata); else return false;
    }

    bool writeSingleRegister(uint8_t device_address, uint16_t register_address, uint16_t data) {
        if (_rtu != nullptr) return _rtu->writeSingleRegister(device_address, register_address, data); else
        if (_tcp != nullptr) return _tcp->writeSingleRegister(device_address, register_address, data); else return false;
    }

signals:

public slots:

private:
    ModbusRtuHandler* _rtu;
    ModbusTcpHandler* _tcp;
};

#endif // MODBUS_HANDLER_H
