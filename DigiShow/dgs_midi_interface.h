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

#ifndef DGSMIDIINTERFACE_H
#define DGSMIDIINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class RtMidi;
class RtMidiIn;
class RtMidiOut;

class DgsMidiInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsMidiInterface(QObject *parent = nullptr);
    ~DgsMidiInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();
    static QString getUniqueMidiPortName(RtMidi *midi, unsigned int index);

signals:

public slots:

private:
    RtMidiIn *m_midiin;
    RtMidiOut *m_midiout;

    void callbackRtMidiIn(double deltatime, std::vector< unsigned char > *message);
    static void _callbackRtMidiIn(double deltatime, std::vector< unsigned char > *message, void *userData);

    int findEndpointWidthMidiNote(int channel, int note);
    int findEndpointWidthMidiControl(int channel, int control);
    int findEndpointWidthMidiProgram(int channel);

};

#endif // DGSMIDIINTERFACE_H
