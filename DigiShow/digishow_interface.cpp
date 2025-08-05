/*
    Copyright 2021-2025 Robin Zhang & Labs

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

#include "digishow_interface.h"
#include "digishow_environment.h"

DigishowInterface::DigishowInterface(QObject *parent) : QObject(parent)
{
    m_isInterfaceOpened = false;
    m_needReceiveRawData = false;
    m_interfaceInfo = dgsInterfaceInfo();

#ifdef QT_DEBUG
    qDebug() << "interface created" << this;
#endif
}

DigishowInterface::~DigishowInterface()
{

#ifdef QT_DEBUG
    qDebug() << "interface released" << this;
#endif
}

int DigishowInterface::setInterfaceOptions(const QVariantMap &options)
{
    m_interfaceOptions = options;
    return ERR_NONE;
}

int DigishowInterface::setInterfaceOption(const QString &name, const QVariant &value)
{
    if (value.isNull())
        m_interfaceOptions.remove(name);
    else
        m_interfaceOptions[name] = value;

    return ERR_NONE;
}

QVariantMap DigishowInterface::getInterfaceInfo()
{
    QVariantMap info;

    info["type"  ] = m_interfaceInfo.type;
    info["mode"  ] = m_interfaceInfo.mode;
    info["output"] = m_interfaceInfo.output;
    info["input" ] = m_interfaceInfo.input;
    info["label" ] = m_interfaceInfo.label;

    return info;
}

int DigishowInterface::addEndpoint(const QVariantMap &options, int *endpointIndex)
{
    m_endpointOptionsList.append(options);
    if (endpointIndex != nullptr) *endpointIndex = m_endpointOptionsList.length()-1;
    return ERR_NONE;
}

int DigishowInterface::setEndpointOptions(int endpointIndex, const QVariantMap &options)
{
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    m_endpointOptionsList[endpointIndex] = options;
    return ERR_NONE;
}

int DigishowInterface::setEndpointOption(int endpointIndex, const QString &name, const QVariant &value)
{
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    m_endpointOptionsList[endpointIndex][name] = value;
    return ERR_NONE;
}

int DigishowInterface::endpointCount()
{
    return m_endpointOptionsList.length();
}

QVariantMap DigishowInterface::getEndpointOptionsAt(int index)
{
    if (index>=0 && index<endpointCount()) return m_endpointOptionsList[index];
    return QVariantMap();
}

QVariantMap DigishowInterface::getEndpointInfoAt(int index)
{
    QVariantMap info;
    if (index>=0 && index<endpointCount()) {

        info["type"    ] = m_endpointInfoList[index].type;
        info["unit"    ] = m_endpointInfoList[index].unit;
        info["channel" ] = m_endpointInfoList[index].channel;
        info["note"    ] = m_endpointInfoList[index].note;
        info["control" ] = m_endpointInfoList[index].control;
        info["signal"  ] = m_endpointInfoList[index].signal;
        info["output"  ] = m_endpointInfoList[index].output;
        info["input"   ] = m_endpointInfoList[index].input;
        info["range"   ] = m_endpointInfoList[index].range;
        info["ready"   ] = m_endpointInfoList[index].ready;
        info["initial" ] = m_endpointInfoList[index].initial;

        info["address" ] = m_endpointInfoList[index].address;

        info["labelEPT"] = m_endpointInfoList[index].labelEPT;
        info["labelEPI"] = m_endpointInfoList[index].labelEPI;
    }

    return info;
}

int DigishowInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;
    updateMetadata();
    m_needReceiveRawData = false;
    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DigishowInterface::closeInterface()
{
    m_needReceiveRawData = false;
    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DigishowInterface::init()
{
    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;

    // set initial value for all endpoints
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) initEndpointValue(n);
    return ERR_NONE;
}

int DigishowInterface::sendData(int endpointIndex, dgsSignalData data)
{
    Q_UNUSED(data)

    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;
    if (!m_endpointInfoList[endpointIndex].enabled) return ERR_DEVICE_NOT_READY;

    emit dataSent(endpointIndex, data);

    return ERR_NONE;
}

int DigishowInterface::loadMedia(const QVariantMap &mediaOptions)
{
    Q_UNUSED(mediaOptions)

    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;

    return ERR_NONE;
}

void DigishowInterface::updateMetadata()
{
    // reset metadata first
    m_interfaceInfo = dgsInterfaceInfo();
    m_endpointInfoList.clear();

    // the implementation for the specific interface
    updateMetadata_();

    // remove the space at the end of the label
    m_interfaceInfo.label = m_interfaceInfo.label.trimmed();

    emit metadataUpdated();
}

dgsEndpointInfo DigishowInterface::initializeEndpointInfo(int index)
{
    dgsEndpointInfo endpointInfo;
    endpointInfo.enabled = m_endpointOptionsList[index].value("enabled", true).toBool();
    endpointInfo.type    = 0;
    endpointInfo.unit    = m_endpointOptionsList[index].value("unit"   ).toInt();
    endpointInfo.channel = m_endpointOptionsList[index].value("channel").toInt();
    endpointInfo.note    = m_endpointOptionsList[index].value("note"   ).toInt();
    endpointInfo.control = m_endpointOptionsList[index].value("control").toInt();
    endpointInfo.signal  = 0;
    endpointInfo.output  = false;
    endpointInfo.input   = false;
    endpointInfo.range   = m_endpointOptionsList[index].value("range"  ).toInt();
    endpointInfo.initial = m_endpointOptionsList[index].value("initial", -1).toDouble();
    endpointInfo.address = m_endpointOptionsList[index].value("address").toString();
    return endpointInfo;
}

int DigishowInterface::initEndpointValue(int endpointIndex)
{
    if (!m_isInterfaceOpened) return ERR_DEVICE_NOT_READY;
    if (endpointIndex<0 || endpointIndex>=m_endpointOptionsList.length()) return ERR_INVALID_INDEX;

    // set initial value for the endpoint
    int n = endpointIndex;
    if (m_endpointInfoList[n].initial >= 0) {
        dgsSignalData data;
        data.signal = m_endpointInfoList[n].signal;
        data.aRange = m_endpointInfoList[n].range;
        switch (data.signal) {
        case DATA_SIGNAL_ANALOG: data.aValue = round(data.aRange * m_endpointInfoList[n].initial); break;
        case DATA_SIGNAL_BINARY: data.bValue = (m_endpointInfoList[n].initial > 0.5); break;
        }

        if (m_endpointInfoList[n].input) {
            //qDebug() << "dataReceived" << n << data.signal << data.aValue << data.bValue;
            emit dataReceived(n, data);
        }
        if (m_endpointInfoList[n].output) {
            //qDebug() << "dataPrepared" << n << data.signal << data.aValue << data.bValue;
            emit dataPrepared(n, data, true);
        }
    }

    return ERR_NONE;
}

QVariantList DigishowInterface::cleanMediaList()
{
    // clean up media list (delete unused media)

    QVariantList mediaList = m_interfaceOptions.value("media").toList();
    int mediaCount = mediaList.length();
    for (int n = mediaCount-1 ; n >= 0 ; n--) {
        QString mediaName = mediaList[n].toMap().value("name").toString();
        bool exists = false;
        for (int i = 0 ; i < endpointCount() ; i++) {
            if (m_endpointOptionsList[i].value("media").toString() == mediaName &&
                m_endpointInfoList[i].enabled) {
                exists = true;
                break;
            }
        }
        if (!exists) mediaList.removeAt(n);
    }
    if (mediaList.length() < mediaCount) m_interfaceOptions["media"] = mediaList;

    return mediaList;
}



