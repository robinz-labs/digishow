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

#ifndef DIGISHOW_LFO_H
#define DIGISHOW_LFO_H

#include <QTimer>
#include <QElapsedTimer>

// Low-Frequency Oscillator

class DigishowLFO : public QObject
{
    Q_OBJECT
public:
    enum LfoFunction {
        LfoNone     = 0,
        LfoSine     = 1,
        LfoSquare   = 2,
        LfoSawtooth = 3,
        LfoTriangle = 4,
        LfoRandom   = 5
    };
    Q_ENUM(LfoFunction)

    explicit DigishowLFO(QObject *parent = nullptr);
    ~DigishowLFO();

    // Property Setters
    void setSampleInterval(int interval); // millisecond
    void setPeriod(int period);           // millisecond
    void setPhase(double phase);          // 0 ~ 1.0
    void setAmplitude(double amp);        // 0 ~ 1.0
    void setFunction(LfoFunction func);

    // Property Getters
    int sampleInterval() const { return m_sampleInterval; }
    int period() const { return m_period; }
    double phase() const { return m_phase; }
    double amplitude() const { return m_amplitude; }
    LfoFunction function() const { return m_function; }

    // Control Methods
    void start();
    void stop();
    bool isActive() const { return m_timer->isActive(); }
    qint64 timeElapsed() const { return m_elapsed->elapsed(); } // millisecond

private slots:
    void onTimerFired();

signals:
    void valueChanged(double value); // normalized value in [0, 1.0]

private:
    QElapsedTimer *m_elapsed;
    QTimer *m_timer;
    int m_sampleInterval; // millisecond
    int m_period;         // millisecond
    double m_phase;       // 0 ~ 1.0
    double m_amplitude;   // 0 ~ 1.0
    LfoFunction m_function;
};

#endif // DIGISHOW_LFO_H
