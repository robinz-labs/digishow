#ifndef DGSARTNETINTERFACE_H
#define DGSARTNETINTERFACE_H

#include <QObject>
#include <QUdpSocket>
#include "digishow_interface.h"

class DgsArtnetInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsArtnetInterface(QObject *parent = nullptr);
    ~DgsArtnetInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

signals:

public slots:
    void onUdpDataReceived(); // for input
    void onTimerFired();      // for output

private:

    QUdpSocket *m_udp;
    QHostAddress m_udpHost;
    int m_udpPort;

    // timer for artnet dmx output
    QTimer *m_timer;

    // buffer for dmx data of all universes
    // key: universe number
    // val: 512 bytes of dmx data
    QMap<int, QByteArray> m_dataAll;

    // sequence number for artnet dmx output ( 0x01 ~ 0xff )
    unsigned char m_sequence;
};

#endif // DGSARTNETINTERFACE_H
