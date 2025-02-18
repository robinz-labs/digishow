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

#ifndef DGSRIOCINTERFACE_H
#define DGSRIOCINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class RiocController;

class DgsRiocInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsRiocInterface(QObject *parent = nullptr);
    ~DgsRiocInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int init() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();

signals:

public slots:
    void onStartingTimeout();
    void onUnitStarted(unsigned char unit);
    void onDigitalInValueUpdated(unsigned char unit, unsigned char channel, bool value);
    void onAnalogInValueUpdated(unsigned char unit, unsigned char channel, int value);
    void onEncoderValueUpdated(unsigned char unit, unsigned char channel, int value);
    void onUserChannelValueUpdated(unsigned char unit, unsigned char channel, int value);

private:
    RiocController *m_rioc;

    QTimer *m_timerStarting;
    int m_startedUnitCount;

    int findEndpoint(int unit, int channel);

    static QString guessRiocMode(uint16_t vid, uint16_t pid);

};

#endif // DGSRIOCINTERFACE_H
