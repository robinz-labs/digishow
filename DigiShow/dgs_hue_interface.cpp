/*
    Copyright 2021 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#include "dgs_hue_interface.h"
#include "digishow_environment.h"

#define HUE_OUT_FREQ 10

DgsHueInterface::DgsHueInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "hue";
    m_needUpdate = false;
    m_countUpdated = 0;
    m_maxUpdateEachPeriod = 1;
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

    // get hue bridge options
    QString serial = m_interfaceOptions.value("serial").toString();
    m_username = m_interfaceOptions.value("username").toString();
    m_bridgeIp = m_interfaceOptions.value("tcpHost").toString();

    // obtain hue bridge ip dynamically using upnp
    if (m_bridgeIp.isEmpty())
    for (int n=0 ; n<3 ; n++) {
        m_bridgeIp = AppUtilities::upnpWaitResponse("hue-bridgeid: " + serial, 2000).split("\r\n").at(0);
        if (!m_bridgeIp.isEmpty()) break;
    }

    bool done = (!m_bridgeIp.isEmpty());

    // reset light status list
    for (int n=0 ; n<HUE_MAX_LIGHT_NUMBER ; n++) m_lights[n] = hueLightInfo();
    for (int n=0 ; n<HUE_MAX_GROUP_NUMBER ; n++) m_groups[n] = hueLightInfo();
    m_needUpdate = false;
    m_countUpdated = 0;

    // set transitiontime for lights
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {

        if (m_endpointOptionsList[n].contains("optTransitionTime")) {

            int type = m_endpointInfoList[n].type;
            int channel = m_endpointInfoList[n].channel;
            int index = channel-1;

            if (type == ENDPOINT_HUE_LIGHT && index >= 0 && index < HUE_MAX_LIGHT_NUMBER) {

                m_lights[index].transitiontime = m_endpointOptionsList[n].value("optTransitionTime").toInt();

            } else
            if (type == ENDPOINT_HUE_GROUP && index >= 0 && index < HUE_MAX_GROUP_NUMBER) {

                 m_groups[index].transitiontime = m_endpointOptionsList[n].value("optTransitionTime").toInt();
            }
        }
    }

    // create a timer for sending http requests to the hue bridge at a particular frequency
    int frequency = m_interfaceOptions.value("frequency").toInt();
    if (frequency == 0) frequency = HUE_OUT_FREQ;

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setSingleShot(false);
    m_timer->setInterval(1000 / frequency);
    m_timer->start();

    //m_maxUpdateEachPeriod = m_timer->interval() / 10;

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

        if (m_countUpdated >= m_maxUpdateEachPeriod) break; // due to too many updates, cancel the rest

        if (buffer[n].needUpdate) {

            QVariantMap options;
            options["transitiontime"] = buffer[n].transitiontime;

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

            if (buffer[n].brightness == 0) {
                options.clear();
                options["on"] = false;
            } else {
                options["on"] = true;
            }

            int channel = n+1;
            int delay = m_countUpdated*5;
            callHueLightApi(type, channel, options, delay);

            buffer[n].needUpdate = false;
            m_countUpdated++;
        }
    }

    return;
}

void DgsHueInterface::onTimerFired()
{
    if (m_needUpdate) {

        m_countUpdated = 0;

        // update lights one by one with calling hue api
        updateLights(ENDPOINT_HUE_GROUP);
        updateLights(ENDPOINT_HUE_LIGHT);

        // confirm all lights are updated
        for (int n=0 ; n<HUE_MAX_GROUP_NUMBER ; n++) if (m_groups[n].needUpdate) return;
        for (int n=0 ; n<HUE_MAX_LIGHT_NUMBER ; n++) if (m_lights[n].needUpdate) return;

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

void DgsHueInterface::callHueLightApi(int type, int channel, const QVariantMap &options, int delay)
{
    HueLightWorker *worker = new HueLightWorker();
    worker->bridgeIp = m_bridgeIp;
    worker->username = m_username;
    worker->type = type;
    worker->channel = channel;
    worker->options = options;
    worker->delay = delay;
    worker->start();
}

void DgsHueInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_HUE;

    // Set interface mode and flags
    m_interfaceInfo.mode = INTERFACE_HUE_DEFAULT;
    m_interfaceInfo.output = true;
    m_interfaceInfo.input = false;

    // Set interface label
    m_interfaceInfo.label = tr("Hue") + " " + m_interfaceOptions.value("serial").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "light") endpointInfo.type = ENDPOINT_HUE_LIGHT;
        else if (typeName == "group") endpointInfo.type = ENDPOINT_HUE_GROUP;

        // Set endpoint properties
        endpointInfo.signal = DATA_SIGNAL_ANALOG;
        endpointInfo.output = true;
        endpointInfo.range  = (endpointInfo.control==CONTROL_LIGHT_HUE ||
                              endpointInfo.control==CONTROL_LIGHT_CT ? 65535 : 255);
        endpointInfo.labelEPT = (endpointInfo.type==ENDPOINT_HUE_GROUP ? tr("Group") : tr("Light")) +
                               " " + QString::number(endpointInfo.channel);
        endpointInfo.labelEPI = DigishowEnvironment::getLightControlName(endpointInfo.control);

        m_endpointInfoList.append(endpointInfo);
    }
}


void HueLightWorker::run()
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    // get ready for the http request
    QString url;
    if (this->type == ENDPOINT_HUE_LIGHT)
        url = QString("http://%1/api/%2/lights/%3/state").arg(this->bridgeIp).arg(this->username).arg(this->channel);
    else
    if (this->type == ENDPOINT_HUE_GROUP)
        url = QString("http://%1/api/%2/groups/%3/action").arg(this->bridgeIp).arg(this->username).arg(this->channel);
    else return;

    QString json = QJsonDocument::fromVariant(this->options).toJson();

    // delay for a while before sending
    if (this->delay > 0) QThread::msleep(delay);

    // send the http request
    AppUtilities::httpRequest(url, "put", json, 120);
}
