#ifndef DGSDMXINTERFACE_H
#define DGSDMXINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class ComHandler;

class DgsDmxInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsDmxInterface(QObject *parent = nullptr);
    ~DgsDmxInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();

signals:

public slots:
    void onTimerFired();

private:
    ComHandler *m_com;
    QTimer *m_timer;

    int m_channels; // number of channels
    unsigned char m_data[512];

    bool enttecDmxOpen(const QString &port, int channels = 512);
    bool enttecDmxSendDmxFrame(unsigned char *data);
};

#endif // DGSDMXINTERFACE_H
