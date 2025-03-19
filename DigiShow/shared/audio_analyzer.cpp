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

#include "audio_analyzer.h"
#include <QAudioFormat>
#include <QDebug>
#include <cmath>
#include <QTimer>
#include "kiss_fft.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AudioAnalyzer::AudioAnalyzer(const QAudioDeviceInfo &device, QObject *parent)
    : QObject(parent)
    , m_enableUpdateLevel(true)
    , m_enableUpdatePeak(true)
    , m_enableUpdateSpectrum(true)
    , m_windowPosition(0)
    , m_audioInput(nullptr)
    , m_audioDevice(nullptr)
{
    // Initialize Kiss FFT
    m_cfg = kiss_fft_alloc(FFT_SIZE, 0, nullptr, nullptr);
    m_fftIn = new kiss_fft_cpx[FFT_SIZE];
    m_fftOut = new kiss_fft_cpx[FFT_SIZE];

    // Setup audio format
    QAudioFormat format;
    format.setSampleRate(SAMPLE_RATE);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // Check if the desired format is supported by the device
    if (!device.isNull()) {
        if (!device.isFormatSupported(format)) {
            qWarning() << "Default format not supported, trying to use nearest";
            format = device.nearestFormat(format);
        }
        m_audioInput = new QAudioInput(device, format, this);
        m_audioInput->setVolume(1.0);
    } else {
        qWarning() << "No audio input device found";
        m_audioInput = nullptr;
    }
    m_fftBuffer.resize(FFT_SIZE);
    m_spectrum.resize(SPECTRUM_BANDS);
    m_sampleWindow.resize(FFT_SIZE);
    m_sampleWindow.fill(0.0f);
    
    // Setup timer
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(1000 / UPDATE_FREQ);  // 20Hz = 50ms
    connect(m_updateTimer, &QTimer::timeout, this, &AudioAnalyzer::performAnalysis);
}

AudioAnalyzer::~AudioAnalyzer()
{
    stop();
    
    // Cleanup Kiss FFT
    kiss_fft_free(m_cfg);
    delete[] m_fftIn;
    delete[] m_fftOut;
}

void AudioAnalyzer::start()
{
    if (m_audioInput && !m_audioDevice) {
        m_audioDevice = m_audioInput->start();
        connect(m_audioDevice, &QIODevice::readyRead, this, &AudioAnalyzer::handleAudioData);
        m_updateTimer->start();
    }
}

void AudioAnalyzer::stop()
{
    if (m_audioInput && m_audioDevice) {
        m_audioInput->stop();
        disconnect(m_audioDevice, &QIODevice::readyRead, this, &AudioAnalyzer::handleAudioData);
        m_audioDevice = nullptr;
        m_updateTimer->stop();
    }
}

void AudioAnalyzer::handleAudioData()
{
    if (!m_audioDevice) return;
    
    QByteArray data = m_audioDevice->readAll();
    if (data.size() > 0) {
        processAudioData(data);
    }
}

void AudioAnalyzer::processAudioData(const QByteArray &data)
{
    int bytesPerSample = 2;
    const qint16 *samples = reinterpret_cast<const qint16*>(data.constData());
    int sampleCount = data.size() / bytesPerSample;
    
    // Write new data to sliding window
    for (int i = 0; i < sampleCount; ++i) {
        m_sampleWindow[m_windowPosition] = samples[i] / 32768.0f; // Normalize 16-bit to [-1, 1]
        m_windowPosition = (m_windowPosition + 1) % FFT_SIZE;
    }
}

void AudioAnalyzer::performAnalysis()
{
    float soundLevel = 0.0f;
    float peakLevel = 0.0f;

    // Calculate overall sound level from the sliding window
    if (m_enableUpdateLevel) {
        float sumSquares = 0.0f;
        for (int i = 0; i < FFT_SIZE; ++i) {
            sumSquares += m_sampleWindow[i] * m_sampleWindow[i];
        }
        float rms = std::sqrt(sumSquares / FFT_SIZE);
        soundLevel = rms;
        //soundLevel = 20.0f * log10(rms + 1e-6f); // Convert to dB
    }

    // Calculate overall peak level from the sliding window
    if (m_enableUpdatePeak) {
        float maxValue = 0.0f;
        for (int i = 0; i < FFT_SIZE; ++i) {
            float absValue = std::abs(m_sampleWindow[i]);
            maxValue = std::max(maxValue, absValue);
        }
        peakLevel = maxValue;
        //peakLevel = 20.0f * log10(maxValue + 1e-6f); // Convert to dB
    }

    // Copy data from sliding window to FFT buffer, maintaining correct time order
    if (m_enableUpdateSpectrum) {
        for (int i = 0; i < FFT_SIZE; ++i) {
            int pos = (m_windowPosition + i) % FFT_SIZE;
            m_fftBuffer[i] = std::complex<float>(m_sampleWindow[pos], 0.0f);
        }
        performFFT();
    }

    emit spectrumDataReady(m_spectrum, soundLevel, peakLevel);
}

void AudioAnalyzer::performFFT()
{
    // Musical frequency bands (Hz)
    const float freqBands[SPECTRUM_BANDS + 1] = {
        20.0f,    // Sub-bass perception
        25.0f,    // Ultra-low bass
        31.5f,    // Subwoofer main
        40.0f,    // Bass fundamental 1
        50.0f,    // Bass fundamental 2
        63.0f,    // Bass playing range 1
        80.0f,    // Bass playing range 2
        100.0f,   // Kick drum main
        125.0f,   // Low vocals
        160.0f,   // Male bass voice
        200.0f,   // Male tenor
        250.0f,   // Middle C range
        315.0f,   // Female alto
        400.0f,   // Violin G string
        500.0f,   // Vocal main 1
        630.0f,   // Vocal main 2
        800.0f,   // Vocal main 3
        1000.0f,  // Vocal harmonics 1
        1250.0f,  // Vocal harmonics 2
        1600.0f,  // Presence range
        2000.0f,  // High detail 1
        2500.0f,  // High detail 2
        5000.0f,  // Brilliance
        10000.0f, // Air band
        20000.0f  // Upper limit
    };
    
    // Apply Hanning window and prepare FFT input
    for (int i = 0; i < FFT_SIZE; i++) {
        float window = 0.5f * (1.0f - cos(2.0f * M_PI * i / (FFT_SIZE - 1)));
        int pos = (m_windowPosition + i) % FFT_SIZE;
        m_fftIn[i].r = m_sampleWindow[pos] * window;
        m_fftIn[i].i = 0;
    }

    // Perform FFT
    kiss_fft(m_cfg, m_fftIn, m_fftOut);

    // Process each frequency band
    for (int band = 0; band < SPECTRUM_BANDS; ++band) {
        float sum = 0.0f;
        float freq1 = freqBands[band];
        float freq2 = freqBands[band + 1];
        
        // Convert frequencies to FFT bin indices
        int startBin = std::max(0, (int)(freq1 * FFT_SIZE / SAMPLE_RATE));
        int endBin = std::max(startBin + 1, std::min(FFT_SIZE/2, (int)(freq2 * FFT_SIZE / SAMPLE_RATE)));
        
        // Calculate power spectrum for current frequency band
        for (int i = startBin; i < endBin; ++i) {
            float real = m_fftOut[i].r;
            float imag = m_fftOut[i].i;
            float magnitude = sqrt(real * real + imag * imag);
            sum += magnitude * magnitude;
        }
        
        // Apply logarithmic compression and normalization
        float rms = sqrt(sum / (endBin - startBin));
        m_spectrum[band]  = 20.0f * log10(rms + 1e-6f); // Convert to dB
    }
}
