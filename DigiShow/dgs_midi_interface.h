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
