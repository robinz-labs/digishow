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

#include <string.h>
#include "dgs_osc_interface.h"

#if _WIN32
#include <winsock2.h>
#define tosc_strncpy(_dst, _src, _len) strncpy_s(_dst, _len, _src, _TRUNCATE)
#else
#include <netinet/in.h>
#define tosc_strncpy(_dst, _src, _len) strncpy(_dst, _src, _len)
#endif
#if __unix__ && !__APPLE__
#include <endian.h>
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#endif

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

    if (m_interfaceInfo.mode == INTERFACE_OSC_INPUT) {

        // for osc receiver

        m_dataAll.clear();
        m_udp->bind(m_udpPort);

        connect(m_udp, SIGNAL(readyRead()), this, SLOT(onUdpDataReceived()));

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

    switch (type) {
    case ENDPOINT_OSC_INT:
        frame[channel] = data.aValue;
        break;
    case ENDPOINT_OSC_FLOAT:
        frame[channel] = double(data.aValue) / (data.aRange > 0 ? data.aRange : 1000000);
        break;
    case ENDPOINT_OSC_BOOL:
        frame[channel] = data.bValue;
        break;
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

        char* buffer = datagram.data();
        int len = datagram.length();
        if (len == 0) continue;

        if (tosc_isBundle(buffer)) {
            tosc_bundle bundle;
            tosc_parseBundle(&bundle, buffer, len);

            #ifdef QT_DEBUG
            const uint64_t timetag = tosc_getTimetag(&bundle);
            printf("osc bundle timetag: %lld\n", timetag);
            #endif

            tosc_message osc;
            while (tosc_getNextMessage(&bundle, &osc)) {
                processOscMessageIn(&osc);
            }
        } else {
            tosc_message osc;
            if (tosc_parseMessage(&osc, buffer, len) == 0) {
                processOscMessageIn(&osc);
            }
        }

    } while (m_udp->hasPendingDatagrams());
}

void DgsOscInterface::processOscMessageIn(tosc_message *osc)
{
    // extract osc message values
    const char* address = tosc_getAddress(osc);
    const char* format = tosc_getFormat(osc);

    #ifdef QT_DEBUG
    printf("[%i bytes] %s %s \r\n",
        osc->len, // the number of bytes in the OSC message
        address,  // the OSC address string, e.g. "/button1"
        format);  // the OSC format string, e.g. "f"
    #endif

    QVariantList values;
    QVariant null;

    int vcount = strlen(osc->format);

    for (int i = 0; i < vcount; i++) {
        switch (format[i]) {
        case 'b': {
            const char *blob = NULL;
            int len = 0;
            tosc_getNextBlob(osc, &blob, &len);
            values.append(null); // blob is unsupported
            break;
        }
        case 'm': {
            tosc_getNextMidi(osc);
            values.append(null); // midi is unsupported
            break;
        }
        case 'f': {
            double fval = tosc_getNextFloat(osc);
            values.append(fval);
            break;
        }
        case 'd': {
            double dval = tosc_getNextDouble(osc);
            values.append(dval);
            break;
        }
        case 'i': {
            int ival = tosc_getNextInt32(osc);
            values.append(ival);
            break;
        }
        case 'h': {
            tosc_getNextInt64(osc);
            values.append(null); // int64 is unsupported
            break;
        }
        case 't': {
            tosc_getNextTimetag(osc);
            values.append(null); // timetag is unsupported
            break;
        }
        case 's': {
            tosc_getNextString(osc);
            values.append(null); // string is unsupported
            break;
        }
        case 'T': {
            values.append(true);
            break;
        }
        case 'F': {
            values.append(false);
            break;
        }
        case 'N': {
            values.append(null);
            break;
        }
        case 'I': {
            values.append(null);
            break;
        }
        default:
            i = vcount; // unrecognized data field encountered, stop fetching more
        }
    }

    // generate digishow signals for the received osc messages

    // look for all address-matched endpoints
    for  (int n=0 ; n<m_endpointInfoList.length() ; n++) {

        // confirm osc address and channel
        if (m_endpointInfoList[n].address != address) continue;

        int channel = m_endpointInfoList[n].channel;
        if (channel<0 || channel >= values.length()) continue;

        // osc value to dgs signal
        const QVariant &val = values.at(channel);

        dgsSignalData data;
        switch (m_endpointInfoList[n].type) {
        case ENDPOINT_OSC_INT:
            if (!val.isNull() && val.type() == QVariant::Int) {
                data.signal = DATA_SIGNAL_ANALOG;
                data.aRange = m_endpointInfoList[n].range;
                data.aValue = qBound(0, val.toInt(), data.aRange);
            }
        case ENDPOINT_OSC_FLOAT:
            if (!val.isNull() && val.type() == QVariant::Double) {
                data.signal = DATA_SIGNAL_ANALOG;
                data.aRange = 1000000;
                data.aValue = int(qBound(0.0, val.toDouble(), 1.0) * data.aRange);
            }
        case ENDPOINT_OSC_BOOL:
            if (!val.isNull() && val.type() == QVariant::Bool) {
                data.signal = DATA_SIGNAL_BINARY;
                data.bValue = val.toBool();
            }
        }

        // emit signal received event
        if (data.signal != 0) emit dataReceived(n, data);
    }

    // emit raw data received event
    if (m_needReceiveRawData) {

        QVariantList rawDataValues;
        int numValues = values.length();

        for (int n=0 ; n<numValues ; n++) {

            const QVariant &val = values.at(n);
            char tag;
            switch(val.type()) {
                case QVariant::Int:    tag = 'i'; break;
                case QVariant::Double: tag = 'f'; break;
                case QVariant::Bool:   tag = (val.toBool() ? 'T' : 'F'); break;
                default: tag = 'N';
            }
            QVariantMap rawDataValue;
            rawDataValue["tag"  ] = QString(tag);
            rawDataValue["value"] = val;
            rawDataValues.append(rawDataValue);
        }

        QVariantMap rawData;
        rawData["address"] = address;
        rawData["values" ] = rawDataValues;
        emit rawDataReceived(rawData);
    }

}


