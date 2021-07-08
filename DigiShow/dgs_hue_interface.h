#ifndef DGSHUEINTERFACE_H
#define DGSHUEINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

#define HUE_MAX_LIGHT_NUMBER 64
#define HUE_MAX_GROUP_NUMBER 64


typedef struct hueLightInfo {

    unsigned char colorR;
    unsigned char colorG;
    unsigned char colorB;
    QVariantList colorXY;

    int brightness;
    int saturation;
    int hue;
    int ct;

    bool needUpdate;

    // defaults
    hueLightInfo() :
      colorR(0),
      colorG(0),
      colorB(0),
      colorXY(QVariantList()),
      brightness(-1),
      saturation(-1),
      hue(-1),
      ct(-1),
      needUpdate(false)
    {}

} hueLightInfo;

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
    void onTimerFired();

private:
    QString m_bridgeIp;
    QString m_username;

    QTimer *m_timer;

    hueLightInfo m_lights[HUE_MAX_LIGHT_NUMBER];
    hueLightInfo m_groups[HUE_MAX_GROUP_NUMBER];
    bool m_needUpdate;

    void updateLights(int type);
    void callHueLightApi(int type, int channel, const QVariantMap &options);
    static bool convertRgbToXy(uint8_t r, uint8_t g, uint8_t b, float *px, float *py);
};

class HueLightWorker : public QThread
{
    Q_OBJECT

    void run() override;

public:
    QString bridgeIp;
    QString username;
    int type;
    int channel;
    QVariantMap options;
};

#endif // DGSHUEINTERFACE_H
