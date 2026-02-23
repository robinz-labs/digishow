/*
    Copyright 2026 Robin Zhang & Labs

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

#include "digishow_lfo.h"
#include <QtMath>
#include <QRandomGenerator>

DigishowLFO::DigishowLFO(QObject *parent) : QObject(parent)
{
    m_elapsed = new QElapsedTimer();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));

    m_sampleInterval = 20; // 50Hz default
    m_period = 1000;       // 1Hz default
    m_phase = 0.0;
    m_amplitude = 1.0;
    m_function = LfoSine;
}

DigishowLFO::~DigishowLFO()
{
    delete m_elapsed;
    delete m_timer;
}

void DigishowLFO::setSampleInterval(int interval)
{
    m_sampleInterval = interval;
    if (m_timer->isActive()) {
        m_timer->start(m_sampleInterval);
    }
}

void DigishowLFO::setPeriod(int period)
{
    m_period = period;
}

void DigishowLFO::setPhase(double phase)
{
    m_phase = phase;
}

void DigishowLFO::setAmplitude(double amp)
{
    m_amplitude = amp;
}

void DigishowLFO::setFunction(LfoFunction func)
{
    m_function = func;
}

void DigishowLFO::start()
{
    m_elapsed->start();
    m_timer->start(m_sampleInterval);
}

void DigishowLFO::stop()
{
    m_timer->stop();
}

void DigishowLFO::onTimerFired()
{
    if (m_period <= 0) return;
    if (m_phase < 0.0 || m_phase >= 1.0) return;

    qint64 elapsed = m_elapsed->elapsed();
    double phase = (elapsed % m_period) / (double)m_period + m_phase;
    if (phase >= 1.0) phase -= 1.0;
    double value = 0.0;

    switch (m_function) {
    case LfoNone:
        value = 0.0;
        break;
    case LfoSine:
        value = (qSin(phase * 2 * M_PI) + 1.0) * 0.5;
        break;
    case LfoSquare:
        value = (phase < 0.5) ? 1.0 : 0.0;
        break;
    case LfoSawtooth:
        value = phase;
        break;
    case LfoTriangle:
        if (phase < 0.5)
            value = phase * 2.0;
        else
            value = (1.0 - phase) * 2.0;
        break;
    case LfoRandom:
        value = QRandomGenerator::global()->generateDouble();
        break;
    }

    emit valueChanged(value * m_amplitude);
}
