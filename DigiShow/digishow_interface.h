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

#ifndef DIGISHOWINTERFACE_H
#define DIGISHOWINTERFACE_H

#include <QObject>
#include <QVariantMap>
#include "digishow.h"

class DigishowInterface : public QObject
{
    Q_OBJECT
public:
    explicit DigishowInterface(QObject *parent = nullptr);
    ~DigishowInterface();

    // interface options
    Q_INVOKABLE int setInterfaceOptions(const QVariantMap &options);
    Q_INVOKABLE int setInterfaceOption(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariantMap getInterfaceOptions() { return m_interfaceOptions; }
    QVariantMap *interfaceOptions() { return &m_interfaceOptions; }

    // interface info
    Q_INVOKABLE QVariantMap getInterfaceInfo();
    dgsInterfaceInfo *interfaceInfo() { return &m_interfaceInfo; }

    // endpoint options
    Q_INVOKABLE int addEndpoint(const QVariantMap &options, int *endpointIndex = nullptr);
    Q_INVOKABLE int setEndpointOptions(int endpointIndex, const QVariantMap &options);
    Q_INVOKABLE int setEndpointOption(int endpointIndex, const QString &name, const QVariant &value);
    Q_INVOKABLE int endpointCount();
    Q_INVOKABLE QVariantMap getEndpointOptionsAt(int endpointIndex);
    QList<QVariantMap> *endpointOptionsList() { return &m_endpointOptionsList; }

    // endpoint info
    Q_INVOKABLE QVariantMap getEndpointInfoAt(int endpointIndex);
    QList<dgsEndpointInfo> *endpointInfoList() { return &m_endpointInfoList; }

    // normalize options ==> info
    Q_INVOKABLE void updateMetadata();

    // controls
    virtual int openInterface();
    virtual int closeInterface();
    virtual int init(); // called after slot service got ready
    virtual int sendData(int endpointIndex, dgsSignalData data);

    // media
    virtual int loadMedia(const QVariantMap &mediaOptions);

    Q_INVOKABLE bool isInterfaceOpened() { return m_isInterfaceOpened; }

    bool needReceiveRawData() { return m_needReceiveRawData; }
    void setNeedReceiveRawData(bool needReceiveRawData) { m_needReceiveRawData = needReceiveRawData; }

signals:

    void dataReceived(int endpointIndex, dgsSignalData dataIn);
    void dataPrepared(int endpointIndex, dgsSignalData dataOut);

    void rawDataReceived(const QVariantMap &rawData);

public slots:

protected:

    // interface options (for read/write)
    QVariantMap m_interfaceOptions;
    QList<QVariantMap> m_endpointOptionsList;

    // interface info (for fast read)
    dgsInterfaceInfo m_interfaceInfo;
    QList<dgsEndpointInfo> m_endpointInfoList;

    bool m_isInterfaceOpened;
    bool m_needReceiveRawData;

    // initialize endpoint signal
    int initEndpointValue(int endpointIndex);

    // clean up media list (delete unused media)
    // only for the interface that contains a media list
    QVariantList cleanMediaList();
};

#endif // DIGISHOWINTERFACE_H
