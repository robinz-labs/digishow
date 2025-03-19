/*
    Copyright 2025 Robin Zhang & Labs

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

#include "dgs_messager_interface.h"
#include "abstract_messager.h"
#include "com_messager.h"
#include "tcp_messager.h"
#include "udp_messager.h"
#include "app_utilities.h"
#include <QSerialPortInfo>

DgsMessagerInterface::DgsMessagerInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "messager";
    m_messager = nullptr;
    m_subscribedMessageIndexes.clear();
}

DgsMessagerInterface::~DgsMessagerInterface()
{
    closeInterface();
}

int DgsMessagerInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // initialize the communication
    bool done = false;
    if (m_interfaceInfo.mode == INTERFACE_MESSAGER_COM) {

        QString comPort = m_interfaceOptions.value("comPort").toString();
        int     comBaud = m_interfaceOptions.value("comBaud").toInt();

        ComMessager::SerialSetting comSetting = ComMessager::Setting8N1;
        QString comParity = m_interfaceOptions.value("comParity").toString();
        if (comParity == "8N1") comSetting = ComMessager::Setting8N1; else
        if (comParity == "8E1") comSetting = ComMessager::Setting8E1; else
        if (comParity == "8O1") comSetting = ComMessager::Setting8O1; else
        if (comParity == "7E1") comSetting = ComMessager::Setting7E1; else
        if (comParity == "7O1") comSetting = ComMessager::Setting7O1; else
        if (comParity == "8N2") comSetting = ComMessager::Setting8N2;

        m_messager = new ComMessager();
        done = ((ComMessager*)m_messager)->open(comPort, comBaud, comSetting);

    } else if (m_interfaceInfo.mode == INTERFACE_MESSAGER_TCP) {

        QString tcpHost = m_interfaceOptions.value("tcpHost").toString();
        int     tcpPort = m_interfaceOptions.value("tcpPort").toInt();

        m_messager = new TcpMessager();
        done = ((TcpMessager*)m_messager)->open(tcpHost, tcpPort);

    } else if (m_interfaceInfo.mode == INTERFACE_MESSAGER_UDP_IN) {

        int udpPort = m_interfaceOptions.value("udpPort").toInt();

        m_messager = new UdpMessager();
        done = ((UdpMessager*)m_messager)->open(udpPort, "", 0); // start the local udp service

    } else if (m_interfaceInfo.mode == INTERFACE_MESSAGER_UDP_OUT) {

        QString udpHost = m_interfaceOptions.value("udpHost").toString();
        int     udpPort = m_interfaceOptions.value("udpPort").toInt();

        m_messager = new UdpMessager();
        done = ((UdpMessager*)m_messager)->open(0, udpHost, udpPort); // connect to the remote udp service
    }

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    // set the callback for receiving incoming messages
    connect(m_messager, SIGNAL(messageReceived(int)), this, SLOT(onMessageReceived(int)));
    connect(m_messager, SIGNAL(rawDataReceived(QByteArray)), this, SLOT(onRawDataReceived(QByteArray)));

    // subscribe the specific incoming messages
    m_subscribedMessageIndexes.clear();

    for  (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        QVariantMap endpointOptions = m_endpointOptionsList[n];

        if (endpointInfo.enabled && endpointOptions.value("subscribed").toBool()) {

            QString message = endpointOptions.value("message").toString();
            bool isHex = (endpointInfo.type == ENDPOINT_MESSAGER_HEXCODE);
            QByteArray msgData = getMessageBytes(message, isHex);

            m_messager->subscribeMessage(msgData);
            m_subscribedMessageIndexes.append(n);
        }
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsMessagerInterface::closeInterface()
{
    if (m_messager) {
        m_messager->close();
        delete m_messager;
        m_messager = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsMessagerInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
    if (data.bValue) {
        QString message = m_endpointOptionsList[endpointIndex].value("message").toString();
        bool isHex = (m_endpointInfoList[endpointIndex].type == ENDPOINT_MESSAGER_HEXCODE);
        QByteArray msgData = getMessageBytes(message, isHex);
        m_messager->sendMessage(msgData);
    }

    return ERR_NONE;
}

void DgsMessagerInterface::onMessageReceived(int index)
{
    if (index < 0 || index >= m_subscribedMessageIndexes.length()) return;


    int epIndex = m_subscribedMessageIndexes[index];

    dgsSignalData data;
    data.signal = DATA_SIGNAL_BINARY;
    data.bValue = true;
    emit dataReceived(epIndex, data);

    QTimer::singleShot(300, this, [this, epIndex]() {
        dgsSignalData data;
        data.signal = DATA_SIGNAL_BINARY;
        data.bValue = false;
        emit dataReceived(epIndex, data);
    });
}

void DgsMessagerInterface::onRawDataReceived(const QByteArray &data)
{
    // emit raw data received event
    if (m_needReceiveRawData) {

        // determine text or hex
        bool isHex = false;
        for (int n=0 ; n<data.length() ; n++) {
            uint8_t byte = data.at(n);
            if ((byte < 0x20 || byte > 0x7E) && byte != '\r' && byte != '\n' && byte != '\t') {
                isHex = true;
                break;
            }
        }

        QVariantMap rawData;
        rawData["type"] = (isHex ? "hex" : "text");
        rawData["message"] = (isHex ? QString(data.toHex(' ').toUpper()) : AppUtilities::encodeCStyleEscapes(data));
        emit rawDataReceived(rawData);
    }
}

QVariantList DgsMessagerInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

#ifdef Q_OS_MAC
        if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif
        info.clear();
        info["comPort"       ] = serialPortInfo.portName();
        info["comVid"        ] = serialPortInfo.vendorIdentifier();
        info["comPid"        ] = serialPortInfo.productIdentifier();
        info["comFacturer"   ] = serialPortInfo.manufacturer();
        info["comDescription"] = serialPortInfo.description();
        info["mode"          ] = "com";
        list.append(info);
    }

    return list;
}

QByteArray DgsMessagerInterface::getMessageBytes(const QString & message, bool isHex)
{
    if (isHex) {

        // the message is in hex code mode
        return QByteArray::fromHex(message.toUtf8());

    } else {

        // the message is in text mode (c-style printf-escaped string)
        return AppUtilities::decodeCStyleEscapes(message);
    }
}

