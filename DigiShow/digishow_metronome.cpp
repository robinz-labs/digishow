#include "digishow_metronome.h"
#include "ableton/Link.hpp"

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

    m_soundList = new QMediaPlaylist(this);
    m_soundList->addMedia(QUrl("qrc:///sounds/metro1.wav"));
    m_soundList->addMedia(QUrl("qrc:///sounds/metro2.wav"));
    m_soundList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    m_soundPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    m_soundPlayer->setPlaylist(m_soundList);

    connect(this, SIGNAL(phaseChanged()), this, SLOT(onPhaseChanged()));
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
        if (m_thread == nullptr) {
            m_thread = new DigishowMetronomeThread(this);
            m_thread->start();
        }
    } else {
        m_running = false;
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

void DigishowMetronome::run()
{
    m_mutex.lock();
    auto sessionState = m_link->captureAppSessionState();
    const auto time = m_link->clock().micros();
    sessionState.forceBeatAtTime(0, time, m_quantum);
    m_link->commitAppSessionState(sessionState);
    m_beat = 0;
    m_phase = 0;
    emit beatChanged();
    emit phaseChanged();
    m_mutex.unlock();

    while (m_running) {

        m_mutex.lock();
        auto sessionState = m_link->captureAppSessionState();
        const auto time = m_link->clock().micros();
        double beat = sessionState.beatAtTime(time, m_quantum);
        double phase = sessionState.phaseAtTime(time, m_quantum);
        bool fired = (int(beat) != int(m_beat));
        m_beat = beat;
        m_phase = phase;
        if (fired) {
            emit beatChanged();
            emit phaseChanged();            
        }
        m_mutex.unlock();

        QThread::msleep(5);
    }
}

void DigishowMetronome::onPhaseChanged()
{
    // play metronome sound
    if (m_soundEnabled) {
        if (m_phase < 1) {
            m_soundList->setCurrentIndex(0);
            m_soundPlayer->play();
        } else if (m_bpm <= 300) {
            m_soundList->setCurrentIndex(1);
            m_soundPlayer->play();
        }
    }
}
