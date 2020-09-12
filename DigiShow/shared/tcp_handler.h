#ifndef TCP_HANDLER_H
#define TCP_HANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QElapsedTimer>

#define TCP_ERR_UNCONNECTED      1
#define TCP_ERR_SEND_FAILED      10
#define TCP_ERR_RECEIVE_FAILED   20
#define TCP_ERR_RECEIVE_TIMEOUT  21
#define TCP_ERR_RECEIVE_CHKERR   22
#define TCP_ERR_RECEIVE_BUFFULL  23

class TcpHandler : public QObject
{
    Q_OBJECT
public:
    explicit TcpHandler(QObject *parent = 0);
    ~TcpHandler();

    bool open(const char* host, int port, bool ignoreError = true);
    void close();

    QString host() { return _tcpSocket->peerName(); }
    int port() { return _tcpSocket->peerPort(); }

    void setAsyncReceiver(bool isAsync);

    bool isConnected() { return _connected; }

    // asynchronize receiver
    bool sendBytes(const char* buffer, int length);
    bool isBusySending() { return _tcpSocket->bytesToWrite()>0; }

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

signals:
    void bytesReceived();

private slots:
    void readData();
    void processError(QAbstractSocket::SocketError socketError);

private:

    QTcpSocket* _tcpSocket;
    bool _connected;
    char _host[512];
    int _port;

    bool _isAsyncReceiver;      // asynchronous receiver always buffer the incoming bytes all the time,
                                // and uses getReceivedBytes() to read the data in buffer later

    char* _bufferReceived;      // pointer to the buffer for storing received bytes
    int _lboundReceived;        // the beginning postion of the received bytes in the buffer
    int _uboundReceived;        // the end postion of the received bytes in the buffer

    QElapsedTimer _elapsedTimer;
    double getCurrentSecond();

};

#endif // TCP_HANDLER_H
