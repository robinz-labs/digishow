#include "tcp_handler.h"
#include "app_common.h"

#define TH_BUFFER_REC_SIZE 0x200000 // 2MBytes

TcpHandler::TcpHandler(QObject *parent) :
    QObject(parent)
{
    // init vars
    _tcpSocket = new QTcpSocket();
    _connected = false;
    _host[0] = 0;
    _port = 0;
    _isAsyncReceiver = true;

    // create buffer for receiving bytes
    _bufferReceived = (char*)malloc(TH_BUFFER_REC_SIZE + 1);
    _lboundReceived = 0;
    _uboundReceived = 0;

    _elapsedTimer.start();
}

TcpHandler::~TcpHandler()
{
    // force to close port
    close();
    _tcpSocket->close();
    delete _tcpSocket;

    // delete buffer for receiving bytes
    free(_bufferReceived);
}

bool TcpHandler::open(const char* host, int port, bool ignoreError)
{
    // if connection already exists
    if (_connected) {
        this->close();
        if (_connected) return false;
    }

    // connect to host
    _tcpSocket->connectToHost(host, port);
    _tcpSocket->waitForConnected(3000);

    _connected = (_tcpSocket->state()==QAbstractSocket::ConnectedState);
    if (ignoreError) _connected = true;

    if (_connected) {

        strcpy(_host, host);
        _port = port;

        _lboundReceived = 0; // clear buffer for receiving incoming bytes
        _uboundReceived = 0;

        if (_isAsyncReceiver) {
            connect(_tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
        }

        connect(_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(processError(QAbstractSocket::SocketError)));

        return true;
    }

    return false;
}

void TcpHandler::close()
{
    // disconnect from host
    if (_connected) {
        _tcpSocket->disconnectFromHost();
        if (_tcpSocket->state()!=QAbstractSocket::UnconnectedState)
            _tcpSocket->waitForDisconnected(3000);
    }

    // determine if connected
    _connected = !(_tcpSocket->state()==QAbstractSocket::UnconnectedState);

    // clear signal callbacks
    if (!_connected) _tcpSocket->disconnect();

    return;
}

void TcpHandler::setAsyncReceiver(bool isAsync)
{
    if (_connected &&
        _isAsyncReceiver != isAsync) {

        if (isAsync) {
            connect(_tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
        } else {
            disconnect(_tcpSocket, SIGNAL(readyRead()), 0, 0);
        }
    }

    _isAsyncReceiver = isAsync;
}


bool TcpHandler::sendBytes(const char* buffer, int length)
{
    if (_connected) {

        if (_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
            open(_host, _port);
        }

        if (_tcpSocket->state() == QAbstractSocket::UnconnectedState) return false;

        if (_tcpSocket->write(buffer, length) != -1) {
            _tcpSocket->flush();
            if (g_needLogCom) qDebug("SND: %s", QByteArray(buffer, length).toHex().constData());
            return true;
        }
    }
    return false;
}

int TcpHandler::numberOfReceivedBytes()
{
    return _uboundReceived - _lboundReceived;
}

char* TcpHandler::receivedBytes()
{
    return _bufferReceived + _lboundReceived;
}

int TcpHandler::getReceivedBytes(char* buffer, int length)
{
    int len = numberOfReceivedBytes();
    if (len > 0) {
        if (length < len) len = length;
        memcpy(buffer, receivedBytes(), len);
        _lboundReceived += len;

        if (_lboundReceived == _uboundReceived) {
            _lboundReceived = 0;
            _uboundReceived = 0;
        }

        return len;
    }
    return 0;
}

int TcpHandler::getReceivedDataFrame(char* buffer, int length, char leading, char ending)
{
    int lenReceived = numberOfReceivedBytes();
    char* bufReceived = receivedBytes();

    // find the first data frame in the received bytes
    int n;
    int posDataFrameBegin = -1;
    for (n=0 ; n<lenReceived-1 ; n++) {
        if (bufReceived[n]==leading) {
            posDataFrameBegin = n;
            break;
        }
    }

    int posDataFrameEnd = -1;
    if (posDataFrameBegin>=0) {
        for (n=posDataFrameBegin+1 ; n<lenReceived ; n++) {
            if (bufReceived[n]==ending) {
                posDataFrameEnd = n;
                break;
            }
        }
    }

    // obtain data frame from the received bytes
    // the obtained data frame contains leading and ending charaters
    if (posDataFrameBegin>=0 && posDataFrameEnd > posDataFrameBegin) {

        int lenDataFrame = posDataFrameEnd - posDataFrameBegin + 1;
        int len = lenDataFrame;

        if (length < len) len = length;
        memcpy(buffer, receivedBytes()+posDataFrameBegin, len);
        _lboundReceived += posDataFrameEnd+1;

        if (_lboundReceived == _uboundReceived) {
            _lboundReceived = 0;
            _uboundReceived = 0;
        }

        return len;

    }

    return 0;

}

int TcpHandler::getReceivedDataFrame(char* buffer, int length,
                                     const char* leading, const char* ending,
                                     int leadingLength, int endingLength)
{
    int lenReceived = numberOfReceivedBytes();
    char* bufReceived = receivedBytes();

    int lenLeading = leadingLength;
    if (leading!=NULL && lenLeading==0) lenLeading = (int)strlen(leading);

    int lenEnding = endingLength;
    if (ending!=NULL && lenEnding==0) lenEnding = (int)strlen(ending);

    if (lenReceived < lenLeading+lenEnding) return 0;

    // find the first data frame in the received bytes
    int n, m;

    int posDataFrameBegin = -1;
    if (lenLeading==0) {
        posDataFrameBegin = 0;
    } else {
        for (n=0 ; n<lenReceived-lenLeading-lenEnding+1 ; n++) {
            for (m=0 ; m<lenLeading ; m++) {
                if (bufReceived[n+m]!=leading[m]) break;
            }
            if (m==lenLeading) {
                posDataFrameBegin = n;
                break;
            }
        }
    }

    int posDataFrameEnd = -1;
    if (posDataFrameBegin>=0) {
        if (lenEnding==0) {
            posDataFrameEnd = lenReceived-1;
        } else {
            for (n=posDataFrameBegin+lenLeading ; n<lenReceived-lenEnding+1 ; n++) {
                for (m=0 ; m<lenEnding ; m++) {
                    if (bufReceived[n+m]!=ending[m]) break;
                }
                if (m==lenEnding) {
                    posDataFrameEnd = n+m-1;
                    break;
                }
            }
        }
    }

    // obtain data frame from the received bytes
    // the obtained data frame contains leading and ending charaters
    if (posDataFrameBegin>=0 && posDataFrameEnd > posDataFrameBegin) {

        int lenDataFrame = posDataFrameEnd - posDataFrameBegin + 1;
        int len = lenDataFrame;

        if (length < len) len = length;
        memcpy(buffer, receivedBytes()+posDataFrameBegin, len);
        _lboundReceived += posDataFrameEnd+1;

        if (_lboundReceived == _uboundReceived) {
            _lboundReceived = 0;
            _uboundReceived = 0;
        }

        return len;

    }

    return 0;

}

int TcpHandler::getReceivedFixedLengthFrame(char* buffer, int length, char leading)
{
    int lenReceived = numberOfReceivedBytes();
    char* bufReceived = receivedBytes();

    // find the first data frame in the received bytes
    int n;
    int posDataFrameBegin = -1;
    int posDataFrameEnd = -1;
    for (n=0 ; n<=lenReceived-length ; n++) {
        if (bufReceived[n]==leading) {
            posDataFrameBegin = n;
            posDataFrameEnd = n+length-1;
            break;
        }
    }

    // obtain data frame from the received bytes
    if (posDataFrameBegin>=0) {

        memcpy(buffer, receivedBytes()+posDataFrameBegin, length);
        _lboundReceived += posDataFrameEnd+1;

        if (_lboundReceived == _uboundReceived) {
            _lboundReceived = 0;
            _uboundReceived = 0;
        }

        return length;
    }

    return 0;
}

int TcpHandler::getReceivedFixedLengthFrame(char* buffer, int length, char leading, char ending)
{
    int lenReceived = numberOfReceivedBytes();
    char* bufReceived = receivedBytes();

    // find the first data frame in the received bytes
    int n;
    int posDataFrameBegin = -1;
    int posDataFrameEnd = -1;
    for (n=0 ; n<=lenReceived-length ; n++) {
        if (bufReceived[n]==leading && bufReceived[n+length-1]==ending) {
            posDataFrameBegin = n;
            posDataFrameEnd = n+length-1;
            break;
        }
    }

    // obtain data frame from the received bytes
    if (posDataFrameBegin>=0) {

        memcpy(buffer, receivedBytes()+posDataFrameBegin, length);
        _lboundReceived += posDataFrameEnd+1;

        if (_lboundReceived == _uboundReceived) {
            _lboundReceived = 0;
            _uboundReceived = 0;
        }

        return length;
    }

    return 0;
}

int TcpHandler::sendAndReceiveBytes(const char* bufSend, int lenBufSend,
                                      char* bufReceive, int lenBufReceive,
                                      const char* szEnding, double timeout,
                                      int* err)
{
  if (!_connected) return -1;

  if (_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
      open(_host, _port);
  }

  if (_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
      if (err!=NULL) *err=TCP_ERR_UNCONNECTED;
      return -1;
  }

  if (err!=NULL) *err=0;

  // drop incoming bytes before sending command
  int len;
  while(true) {
      _tcpSocket->waitForReadyRead(1);
      len = _tcpSocket->read(bufReceive, lenBufReceive);
      if (len<=0) break;
  }

  // send command bytes
  if (_tcpSocket->write(bufSend, lenBufSend) == -1) {
      if (err!=NULL) *err=TCP_ERR_SEND_FAILED;
      return -1;
  }
  _tcpSocket->flush();

  if (g_needLogCom) qDebug("SND: %s", QByteArray(bufSend, lenBufSend).toHex().constData());

  // receive response bytes
  int lenReceived = 0;
  double begin = getCurrentSecond();

  int n;

  bool isEnd;
  int lenEnding = 0;
  if (szEnding!=NULL) lenEnding = (int)strlen(szEnding);

  while(true) {
      _tcpSocket->waitForReadyRead(50);
      len = _tcpSocket->read(bufReceive+lenReceived, lenBufReceive-lenReceived);
      if (len>0) {

          if (g_needLogCom) qDebug("REC: %s", QByteArray(bufReceive+lenReceived, len).toHex().constData());

          lenReceived += len;
      }

      // ending characters checking
      if (lenEnding>0 &&
          lenReceived >= lenEnding) {

          isEnd = true;
          for (n=0;n<lenEnding;n++) {
              if (bufReceive[lenReceived-lenEnding+n] != szEnding[n]) {
                  isEnd = false;
                  break;
              }
          }
          if (isEnd) break;
      }

      // timeout checking
      double now = getCurrentSecond();
      if (now-begin>timeout) {
          if (err!=NULL) *err=TCP_ERR_RECEIVE_TIMEOUT;
          break;
      }

      // buffer is full
      if (lenBufReceive == lenReceived) {
          if (err!=NULL) *err=TCP_ERR_RECEIVE_BUFFULL;
          break;
      }
  }

  return lenReceived;
}

void TcpHandler::readData()
{
    if (!_connected) return;

    // calculate unused buffer to receive incoming bytes
    int unusedBufferSize = TH_BUFFER_REC_SIZE - _uboundReceived;
    if (unusedBufferSize <= 0) {
        if (_uboundReceived - _lboundReceived > 0 &&
            _lboundReceived != 0) {

            // no enough space, then move received bytes in the buffer to obtain more free space
            // before: ........XXX
            // after:  XXX........

            int lenReceived = _uboundReceived - _lboundReceived;
            memcpy(_bufferReceived, _bufferReceived + _lboundReceived, lenReceived);
            _lboundReceived = 0;
            _uboundReceived = lenReceived;
            unusedBufferSize = TH_BUFFER_REC_SIZE - _uboundReceived;

        } else {

            // buffer is full, then have to empty all (unexceptional)
            // before: XXXXXXXXXXX
            // after:  ...........

            _lboundReceived = 0;
            _uboundReceived = 0;
            unusedBufferSize = TH_BUFFER_REC_SIZE;
        }
    }

    // read bytes from device
    int len = _tcpSocket->read(_bufferReceived + _uboundReceived, unusedBufferSize);
    //qDebug("unused = %d, len = %d", unusedBufferSize, (int)len);

    if (len>0) {

        if (g_needLogCom) qDebug("REC: %s", QByteArray(_bufferReceived + _uboundReceived, len).toHex().constData());

        _uboundReceived += len;
        _bufferReceived[_uboundReceived]=0; // make the buffer as a zero-terminated string

        //qDebug("received length = %d (%d ~ %d)", numberOfReceivedBytes(), _lboundReceived, _uboundReceived);
        //qDebug("received length = %d (%d ~ %d) %s", numberOfReceivedBytes(), _lboundReceived, _uboundReceived, _bufferReceived + _uboundReceived - len);

        emit bytesReceived();
    }
}

void TcpHandler::processError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    //qDebug("tcp socket error %d", socketError);
}

double TcpHandler::getCurrentSecond()
{
    return _elapsedTimer.elapsed() * 0.001;
}

