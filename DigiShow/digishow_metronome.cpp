#include "digishow_metronome.h"
#include "ableton/Link.hpp"
#include <qmath.h>
#include <qendian.h>

DigishowMetronome::DigishowMetronome(QObject *parent) : QObject(parent)
{
    m_running = false;
    m_soundEnabled = false;
    m_linkEnabled = false;
    m_bpm = 120.0;
    m_quantum = 4;

    m_beat = 0;
    m_phase = 0;

    m_link = new ableton::Link(m_bpm);
    m_link->setTempoCallback([&](double bpm) {
        m_bpm = bpm;
        emit bpmChanged();
    });

    m_thread = nullptr;

    // sound player
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    auto deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    if (!deviceInfo.isFormatSupported(format)) format = deviceInfo.nearestFormat(format);

    const int durationSeconds = 1;
    const int toneSampleRateHz = 880;
    m_soundGenerator.reset(new ToneGenerator(format, durationSeconds * 1000000, toneSampleRateHz));
    m_soundOutput.reset(new QAudioOutput(deviceInfo, format));

    connect(this, SIGNAL(beatChanged()), this, SLOT(onBeatChanged()));
    connect(this, SIGNAL(quarterChanged()), this, SLOT(onQuarterChanged()));

    // tap to bpm
    m_tapCount = 0;
    m_tapTimeoutTimer.setSingleShot(true);
    m_tapTimeoutTimer.setInterval(2500);
    connect(&m_tapTimeoutTimer, SIGNAL(timeout()), this, SLOT(onTapTimeout()));

}

DigishowMetronome::~DigishowMetronome()
{
    setLinkEnabled(false);
    setRunning(false);
}

void DigishowMetronome::reset()
{
    setRunning(false);
    setSoundEnabled(false);
    setLinkEnabled(false);
    setBpm(120.0);
    setQuantum(4);
}

void DigishowMetronome::setParameters(const QVariantMap &params)
{
    setRunning(params.value("run", false).toBool());
    setSoundEnabled(params.value("sound", false).toBool());
    setLinkEnabled(params.value("link", false).toBool());
    setBpm(params.value("bpm", 120.0).toDouble());
    setQuantum(params.value("quantum", 4).toInt());
}

QVariantMap DigishowMetronome::getParameters()
{
    QVariantMap params;
    params["run"] = m_running;
    params["sound"] = m_soundEnabled;
    params["link"] = m_linkEnabled;
    params["bpm"] = m_bpm;
    params["quantum"] = m_quantum;
    return params;
}

void DigishowMetronome::setRunning(bool running)
{
    m_mutex.lock();
    if (running) {
        m_running = true;
        if (m_soundEnabled) startSoundOutput();
        if (m_thread == nullptr) {
            m_thread = new DigishowMetronomeThread(this);
            m_thread->start();
        }
    } else {
        m_running = false;
        if (m_soundEnabled) stopSoundOutput();
        if (m_thread != nullptr) {
            m_thread->wait(2000);
            delete m_thread;
            m_thread = nullptr;
        }
    }
    emit isRunningChanged();
    m_mutex.unlock();
}

void DigishowMetronome::setSoundEnabled(bool enabled)
{
    m_mutex.lock();
    m_soundEnabled = enabled;
    emit isSoundEnabledChanged();
    m_mutex.unlock();

    if (enabled)
        startSoundOutput();
    else
        stopSoundOutput();
}

void DigishowMetronome::setLinkEnabled(bool enabled)
{
    m_mutex.lock();
    m_link->enable(enabled);
    m_linkEnabled = enabled;
    emit isLinkEnabledChanged();
    m_mutex.unlock();
}

void DigishowMetronome::setBpm(double bpm)
{
    m_mutex.lock();
    auto sessionState = m_link->captureAppSessionState();
    const auto time = m_link->clock().micros();
    sessionState.setTempo(bpm, time);
    m_link->commitAppSessionState(sessionState);    
    m_bpm = bpm;
    emit bpmChanged();
    m_mutex.unlock();
}

void DigishowMetronome::setQuantum(int quantum)
{
    m_mutex.lock();
    m_quantum = quantum;
    emit quantumChanged();
    m_mutex.unlock();
}

void DigishowMetronome::resetBeat()
{
    m_mutex.lock();
    const auto time = m_link->clock().micros();
    auto sessionState = m_link->captureAppSessionState();
    sessionState.forceBeatAtTime(0.0, time, m_quantum); // set beat=0, phase=0
    m_link->commitAppSessionState(sessionState);
    m_mutex.unlock();
}