int DgsOscInterface::prepareOscMessageOut(char *buffer, const int len, const char *address, const QVariantList &values)
{
    memset(buffer, 0, len); // clear the buffer
    int i = strlen(address);
    if (address == NULL || i >= len) return -1;
    tosc_strncpy(buffer, address, len);
    i = (i + 4) & ~0x3;
    buffer[i++] = ',';

    int s_len = values.length();
    QByteArray formatBytes(s_len, 0);
    for (int n=0 ; n<s_len; n++) {
        QVariant val = values[n];
        switch (val.type()) {
        case QVariant::Bool:
            formatBytes[n] = (val.toBool() ? 'T' : 'F');
            break;
        case QVariant::Int:
            formatBytes[n] = 'i';
            break;
        case QVariant::Double:
            formatBytes[n] = 'f';
            break;
        default:
            formatBytes[n] = 'N';
        }
    }
    const char* format = formatBytes.constData();

    if (format == NULL || (i + s_len) >= len) return -2;
    tosc_strncpy(buffer+i, format, len-i-s_len);
    i = (i + 4 + s_len) & ~0x3;

    for (int j = 0; j < s_len; ++j) {
        switch (format[j]) {
            case 'f': {
                if (i + 4 > len) return -3;
                const float f = values[j].toFloat();
                *((uint32_t *) (buffer+i)) = htonl(*((uint32_t *) &f));
                i += 4;
                break;
            }
            case 'i': {
                if (i + 4 > len) return -3;
                const uint32_t k = values[j].toInt();
                *((uint32_t *) (buffer+i)) = htonl(k);
                i += 4;
                break;
            }
            case 'T': // true
            case 'F': // false
            case 'N': // nil
                break;
            default: return -4; // unknown type
        }
    }

    return i; // return the total number of bytes written
}


void DgsOscInterface::onTimerFired()
{
    static char bufOscMsg[20480];

    if (m_udp->bytesToWrite() > 0) return;

    foreach (const QString & address, m_dataUpdatedAddresses) {

        // prepare osc message
        QVariantList frame = m_dataAll.value(address).toList();
        if (frame.isEmpty()) continue;

        int len = prepareOscMessageOut(bufOscMsg, sizeof(bufOscMsg), address.toUtf8(), frame);
        if (len < 1) continue;

        // send osc message
        m_udp->writeDatagram(bufOscMsg, len, m_udpHost, (quint16)m_udpPort);
    }

    // send osc data once or continuously
    if (!m_outputContinuous) m_dataUpdatedAddresses.clear();
}


void DgsOscInterface::updateMetadata_()
{
    m_interfaceInfo.type = INTERFACE_OSC;

    // Set interface mode
    QString modeName = m_interfaceOptions.value("mode").toString();
    if      (modeName == "input" ) m_interfaceInfo.mode = INTERFACE_OSC_INPUT;
    else if (modeName == "output") m_interfaceInfo.mode = INTERFACE_OSC_OUTPUT;

    // Set interface flags
    m_interfaceInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
    m_interfaceInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);

    // Set interface label
    m_interfaceInfo.label = tr("OSC") + " " +
                           m_interfaceOptions.value("udpHost").toString() + ":" +
                           m_interfaceOptions.value("udpPort").toString();

    // Process endpoints
    for (int n = 0; n < m_endpointOptionsList.length(); n++) {
        dgsEndpointInfo endpointInfo = initializeEndpointInfo(n);

        // Set endpoint type
        QString typeName = m_endpointOptionsList[n].value("type").toString();
        if      (typeName == "int"  ) endpointInfo.type = ENDPOINT_OSC_INT;
        else if (typeName == "float") endpointInfo.type = ENDPOINT_OSC_FLOAT;
        else if (typeName == "bool" ) endpointInfo.type = ENDPOINT_OSC_BOOL;

        // Set endpoint properties based on type
        endpointInfo.labelEPT = tr("OSC");

        switch (endpointInfo.type) {
            case ENDPOINT_OSC_INT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
                endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
                endpointInfo.range  = (endpointInfo.range ? endpointInfo.range : 1000000);
                endpointInfo.labelEPI = tr("Integer") + QString(" %1").arg(endpointInfo.channel + 1);
                break;
            case ENDPOINT_OSC_FLOAT:
                endpointInfo.signal = DATA_SIGNAL_ANALOG;
                endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
                endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
                endpointInfo.range  = 1000000;
                endpointInfo.labelEPI = tr("Float") + QString(" %1").arg(endpointInfo.channel + 1);
                break;
            case ENDPOINT_OSC_BOOL:
                endpointInfo.signal = DATA_SIGNAL_BINARY;
                endpointInfo.output = (m_interfaceInfo.mode == INTERFACE_OSC_OUTPUT);
                endpointInfo.input  = (m_interfaceInfo.mode == INTERFACE_OSC_INPUT);
                endpointInfo.labelEPI = tr("Bool") + QString(" %1").arg(endpointInfo.channel + 1);
                break;
        }

        m_endpointInfoList.append(endpointInfo);
    }
}
