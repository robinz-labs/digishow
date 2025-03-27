/*
    Copyright 2021-2015 Robin Zhang & Labs

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

#include "digishow_interface.h"

class AudioAnalyzer;

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
    void onSpectrumDataReady(const QVector<float> &spectrum, float level, float peak);

private:

    AudioAnalyzer *m_analyzer;

    void updateMetadata_() override;
};

#endif // DGSAUDIOININTERFACE_H
