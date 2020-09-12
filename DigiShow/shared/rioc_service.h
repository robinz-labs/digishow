#ifndef RIOCSERVICE_H
#define RIOCSERVICE_H

#include <QtCore>
#include <QElapsedTimer>

class ComHandler;

class RiocService : public QObject
{
    Q_OBJECT
public:
    explicit RiocService(QObject *parent = 0);
    virtual ~RiocService();

    void setLoggerEnabled(bool enable) { _isLoggerEnanbled = enable; }

    bool addSerialConnection(const QString & serialPort, int serialBaud = 9600);
    void clearSerialConnections();
    int getSerialCount();

    void sendRiocMessage(unsigned char fromID, unsigned char toID, const QByteArray & data);
    bool sendRiocMessageAndWaitResponse(unsigned char fromID, unsigned char toID,
                                        const QByteArray & dataOut, QByteArray & dataIn,
                                        double timeout = 0.6);

signals:
    void riocMessageReceived(unsigned char fromID, unsigned char toID, const QByteArray & data);

private slots:
    void handleSerialBytesReceived(ComHandler* serial);

private:
    bool _isLoggerEnanbled;
    QList<ComHandler*> _serials;

    void sendSerialMessage(const QByteArray & message);

    // for receiving specific message
    bool _isSpecificIncomingMessageReceived;
    unsigned char _specificIncomingMessageFromID;
    QByteArray _specificIncomingMessageFilter;
    QByteArray _specificIncomingMessageReceived;

    QElapsedTimer _elapsedTimer;
    double getCurrentSecond();

    unsigned char checksum(unsigned char *bytes, int length);
};

#endif // RIOCSERVICE_H