void DigishowMetronome::tap()
{
    m_tapCount++;
    if (m_tapCount == 1) {

        // first tap
        m_tapElapsedTimer.start();

    } else if (m_tapCount > 2) {

        // following taps
        qint64 milliseconds = m_tapElapsedTimer.elapsed() / (m_tapCount - 1);
        setBpm(60000 / milliseconds);
    }
    emit tapCountChanged();


    // stop when too many taps
    if (m_tapCount == 12) {
        m_tapCount = 0;
        m_tapElapsedTimer.invalidate();
        emit tapCountChanged();
    }

    // stop when no more tap
    m_tapTimeoutTimer.stop();
    if (m_tapCount > 0) m_tapTimeoutTimer.start();
}

void DigishowMetronome::startSoundOutput()
{
    m_soundGenerator->start();
    m_soundOutput->setVolume(0);
    m_soundOutput->start(m_soundGenerator.data());
}

void DigishowMetronome::stopSoundOutput()
{
    m_soundOutput->stop();
    m_soundGenerator->stop();
}

void DigishowMetronome::run()
{
    m_mutex.lock();
    auto sessionState = m_link->captureAppSessionState();
    const auto time = m_link->clock().micros();
    sessionState.forceBeatAtTime(0, time, m_quantum);
    m_link->commitAppSessionState(sessionState);
    m_beat = 0;
    m_phase = 0;
    emit quarterChanged();
    emit beatChanged();
    m_mutex.unlock();

    while (m_running) {

        m_mutex.lock();
        auto sessionState = m_link->captureAppSessionState();
        const auto time = m_link->clock().micros();
        double beat = sessionState.beatAtTime(time, m_quantum);
        double phase = sessionState.phaseAtTime(time, m_quantum);
        bool quarterProgressed = (int(beat*4) != int(m_beat*4));
        bool oneBeatProgressed = quarterProgressed && (int(beat) != int(m_beat));
        m_beat = beat;
        m_phase = phase;
        if (quarterProgressed) emit quarterChanged();
        if (oneBeatProgressed) emit beatChanged();
        m_mutex.unlock();

        QThread::msleep(1);
    }
}

void DigishowMetronome::onBeatChanged()
{
    // play metronome sound
    if (m_soundEnabled) {
        if (m_phase < 1) {
            m_soundOutput->setVolume(0.8);
        } else {
            m_soundOutput->setVolume(0.2);
        }
    }
}

void DigishowMetronome::onQuarterChanged()
{
    // mute metronome sound
    if (m_soundEnabled && int(m_phase*4)%4 == 1) m_soundOutput->setVolume(0);
}

void DigishowMetronome::onTapTimeout()
{
    // stop when no more tap
    m_tapCount = 0;
    m_tapElapsedTimer.invalidate();
    emit tapCountChanged();
}

ToneGenerator::ToneGenerator(const QAudioFormat &format
    , qint64 durationUs
    , int sampleRate)
{
    if (format.isValid())
        generateData(format, durationUs, sampleRate);
}

void ToneGenerator::start()
{
    open(QIODevice::ReadOnly);
}

void ToneGenerator::stop()
{
    m_pos = 0;
    close();
}

void ToneGenerator::generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
                        * durationUs / 1000000;
    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes) // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int sampleIndex = 0;

    while (length) {
        // Produces value (-1..1)
        const qreal x = qSin(2 * M_PI * sampleRate * qreal(sampleIndex++ % format.sampleRate()) / format.sampleRate());

        for (int i=0; i<format.channelCount(); ++i) {
            if (format.sampleSize() == 8) {
                if (format.sampleType() == QAudioFormat::UnSignedInt) {
                    const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                    *reinterpret_cast<quint8 *>(ptr) = value;
                } else if (format.sampleType() == QAudioFormat::SignedInt) {
                    const qint8 value = static_cast<qint8>(x * 127);
                    *reinterpret_cast<qint8 *>(ptr) = value;
                }
            } else if (format.sampleSize() == 16) {
                if (format.sampleType() == QAudioFormat::UnSignedInt) {
                    quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                    if (format.byteOrder() == QAudioFormat::LittleEndian)
                        qToLittleEndian<quint16>(value, ptr);
                    else
                        qToBigEndian<quint16>(value, ptr);
                } else if (format.sampleType() == QAudioFormat::SignedInt) {
                    qint16 value = static_cast<qint16>(x * 32767);
                    if (format.byteOrder() == QAudioFormat::LittleEndian)
                        qToLittleEndian<qint16>(value, ptr);
                    else
                        qToBigEndian<qint16>(value, ptr);
                }
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
    }
}

qint64 ToneGenerator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 ToneGenerator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 ToneGenerator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}
