#include "dgs_hue_interface.h"

DgsHueInterface::DgsHueInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "hue";
}

DgsHueInterface::~DgsHueInterface()
{
    closeInterface();
}

int DgsHueInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get hue bridge ip
    QString serial = m_interfaceOptions.value("serial").toString();
    m_username = m_interfaceOptions.value("username").toString();

    for (int n=0 ; n<3 ; n++) {
        m_bridgeIp = AppUtilities::upnpWaitResponse("hue-bridgeid: " + serial, 2000).split("\r\n").at(0);
        if (!m_bridgeIp.isEmpty()) break;
    }

    bool done = (!m_bridgeIp.isEmpty());

    for (int n=0 ; n<512 ; n++) {
        m_lightR[n] = 0;
        m_lightG[n] = 0;
        m_lightB[n] = 0;
    }

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsHueInterface::closeInterface()
{
    m_isInterfaceOpened = false;
    return ERR_NONE;

}

int DgsHueInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // confirm it's a light endpoint
    if (m_endpointInfoList[endpointIndex].type != ENDPOINT_HUE_LIGHT) return ERR_INVALID_OPTION;

    if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

    int channel = m_endpointInfoList[endpointIndex].channel;
    int control = m_endpointInfoList[endpointIndex].control;

    int range = (control == CONTROL_LIGHT_HUE || control == CONTROL_LIGHT_CT ? 65535 : 255);
    int value = range * data.aValue / (data.aRange==0 ? range : data.aRange);
    if (value<0 || value>range) return ERR_INVALID_DATA;

    QVariantMap options;
    options["transitiontime"] = 2; // 200 milliseconds

    if (control == CONTROL_LIGHT_R || control == CONTROL_LIGHT_G || control == CONTROL_LIGHT_B) {

        int i = channel-1;
        switch (control) {
        case CONTROL_LIGHT_R: m_lightR[i] = uint8_t(value); break;
        case CONTROL_LIGHT_G: m_lightG[i] = uint8_t(value); break;
        case CONTROL_LIGHT_B: m_lightB[i] = uint8_t(value); break;
        }

        float x = 0, y = 0;
        if (convertRgbToXy(m_lightR[i], m_lightG[i], m_lightB[i], &x, &y)) {

            QVariantList xy = { x, y };
            options["xy"] = xy;
            options["bri"] = 254;
            options["sat"] = 254;
            options["on"] = true;
            callHueLightApi(channel, options);

        } else {

            options["on"] = false;
            callHueLightApi(channel, options);
        }

    } else if (control == CONTROL_LIGHT_BRI) {

        if (value > 0) {

            options["bri"] = value;
            options["on"] = true;
            callHueLightApi(channel, options);

        } else {

            options["on"] = false;
            callHueLightApi(channel, options);
        }

    } else if (control == CONTROL_LIGHT_SAT) {

            options["sat"] = value;
            options["on"] = true;
            callHueLightApi(channel, options);

    } else if (control == CONTROL_LIGHT_HUE) {

            options["hue"] = value;
            options["on"] = true;
            callHueLightApi(channel, options);

    } else if (control == CONTROL_LIGHT_CT) {

            options["ct"] = value;
            options["on"] = true;
            callHueLightApi(channel, options);
    }

    return ERR_NONE;

}


QVariantList DgsHueInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    return list;
}

bool DgsHueInterface::convertRgbToXy(uint8_t r, uint8_t g, uint8_t b, float *px, float *py)
{
    if ((r == 0) && (g == 0) && (b == 0)) return false;

    const float red = float(r) / 255;
    const float green = float(g) / 255;
    const float blue = float(b) / 255;

    // gamma correction
    const float redCorrected = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
    const float greenCorrected = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
    const float blueCorrected = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

    const float X = redCorrected * 0.664511f + greenCorrected * 0.154324f + blueCorrected * 0.162028f;
    const float Y = redCorrected * 0.283881f + greenCorrected * 0.668433f + blueCorrected * 0.047685f;
    const float Z = redCorrected * 0.000088f + greenCorrected * 0.072310f + blueCorrected * 0.986039f;

    *px = X / (X + Y + Z);
    *py = Y / (X + Y + Z);

    return true;
}

void DgsHueInterface::callHueLightApi(int channel, const QVariantMap &options)
{
    HueLightWorker *worker = new HueLightWorker();
    worker->bridgeIp = m_bridgeIp;
    worker->username = m_username;
    worker->channel = channel;
    worker->options = options;
    worker->start();
}

void HueLightWorker::run()
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    QString url  = QString("http://%1/api/%2/lights/%3/state").arg(this->bridgeIp).arg(this->username).arg(this->channel);
    QString json = QJsonDocument::fromVariant(this->options).toJson();

    AppUtilities::httpRequest(url, "put", json, 120);
}
