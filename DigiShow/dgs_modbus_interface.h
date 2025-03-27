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

#ifndef DGSMODBUSINTERFACE_H
#define DGSMODBUSINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

typedef struct modbusEndpointData {

    bool enabled;

    int type;         // modbus io type
    uint8_t  station; // modbus station number
    uint16_t address; // modbus address number
    bool output;      // is output
    bool input;       // is input

    uint16_t value;   // latest data value
    bool updated;     // data updated flag

    // defaults
    modbusEndpointData() :
      enabled(true),
      type(0),
      station(0),
      address(0),
      output(false),
      input(false),
      value(0),
      updated(false)
    {}

} modbusEndpointData;

class ModbusHandler;
class DgsModbusThread;

class DgsModbusInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsModbusInterface(QObject *parent = nullptr);
    ~DgsModbusInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    void run(); // called by the worker thread

    static QVariantList listOnline();

signals:

public slots:
    void onTimerFired();

private:

    // modbus handler (lives in worker thread)
    ModbusHandler *m_modbus;

    // timer (lives in main thread)
    QTimer *m_timer;

    // worker thread
    DgsModbusThread* m_thread;

    // threading flags (shared between both threads)
    bool m_run;
    int m_threadStatus;
    int m_threadError;

    // modbus port options (shared between both threads)
    int     m_modbusMode;
    QString m_modbusComPort;
    int     m_modbusComBaud;
    int     m_modbusComParity;
    QString m_modbusTcpHost;
    int     m_modbusTcpPort;

    // modbus realtime data (shared between both threads)
    QList<modbusEndpointData> m_modbusEpDataList;
    QMutex m_modbusEpDataLock;

    void resetEpData();

    int  getEpDataCount();
    modbusEndpointData getEpDataAt(int index);

    void updateEpData(int index, uint16_t value);
    bool fetchUpdatedEpDataOut(modbusEndpointData *pEpDataOut, int *pEpIndex = nullptr);
    bool fetchUpdatedEpDataIn(modbusEndpointData *pEpDataIn, int *pEpIndex = nullptr);

    // process data input and output (in worker thread)
    void processEpDataOut();
    void processEpDataIn();

    void updateMetadata_() override;
};


class DgsModbusThread : public QThread
{
    Q_OBJECT
public:

    enum ThreadStatus {
        Standby = 0,
        Starting,
        Working,
        Stopping
    };

    explicit DgsModbusThread(QObject *parent) : QThread(parent) {
        m_interface = (DgsModbusInterface*)parent;
    }

private:
    DgsModbusInterface* m_interface;

    void run() {
        m_interface->run();
    }

};

#endif // DGSMODBUSINTERFACE_H
