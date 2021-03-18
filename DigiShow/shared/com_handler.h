#ifndef COM_HANDLER_H
#define COM_HANDLER_H

#include <QObject>
#include <QSerialPort>
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

    void setAsyncReceiver(bool isAsync);

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

private:

    QSerialPort* _serial;
    bool _connected;

    bool _isAsyncReceiver;      // asynchronous receiver always buffer the incoming bytes all the time,
                                // and uses getReceivedBytes() to read the data in buffer later

    char* _bufferReceived;      // pointer to the buffer for storing received bytes
    int _lboundReceived;        // the beginning postion of the received bytes in the buffer
    int _uboundReceived;        // the end postion of the received bytes in the buffer

    QElapsedTimer _elapsedTimer;
    double getCurrentSecond();
};


#endif // COM_HANDLER_H
