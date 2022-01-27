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

#include "dgs_osc_interface.h"

#include "osc/composer/OscMessageComposer.h"
#include "osc/reader/OscReader.h"
#include "osc/reader/OscMessage.h"

#define OSC_UDP_PORT 8000
#define OSC_OUT_FREQ 50

DgsOscInterface::DgsOscInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "osc";
    m_udp = nullptr;
    m_timer = nullptr;
    m_udpHost = QHostAddress::LocalHost;
    m_udpPort = OSC_UDP_PORT;

    m_outputContinuous = false;

    // clear data buffer
    m_dataAll.clear();
    m_dataUpdatedAddresses.clear();
}


DgsOscInterface::~DgsOscInterface()
{
    closeInterface();
}

int DgsOscInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    // get osc interface configuration
    QString udpHost = m_interfaceOptions.value("udpHost").toString();
    int     udpPort = m_interfaceOptions.value("udpPort").toInt();

    m_udpHost = QHostAddress::LocalHost;
    m_udpPort = OSC_UDP_PORT;

    if (!udpHost.isEmpty()) m_udpHost = QHostAddress(udpHost);
    if ( udpPort > 0      ) m_udpPort = udpPort;

    // create an udp socket for osc
    m_udp = new QUdpSocket();
    connect(m_udp, SIGNAL(readyRead()), this, SLOT(onUdpDataReceived()));

    if (m_interfaceInfo.mode == INTERFACE_OSC_INPUT) {

        // for osc receiver

        m_dataAll.clear();
        m_udp->bind(m_udpPort);

    } else if (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT) {

        // for osc sender

        m_dataAll.clear();
        m_dataUpdatedAddresses.clear();

        // create a timer for sending data frames at a particular frequency
        int frequency = OSC_OUT_FREQ;
        m_outputContinuous = false;

        if (m_interfaceOptions.contains("frequency")) {
            frequency = m_interfaceOptions.value("frequency").toInt();
            m_outputContinuous = true;
        }

        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
        m_timer->setTimerType(Qt::PreciseTimer);
        m_timer->setSingleShot(false);
        m_timer->setInterval(1000 / frequency);
        m_timer->start();
    }

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsOscInterface::closeInterface()
{
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    if (m_udp != nullptr) {
        m_udp->close();
        delete m_udp;
        m_udp = nullptr;
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsOscInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    QString address = m_endpointInfoList[endpointIndex].address;
    int channel = m_endpointInfoList[endpointIndex].channel;
    int type = m_endpointInfoList[endpointIndex].type;

    if ((type == ENDPOINT_OSC_INT   && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_OSC_FLOAT && data.signal != DATA_SIGNAL_ANALOG) ||
        (type == ENDPOINT_OSC_BOOL  && data.signal != DATA_SIGNAL_BINARY)) return ERR_INVALID_DATA;

    // update data frame of the specified address
    QVariantList frame = m_dataAll.value(address).toList();

    for (int n=frame.length() ; n<=channel ; n++) frame.append(QVariant());
    if (type == ENDPOINT_OSC_INT   && data.signal == DATA_SIGNAL_ANALOG) {
        frame[channel] = data.aValue;
    } else
    if (type == ENDPOINT_OSC_FLOAT && data.signal == DATA_SIGNAL_ANALOG) {
        frame[channel] = double(data.aValue) / (data.aRange > 0 ? data.aRange : 1000000);
    } else
    if (type == ENDPOINT_OSC_BOOL  && data.signal == DATA_SIGNAL_BINARY) {
        frame[channel] = data.bValue;
    }

    m_dataAll[address] = frame;
    if (!m_dataUpdatedAddresses.contains(address)) m_dataUpdatedAddresses.append(address);

    return ERR_NONE;
}

void DgsOscInterface::onUdpDataReceived()
{
    QByteArray datagram;
    QHostAddress host;
    quint16 port;

    do {
        datagram.resize(int(m_udp->pendingDatagramSize()));
        m_udp->readDatagram(datagram.data(), datagram.size(), &host, &port);

        //#ifdef QT_DEBUG
        //qDebug() << "osc udp received: " << datagram.toHex();
        //#endif

        OscReader reader(&datagram);
        OscMessage* msg = reader.getMessage();
        if (msg != nullptr) {

            QString address = msg->getAddress(); // get osc address

            // look for all address-matched endpoints
            for  (int n=0 ; n<m_endpointInfoList.length() ; n++) {

                // confirm osc address and channel
                if (m_endpointInfoList[n].address != address) continue;

                int channel = m_endpointInfoList[n].channel;
                if (channel<0 || channel >= (int)msg->getNumValues()) continue;

                // osc value to dgs signal
                OscValue* val = msg->getValue(channel);
                char tag = val->getTag();

                dgsSignalData data;
                switch (m_endpointInfoList[n].type) {
                case ENDPOINT_OSC_INT:
                    if (tag == 'i') {
                        data.signal = DATA_SIGNAL_ANALOG;
                        data.aRange = 0x7FFFFFFF;
                        data.aValue = val->toInteger();
                    }
                case ENDPOINT_OSC_FLOAT:
                    if (tag == 'f') {
                        data.signal = DATA_SIGNAL_ANALOG;
                        data.aRange = 1000000;
                        float fv = val->toFloat();
                        if (fv < 0) fv = 0; else if (fv > 1.0) fv = 1.0;
                        data.aValue = int(fv * data.aRange);
                    }
                case ENDPOINT_OSC_BOOL:
                    if (tag == 'T') {
                        data.signal = DATA_SIGNAL_BINARY;
                        data.bValue = true;
                    } else
                    if (tag == 'F') {
                        data.signal = DATA_SIGNAL_BINARY;
                        data.bValue = false;
                    }
                }

                // emit signal received event
                if (data.signal != 0) emit dataReceived(n, data);
            }

            // emit raw data received event
            if (m_needReceiveRawData) {

                QVariantList rawDataValues;
                int numValues = msg->getNumValues();

                for (int n=0 ; n<numValues ; n++) {

                    OscValue* val = msg->getValue(n);
                    char tag = val->getTag();

                    QVariant value;
                    switch(tag) {
                    case 'i': value = val->toInteger(); break;
                    case 'f': value = val->toFloat(); break;
                    case 'T': value = true; break;
                    case 'F': value = false; break;
                    }
                    QVariantMap rawDataValue;
                    rawDataValue["tag"  ] = QString(tag);
                    rawDataValue["value"] = value;
                    rawDataValues.append(rawDataValue);
                }

                QVariantMap rawData;
                rawData["address"] = address;
                rawData["values" ] = rawDataValues;
                emit rawDataReceived(rawData);
            }

            delete msg;
        }


    } while (m_udp->hasPendingDatagrams());

}

void DgsOscInterface::onTimerFired()
{
    if (m_udp->bytesToWrite() > 0) return;

    foreach (const QString & address, m_dataUpdatedAddresses) {

        // prepare osc message
        QVariantList frame = m_dataAll.value(address).toList();
        if (frame.isEmpty()) continue;

        OscMessageComposer msg(address);
        for (int n=0 ; n<frame.length() ; n++) {
            QVariant val = frame[n];

            switch (val.type()) {
            case QVariant::Bool:
                if (val.toBool()) msg.pushTrue(); else msg.pushFalse();
                break;
            case QVariant::Int:
                msg.pushInt32(val.toInt());
                break;
            case QVariant::Double:
                msg.pushFloat(val.toFloat());
                break;
            default:
                msg.pushNil();
            }
        }

        // send osc message
        QByteArray* formattedMsg = msg.getBytes();
        m_udp->writeDatagram(formattedMsg->constData(), formattedMsg->length(),
                             m_udpHost, (quint16)m_udpPort);
    }

    // send osc data once or continuously
    if (!m_outputContinuous) m_dataUpdatedAddresses.clear();
}
