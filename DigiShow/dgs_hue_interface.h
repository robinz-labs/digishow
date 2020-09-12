#ifndef DGSHUEINTERFACE_H
#define DGSHUEINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class ComHandler;

class DgsHueInterface : public DigishowInterface
{
    Q_OBJECT

public:
    explicit DgsHueInterface(QObject *parent = nullptr);
    ~DgsHueInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();

signals:

public slots:

private:
    QString m_bridgeIp;
    QString m_username;

    uint8_t m_lightR[512];
    uint8_t m_lightG[512];
    uint8_t m_lightB[512];

    void callHueLightApi(int channel, const QVariantMap &options);
    static bool convertRgbToXy(uint8_t r, uint8_t g, uint8_t b, float *px, float *py);
};

class HueLightWorker : public QThread
{
    Q_OBJECT

    void run() override;

public:
    QString bridgeIp;
    QString username;
    int channel;
    QVariantMap options;
};

#endif // DGSHUEINTERFACE_H
