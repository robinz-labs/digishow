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
    void onUnitStarted(unsigned char unit);
    void onDigitalInValueUpdated(unsigned char unit, unsigned char channel, bool value);
    void onAnalogInValueUpdated(unsigned char unit, unsigned char channel, int value);
    void onEncoderValueUpdated(unsigned char unit, unsigned char channel, int value);

private:
    RiocController *m_rioc;

    int findEndpoint(int unit, int channel);

    static QString guessRiocMode(uint16_t vid, uint16_t pid);

};

#endif // DGSRIOCINTERFACE_H
