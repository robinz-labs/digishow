#include "dgs_hue_interface.h"

#define HUE_OUT_FREQ 8

DgsHueInterface::DgsHueInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "hue";
    m_needUpdate = false;
    m_timer = nullptr;
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

    // reset light status list
    for (int n=0 ; n<HUE_MAX_LIGHT_NUMBER ; n++) m_lights[n] = hueLightInfo();
    for (int n=0 ; n<HUE_MAX_GROUP_NUMBER ; n++) m_groups[n] = hueLightInfo();
    m_needUpdate = false;

    // create a timer for sending http requests to the hue bridge at a particular frequency
    int frequency = m_interfaceOptions.value("frequency").toInt();
    if (frequency == 0) frequency = HUE_OUT_FREQ;

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setSingleShot(false);
    m_timer->setInterval(1000 / frequency);
    m_timer->start();


    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsHueInterface::closeInterface()
{
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsHueInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_ANALOG) return ERR_INVALID_DATA;

    int type    = m_endpointInfoList[endpointIndex].type;
    int channel = m_endpointInfoList[endpointIndex].channel;
    int control = m_endpointInfoList[endpointIndex].control;

    int maxChannelNumber;
    hueLightInfo* buffer;

    if (type == ENDPOINT_HUE_LIGHT) {

        maxChannelNumber = HUE_MAX_LIGHT_NUMBER;
        buffer = m_lights;

    } else if (type == ENDPOINT_HUE_GROUP) {

        maxChannelNumber = HUE_MAX_GROUP_NUMBER;
        buffer = m_groups;

    } else return ERR_INVALID_OPTION;

    int range = (control == CONTROL_LIGHT_HUE || control == CONTROL_LIGHT_CT ? 65535 : 255);
    int value = range * data.aValue / (data.aRange==0 ? range : data.aRange);
    if (value<0 || value>range) return ERR_INVALID_DATA;

    QVariantMap options;
    options["transitiontime"] = 2; // 200 milliseconds

    if (channel < 1 || channel > maxChannelNumber) return ERR_INVALID_OPTION;
    int i = channel-1;

    if (control == CONTROL_LIGHT_R || control == CONTROL_LIGHT_G || control == CONTROL_LIGHT_B) {

        switch (control) {
        case CONTROL_LIGHT_R: buffer[i].colorR = value; break;
        case CONTROL_LIGHT_G: buffer[i].colorG = value; break;
        case CONTROL_LIGHT_B: buffer[i].colorB = value; break;
        }

        float x = 0, y = 0;
        if (convertRgbToXy(buffer[i].colorR, buffer[i].colorG, buffer[i].colorB, &x, &y)) {

            buffer[i].colorXY = { x, y };
            buffer[i].needUpdate = true;
            m_needUpdate = true;
        }

    } else if (control == CONTROL_LIGHT_BRI) {

        buffer[i].brightness = value;
        buffer[i].needUpdate = true;
        m_needUpdate = true;

    } else if (control == CONTROL_LIGHT_SAT) {

        buffer[i].saturation = value;
        buffer[i].needUpdate = true;
        m_needUpdate = true;

    } else if (control == CONTROL_LIGHT_HUE) {

        buffer[i].hue = value;
        buffer[i].needUpdate = true;
        m_needUpdate = true;

    } else if (control == CONTROL_LIGHT_CT) {

        buffer[i].ct = value;
        buffer[i].needUpdate = true;
        m_needUpdate = true;
    }

    return ERR_NONE;
}

void DgsHueInterface::updateLights(int type) {

    int maxChannelNumber;
    hueLightInfo* buffer;

    if (type == ENDPOINT_HUE_LIGHT) {

        maxChannelNumber = HUE_MAX_LIGHT_NUMBER;
        buffer = m_lights;

    } else if (type == ENDPOINT_HUE_GROUP) {

        maxChannelNumber = HUE_MAX_GROUP_NUMBER;
        buffer = m_groups;

    } else return;

    for (int n=0 ; n<maxChannelNumber ; n++) {
        if (buffer[n].needUpdate) {

            QVariantMap options;

            options["transitiontime"] = 2; // 200 milliseconds

            options["on"] = true;
            if (buffer[n].brightness == 0) {
                options["on"] = false;
            } else if (buffer[n].brightness > 0) {
                options["on"] = true;
            }

            if (!buffer[n].colorXY.isEmpty()) {
                options["xy"] = buffer[n].colorXY;
                options["bri"] = 254;
                options["sat"] = 254;
            }

            if (buffer[n].brightness >= 0) {
                options["bri"] = buffer[n].brightness;
            }

            if (buffer[n].saturation >= 0) {
                options["sat"] = buffer[n].saturation;
            }

            if (buffer[n].hue >= 0) {
                options["hue"] = buffer[n].hue;
            }

            if (buffer[n].ct >= 0) {
                options["ct"] = buffer[n].ct;
            }

            int channel = n+1;
            callHueLightApi(type, channel, options);

            buffer[n].needUpdate = false;
        }
    }

}


void DgsHueInterface::onTimerFired()
{
    if (m_needUpdate) {
        updateLights(ENDPOINT_HUE_LIGHT);
        updateLights(ENDPOINT_HUE_GROUP);
        m_needUpdate = false;
    }
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

void DgsHueInterface::callHueLightApi(int type, int channel, const QVariantMap &options)
{
    HueLightWorker *worker = new HueLightWorker();
    worker->bridgeIp = m_bridgeIp;
    worker->username = m_username;
    worker->type = type;
    worker->channel = channel;
    worker->options = options;
    worker->start();
}

void HueLightWorker::run()
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    QString url;
    if (this->type == ENDPOINT_HUE_LIGHT)
        url = QString("http://%1/api/%2/lights/%3/state").arg(this->bridgeIp).arg(this->username).arg(this->channel);
    else
    if (this->type == ENDPOINT_HUE_GROUP)
        url = QString("http://%1/api/%2/groups/%3/action").arg(this->bridgeIp).arg(this->username).arg(this->channel);

    QString json = QJsonDocument::fromVariant(this->options).toJson();

    if (!url.isEmpty()) AppUtilities::httpRequest(url, "put", json, 120);
}
