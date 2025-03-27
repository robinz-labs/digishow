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

#include "dgs_messenger_interface.h"
#include "abstract_messenger.h"
#include "com_messenger.h"
#include "tcp_messenger.h"
#include "udp_messenger.h"
#include "app_utilities.h"
#include <QSerialPortInfo>

DgsMessengerInterface::DgsMessengerInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "messenger";
    m_messenger = nullptr;
    m_subscribedMessageIndexes.clear();
}

DgsMessengerInterface::~DgsMessengerInterface()
{
    closeInterface();
}

int DgsMessengerInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // initialize the communication
    bool done = false;
    if (m_interfaceInfo.mode == INTERFACE_MESSENGER_COM) {

        QString comPort = m_interfaceOptions.value("comPort").toString();
        int     comBaud = m_interfaceOptions.value("comBaud").toInt();

        ComMessenger::SerialSetting comSetting = ComMessenger::Setting8N1;
        QString comParity = m_interfaceOptions.value("comParity").toString();
        if (comParity == "8N1") comSetting = ComMessenger::Setting8N1; else
        if (comParity == "8E1") comSetting = ComMessenger::Setting8E1; else
        if (comParity == "8O1") comSetting = ComMessenger::Setting8O1; else
        if (comParity == "7E1") comSetting = ComMessenger::Setting7E1; else
        if (comParity == "7O1") comSetting = ComMessenger::Setting7O1; else
        if (comParity == "8N2") comSetting = ComMessenger::Setting8N2;

        m_messenger = new ComMessenger();
        done = ((ComMessenger*)m_messenger)->open(comPort, comBaud, comSetting);

    } else if (m_interfaceInfo.mode == INTERFACE_MESSENGER_TCP) {

        QString tcpHost = m_interfaceOptions.value("tcpHost").toString();
        int     tcpPort = m_interfaceOptions.value("tcpPort").toInt();

        m_messenger = new TcpMessenger();
        done = ((TcpMessenger*)m_messenger)->open(tcpHost, tcpPort);

    } else if (m_interfaceInfo.mode == INTERFACE_MESSENGER_UDP_IN) {

        int udpPort = m_interfaceOptions.value("udpPort").toInt();

        m_messenger = new UdpMessenger();
        done = ((UdpMessenger*)m_messenger)->open(udpPort, "", 0); // start the local udp service

    } else if (m_interfaceInfo.mode == INTERFACE_MESSENGER_UDP_OUT) {

        QString udpHost = m_interfaceOptions.value("udpHost").toString();
        int     udpPort = m_interfaceOptions.value("udpPort").toInt();

        m_messenger = new UdpMessenger();
        done = ((UdpMessenger*)m_messenger)->open(0, udpHost, udpPort); // connect to the remote udp service
    }

    if (!done) {
        closeInterface();
        return ERR_DEVICE_NOT_READY;
    }

    // set the callback for receiving incoming messages
    connect(m_messenger, SIGNAL(messageReceived(int)), this, SLOT(onMessageReceived(int)));
    connect(m_messenger, SIGNAL(rawDataReceived(QByteArray)), this, SLOT(onRawDataReceived(QByteArray)));

    // subscribe the specific incoming messages
    m_subscribedMessageIndexes.clear();

    for  (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        dgsEndpointInfo endpointInfo = m_endpointInfoList[n];
        QVariantMap endpointOptions = m_endpointOptionsList[n];

        if (endpointInfo.enabled && endpointOptions.value("subscribed").toBool()) {

            QString message = endpointOptions.value("message").toString();
            bool isHex = (endpointInfo.type == ENDPOINT_MESSENGER_HEXCODE);
            QByteArray msgData = getMessageBytes(message, isHex);

            m_messenger->subscribeMessage(msgData);
            m_subscribedMessageIndexes.append(n);
        }
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsMessengerInterface::closeInterface()
{
    if (m_messenger) {
        m_messenger->close();
        delete m_messenger;
        m_messenger = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsMessengerInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    if (data.signal != DATA_SIGNAL_BINARY) return ERR_INVALID_DATA;
    if (data.bValue) {
        QString message = m_endpointOptionsList[endpointIndex].value("message").toString();
        bool isHex = (m_endpointInfoList[endpointIndex].type == ENDPOINT_MESSENGER_HEXCODE);
        QByteArray msgData = getMessageBytes(message, isHex);
        m_messenger->sendMessage(msgData);
    }

    return ERR_NONE;
}

void DgsMessengerInterface::onMessageReceived(int index)
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

void DgsMessengerInterface::onRawDataReceived(const QByteArray &data)
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

QVariantList DgsMessengerInterface::listOnline()
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

QByteArray DgsMessengerInterface::getMessageBytes(const QString & message, bool isHex)
{
    if (isHex) {

        // the message is in hex code mode
        return QByteArray::fromHex(message.toUtf8());

    } else {

        // the message is in text mode (c-style printf-escaped string)
        return AppUtilities::decodeCStyleEscapes(message);
    }
}

void DgsMessengerInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_MESSENGER;

    // Set interface mode
    QString modeName = m_interfaceOptions.value("mode").toString();
    if      (modeName == "com"    ) m_interfaceInfo.mode = INTERFACE_MESSENGER_COM;
    else if (modeName == "tcp"    ) m_interfaceInfo.mode = INTERFACE_MESSENGER_TCP;
    else if (modeName == "udp_in" ) m_interfaceInfo.mode = INTERFACE_MESSENGER_UDP_IN;
    else if (modeName == "udp_out") m_interfaceInfo.mode = INTERFACE_MESSENGER_UDP_OUT;

    // Set interface flags
    m_interfaceInfo.input = (m_interfaceInfo.mode != INTERFACE_MESSENGER_UDP_OUT);
    m_interfaceInfo.output = (m_interfaceInfo.mode != INTERFACE_MESSENGER_UDP_IN);

    // Set interface label
    QString labelType, labelIdentity;
    switch (m_interfaceInfo.mode) {
    case INTERFACE_MESSENGER_COM:
        labelType = tr("Serial");
        labelIdentity = m_interfaceOptions.value("comPort").toString();
        break;
    case INTERFACE_MESSENGER_TCP:
        labelType = tr("TCP");
        labelIdentity = m_interfaceOptions.value("tcpHost").toString() + ":" +
                        m_interfaceOptions.value("tcpPort").toString();
        break;
    case INTERFACE_MESSENGER_UDP_IN:
        labelType = tr("UDP");
        labelIdentity = m_interfaceOptions.value("udpPort").toString();
        break;
    case INTERFACE_MESSENGER_UDP_OUT:
        labelType = tr("UDP");
        labelIdentity = m_interfaceOptions.value("udpHost").toString() + ":" +
                        m_interfaceOptions.value("udpPort").toString();
        break;
    }
    m_interfaceInfo.label = labelType + " " + labelIdentity;

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "text") endpointInfo.type = ENDPOINT_MESSENGER_TEXT;
        else if (typeName == "hex" ) endpointInfo.type = ENDPOINT_MESSENGER_HEXCODE;

        // Set endpoint properties
        endpointInfo.signal = DATA_SIGNAL_BINARY;
        if (m_endpointOptionsList[n].value("subscribed").toBool())
            endpointInfo.input = true;
        else
            endpointInfo.output = true;
        endpointInfo.labelEPT = labelType;
        endpointInfo.labelEPI = m_endpointOptionsList[n].value("message").toString().left(8);

        m_endpointInfoList.append(endpointInfo);
    }
}
