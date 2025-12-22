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

#ifndef COM_HANDLER_H
#define COM_HANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QElapsedTimer>
#include <inttypes.h>

#define CH_SETTING_8N1 0
#define CH_SETTING_8E1 1
#define CH_SETTING_8O1 2
#define CH_SETTING_7E1 3
#define CH_SETTING_7O1 4
#define CH_SETTING_8N2 5

#define COM_ERR_UNCONNECTED      1
#define COM_ERR_SEND_FAILED      10
#define COM_ERR_RECEIVE_FAILED   20
#define COM_ERR_RECEIVE_TIMEOUT  21
#define COM_ERR_RECEIVE_CHKERR   22
#define COM_ERR_RECEIVE_BUFFULL  23

class ComHandler : public QObject
{
    Q_OBJECT
public:
    explicit ComHandler(QObject *parent = 0);
    ~ComHandler();

    QSerialPort* serialPort() { return _serial; }

    bool open(const char* port, int baud = 9600, int setting = CH_SETTING_8N1);
    void close();

    bool isConnected() { return _connected; }

    void setAsyncReceiver(bool isAsync);
    void setAutoReconnect(bool enabled);

    // asynchronize receiver
    bool sendBytes(const char* buffer, int length, bool flush = true);
    bool isBusySending() { return _serial->bytesToWrite()>0; }

    int numberOfReceivedBytes();
    char* receivedBytes();
    int getReceivedBytes(char* buffer, int length);
    int getReceivedDataFrame(char* buffer, int length, char leading, char ending);
    int getReceivedDataFrame(char* buffer, int length,
                             const char* leading, const char* ending,
                             int leadingLength = 0, int endingLength = 0);
    int getReceivedFixedLengthFrame(char* buffer, int length, char leading);
    int getReceivedFixedLengthFrame(char* buffer, int length, char leading, char ending);

    // synchronize receiver
    int sendAndReceiveBytes(const char* bufSend, int lenBufSend,
                            char* bufReceive, int lenBufReceive,
                            const char* szEnding = NULL,
                            double timeout = 1.5,
                            int* err = NULL);

    void checkReceivedBytes();

    // find port by vid/pid
    static QString findPort(uint32_t vid, uint32_t pid,
                            const char* description = NULL,
                            const char* manufacturer = NULL);

signals:
    void bytesReceived(ComHandler* com = NULL);

private slots:
    void readData();
    void handleSerialError(QSerialPort::SerialPortError error);
    void tryReconnect();

private:

    QSerialPort* _serial;
    bool _connected;

    bool _isAsyncReceiver;      // asynchronous receiver always buffer the incoming bytes all the time,
                                // and uses getReceivedBytes() to read the data in buffer later

    char* _bufferReceived;      // pointer to the buffer for storing received bytes
    int _lboundReceived;        // the beginning position of the received bytes in the buffer
    int _uboundReceived;        // the end position of the received bytes in the buffer

    QString _serialPort;        // serial connection parameters
    int _serialBaud;            // for port reconnection
    int _serialSetting;

    bool _isAutoReconnectEnabled;  // flag that enables automatic serial port reconnection upon link loss
    QTimer* _timerTryReconnect;    // timer that polls the serial port status and handles reconnection
    double _timeLastTryReconnect;

    QElapsedTimer _elapsedTimer;
    double getCurrentSecond();

};


#endif // COM_HANDLER_H
