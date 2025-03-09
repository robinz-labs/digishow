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

 /*
    NOTE: 
    The code in this file was co-written by AI (Trae/Claude-3.5-Sonnet).
 */

#ifndef AUDIOANALYZER_H
#define AUDIOANALYZER_H

#include <QObject>
#include <QAudioInput>
#include <QByteArray>
#include <QVector>
#include <QTimer>
#include <complex>
#include "kiss_fft.h"

class AudioAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit AudioAnalyzer(const QAudioDeviceInfo &device, QObject *parent = nullptr);
    ~AudioAnalyzer();

    void start();
    void stop();

    void setEnableUpdateLevel(bool enable) { m_enableUpdateLevel = enable; }
    void setEnableUpdatePeak(bool enable) { m_enableUpdatePeak = enable; }
    void setEnableUpdateSpectrum(bool enable) { m_enableUpdateSpectrum = enable; }

signals:
    void spectrumDataReady(const QVector<float> &spectrum, float level, float peak);

private slots:
    void handleAudioData();
    void performAnalysis();

private:

    bool m_enableUpdateLevel;
    bool m_enableUpdatePeak;
    bool m_enableUpdateSpectrum;

    QTimer *m_updateTimer;
    QVector<float> m_sampleWindow;  // Sliding window buffer
    int m_windowPosition;           // Current write position
    
    void processAudioData(const QByteArray &data);
    void performFFT();
    
    QAudioInput *m_audioInput;
    QIODevice *m_audioDevice;
    
    static const int SAMPLE_RATE = 44100;
    static const int FFT_SIZE = 2048;
    static const int SPECTRUM_BANDS = 24;
    static const int UPDATE_FREQ = 40; // Hz
    
    QVector<std::complex<float>> m_fftBuffer;
    QVector<float> m_spectrum;

    kiss_fft_cfg m_cfg;
    kiss_fft_cpx *m_fftIn;
    kiss_fft_cpx *m_fftOut;
};

#endif // AUDIOANALYZER_H
