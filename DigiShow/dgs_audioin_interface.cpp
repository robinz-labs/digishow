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

#include "dgs_audioin_interface.h"

DgsAudioinInterface::DgsAudioinInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "audioin";

    // TODO:
}


DgsAudioinInterface::~DgsAudioinInterface()
{
    closeInterface();
}

int DgsAudioinInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;
    updateMetadata();

    // request privacy permission to access the microphone.
    if (!AppUtilities::canAccessMicrophone()) return ERR_DEVICE_NOT_READY;

    // confirm the specified audio input device
    QString audioinName = m_interfaceOptions.value("port").toString();
    QAudioDeviceInfo device;
    if (audioinName.isEmpty()) {
        device = QAudioDeviceInfo::defaultInputDevice();
    } else {
        int audioinIndex = -1;
        QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        for (int n=0 ; n < devices.length() ; n++)
            if (getUniqueAudioinName(n) == audioinName) { audioinIndex = n; break; }
        if (audioinIndex == -1) return ERR_DEVICE_NOT_READY;
        device = devices[audioinIndex];
    }

    // initialize audio input and sound level meter
    /*
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    if (!device.isFormatSupported(format)) format = device.nearestFormat(format);
    */

    QAudioFormat format = device.preferredFormat();
    m_soundLevelMeter.reset(new SoundLevelMeter(format));
    connect(m_soundLevelMeter.data(), &SoundLevelMeter::update, [this]() {

        // qDebug() << m_soundLevelMeter->level();

        for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
            if (m_endpointInfoList[n].type == ENDPOINT_AUDIOIN_LEVEL) {
                dgsSignalData data;
                data.signal = DATA_SIGNAL_ANALOG;
                data.aRange = m_endpointInfoList[n].range;
                data.aValue = m_soundLevelMeter->level() * m_endpointInfoList[n].range;
                emit dataReceived(n, data);
            }
        }
    });

    m_audioInput.reset(new QAudioInput(device, format));
    m_audioInput->setVolume(1.0);
    m_soundLevelMeter->start();
    m_audioInput->start(m_soundLevelMeter.data());

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsAudioinInterface::closeInterface()
{
    if (!m_audioInput.isNull()) {
        m_audioInput->stop();
        m_audioInput.reset();
    }

    if (!m_soundLevelMeter.isNull()) {
        m_soundLevelMeter->stop();
        m_soundLevelMeter->disconnect();
        m_soundLevelMeter.reset();
    }

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsAudioinInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // TODO:

    return ERR_NONE;
}

QVariantList DgsAudioinInterface::listOnline()
{
    QVariantList list;
    QVariantMap info;

    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (int n=0 ; n < devices.length() ; n++) {
        info.clear();
        info["port"] = getUniqueAudioinName(n);

        QAudioFormat format = devices[n].preferredFormat();
        info["channelCount"] = format.channelCount();
        info["sampleRate"] = format.sampleRate();
        info["sampleSize"] = format.sampleSize();
        info["sampleType"] = format.sampleType();
        list.append(info);
    }

    return list;
}

QString DgsAudioinInterface::getUniqueAudioinName(int index)
{
    QString audioinName;
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    int i = 0;

    if (index >= 0 && index < devices.length()) {

        audioinName = devices[index].deviceName();

        for (int n=0 ; n<index ; n++)
            if (devices[n].deviceName() == audioinName) i++;
    }

    if (i>0) return audioinName + " #" + QString::number(i+1);
    return audioinName;
}

SoundLevelMeter::SoundLevelMeter(const QAudioFormat &format)
    : m_format(format)
{
    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        break;
    }
}

void SoundLevelMeter::start()
{
    open(QIODevice::WriteOnly);
}

void SoundLevelMeter::stop()
{
    close();
}

qint64 SoundLevelMeter::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 SoundLevelMeter::writeData(const char *data, qint64 len)
{
    if (m_maxAmplitude) {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channelCount() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        quint32 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i) {
            for (int j = 0; j < m_format.channelCount(); ++j) {
                quint32 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
                    value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;
    }

    emit update();
    return len;
}

