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

signals:

public slots:

private:
    bool _isOpen;
    TcpHandler* _tcp;
};

#endif // MODBUS_TCP_HANDLER_H
