#include <QThread>
#include "rioc_service.h"
#include "com_handler.h"
#include "app_common.h"

#define SERIAL_SETTING CH_SETTING_8N1

RiocService::RiocService(QObject *parent) : QObject(parent)
{
    _isLoggerEnanbled = true;

    _isSpecificIncomingMessageReceived = true;
    _specificIncomingMessageFromID = 0;
    _specificIncomingMessageFilter.clear();
    _specificIncomingMessageReceived.clear();

    _elapsedTimer.start();
}

RiocService::~RiocService()
{
    this->clearSerialConnections();
}

bool RiocService::addSerialConnection(const QString & serialPort, int serialBaud)
{
    ComHandler* serial = new ComHandler(this);
    serial->setAsyncReceiver(true);
    if (!serial->open(serialPort.toLocal8Bit().constData(), serialBaud, SERIAL_SETTING)) {
        qWarning("[ERR] failed to connect RIOC via %s (%d).",
                 serialPort.toLocal8Bit().constData(), serialBaud);
        delete serial;
        return false;
    }

    connect(serial, SIGNAL(bytesReceived(ComHandler*)), this, SLOT(handleSerialBytesReceived(ComHandler*)));
    _serials.append(serial);
    qDebug("[OK] connected RIOC via %s (%d).",
          serialPort.toLocal8Bit().constData(), serialBaud);
    return true;
}

void RiocService::clearSerialConnections()
{
    for (int n=0 ; n<_serials.size() ; n++) {
        _serials[n]->close();
        delete _serials[n];
    }
    _serials.clear();
}

int RiocService::getSerialCount()
{
    return _serials.size();
}

void RiocService::sendSerialMessage(const QByteArray & message)
{
    if (message.length()<2) return;

    QByteArray data = message;
    unsigned char crc = checksum((unsigned char*)(data.constData())+1, data.length()-1);

    data.insert(0, '\x5A'); // leading character
    data.append(crc);     // checksum
    data.append('\xA5');    // ending character

    for (int n=0 ; n<_serials.size() ; n++) {
        _serials[n]->sendBytes(data.constData(), data.length());
    }
}

void RiocService::sendRiocMessage(unsigned char fromID, unsigned char toID, const QByteArray & data)
{
    if (data.length()!=8) return; // data length must be 8

    const char bytes[] = {
        0x00,         // frame version
        (char)fromID, // source address
        (char)toID,   // destination address
        data.at(0),   // 8-byte data
        data.at(1),
        data.at(2),
        data.at(3),
        data.at(4),
        data.at(5),
        data.at(6),
        data.at(7)
    };
    QByteArray datagram(bytes, sizeof(bytes));

    if (_isLoggerEnanbled)
        qDebug() << "-->" << datagram.toHex();

    //QThread::msleep(30);
    sendSerialMessage(datagram);
}

bool RiocService::sendRiocMessageAndWaitResponse(unsigned char fromID, unsigned char toID,
                                                 const QByteArray & dataOut, QByteArray & dataIn,
                                                 double timeout)
{
    // specify the message response we want to receive
    _isSpecificIncomingMessageReceived = false;
    _specificIncomingMessageFromID = toID;

    unsigned char rspFunctionCode = (unsigned char)dataOut.at(1) | 0x80;
    const char filter[] = {
        dataOut.at(0),          // class
        (char)rspFunctionCode,  // function
        dataOut.at(2)           // channel
    };
    _specificIncomingMessageFilter = QByteArray(filter, sizeof(filter));

    // send message
    sendRiocMessage(fromID, toID, dataOut);

    // wait and receive response
    double begin = getCurrentSecond();
    while (!_isSpecificIncomingMessageReceived && getCurrentSecond()-begin<timeout) {
        for (int n=0 ; n<_serials.size() ; n++) {
            _serials[n]->checkReceivedBytes(); // 处理所有接收到的数据（功能与异步接收相同）
        }
        //delay(50);
    }

    if (_isSpecificIncomingMessageReceived) {
        dataIn = _specificIncomingMessageReceived;
        return true;
    }

    _isSpecificIncomingMessageReceived = true;
    return false;
}

void RiocService::handleSerialBytesReceived(ComHandler* serial)
{
    //qDebug("buffer size = %d", _serial->numberOfReceivedBytes());

    char buf[14];
    int len;
    while ((len = serial->getReceivedFixedLengthFrame(buf, sizeof(buf), '\x5A', '\xA5')) >0) {

        if (len<5) continue; // length of dataframe is too short

        QByteArray datagram(buf, len);
        unsigned char crc = checksum((unsigned char*)buf+2, len-4);
        if ((unsigned char)datagram.at(len-2)!=crc &&
            (unsigned char)datagram.at(len-2)!=0xFF) continue; // crc error

        datagram.remove(0, 1);                   // get rid of the heading character
        datagram.remove(datagram.length()-2, 2); // get rid of the crc and ending characters

        // process the incoming rioc message
        if (datagram.length()==11 && datagram.at(0)==0x00) {

            if (_isLoggerEnanbled)
                qDebug() << "<--" << datagram.toHex();

            unsigned char fromID = datagram.at(1);
            unsigned char toID = datagram.at(2);

            unsigned char bytes[8];
            for (int n=0 ; n<8 ; n++) bytes[n]= datagram.at(n+3);
            QByteArray data((const char*)bytes, sizeof(bytes));

            emit riocMessageReceived(fromID, toID, data);

            // determine a specific message received
            if (!_isSpecificIncomingMessageReceived &&
                _specificIncomingMessageFromID == fromID &&
                data.startsWith(_specificIncomingMessageFilter)) {

                _specificIncomingMessageReceived = data;
                _isSpecificIncomingMessageReceived = true;
            }
        }
    }
}

double RiocService::getCurrentSecond()
{
    return _elapsedTimer.elapsed() * 0.001;
}

// make CRC checksum
unsigned char RiocService::checksum(unsigned char *bytes, int length)
{
  unsigned char checksum = 0;
  for (int n=0 ; n<length ; n++) {
    checksum += bytes[n];
  }
  return ~checksum + 1;
}


