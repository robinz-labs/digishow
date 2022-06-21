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

#ifndef DGSAUDIOININTERFACE_H
#define DGSAUDIOININTERFACE_H

#include <QScopedPointer>
#include <QAudioInput>
#include "digishow_interface.h"

class SoundLevelMeter;

class DgsAudioinInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsAudioinInterface(QObject *parent = nullptr);
    ~DgsAudioinInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();
    static QString getUniqueAudioinName(int index);

signals:

public slots:

private:

    QScopedPointer<SoundLevelMeter> m_soundLevelMeter;
    QScopedPointer<QAudioInput> m_audioInput;
};

class SoundLevelMeter : public QIODevice
{
    Q_OBJECT

public:
    SoundLevelMeter(const QAudioFormat &format);

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    const QAudioFormat m_format;
    quint32 m_maxAmplitude = 0;
    qreal m_level = 0.0; // 0.0 <= m_level <= 1.0

signals:
    void update();
};

#endif // DGSAUDIOININTERFACE_H
