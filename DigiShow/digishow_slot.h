#ifndef DIGISHOWSLOT_H
#define DIGISHOWSLOT_H

#include <QObject>
#include <QVariantMap>
#include "digishow.h"

class DigishowInterface;

class DigishowSlot : public QObject
{
    Q_OBJECT

public:
    explicit DigishowSlot(QObject *parent = nullptr);
    ~DigishowSlot();

    // source and destination
    int setSource(DigishowInterface *interface, int endpointIndex);
    int setDestination(DigishowInterface *interface, int endpointIndex);
    Q_INVOKABLE DigishowInterface *sourceInterface() { return m_sourceInterface; }
    Q_INVOKABLE DigishowInterface *destinationInterface() { return m_destinationInterface; }
    Q_INVOKABLE int sourceEndpointIndex() { return m_sourceEndpointIndex; }
    Q_INVOKABLE int destinationEndpointIndex() { return m_destinationEndpointIndex; }

    // slot options
    Q_INVOKABLE int setSlotOptions(const QVariantMap &options);
    Q_INVOKABLE int setSlotOption(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariantMap getSlotOptions() { return m_slotOptions; }
    QVariantMap *slotOptions() { return &m_slotOptions; }

    // slot info
    Q_INVOKABLE QVariantMap getSlotInfo();
    dgsSlotInfo *slotInfo() { return &m_slotInfo; }

    // slot controls
    Q_INVOKABLE int setEnabled(bool enabled);
    Q_INVOKABLE bool isEnabled() { return m_enabled; }
    Q_INVOKABLE int setLinked(bool linked);
    Q_INVOKABLE bool isLinked() { return m_linked; }

    Q_INVOKABLE int getEndpointInValue();
    Q_INVOKABLE int getEndpointOutValue();
    Q_INVOKABLE bool isEndpointInBusy();
    Q_INVOKABLE bool isEndpointOutBusy();
    Q_INVOKABLE void setEndpointOutValue(int value);


signals:

public slots:

    void onDataReceived(int endpointIndex, dgsSignalData dataIn);  // incoming data is received
    void onDataPrepared(int endpointIndex, dgsSignalData dataOut); // outgoing data is pareared
    void onDataOutTimerFired();
    void onEnvelopeTimerFired();
    void onSmoothingTimerFired();

private:

    // source and destination
    DigishowInterface *m_sourceInterface;
    DigishowInterface *m_destinationInterface;
    int m_sourceEndpointIndex;
    int m_destinationEndpointIndex;

    // slot options (for read/write)
    QVariantMap m_slotOptions;

    // slot info (for fast read)
    dgsSlotInfo m_slotInfo;

    // normalize options ==> info
    void updateSlotInfoItem(const QString &name, const QVariant &value);
    void updateSlotOuputInterval();

    // slot controls
    bool m_enabled;
    bool m_linked;
    dgsSignalData m_lastDataIn;
    dgsSignalData m_lastDataOut;

    dgsSignalData processInputAnalog(dgsSignalData dataIn);
    dgsSignalData processInputBinary(dgsSignalData dataIn);
    dgsSignalData processInputNote(dgsSignalData dataIn);

    void sendDataOut(dgsSignalData dataOut);

    // time controls
    QElapsedTimer m_elapsedTimer;
    qint64 elapsed() { return m_elapsedTimer.elapsed(); }

    // for traffic managment
    int    m_outputInterval;
    qint64 m_dataInTimeLastReceived;
    qint64 m_dataOutTimeLastSent;
    QTimer m_dataOutTimer;
    bool m_needSendDataOutLater;

    // output envelope controls
    QTimer m_envelopeTimer;
    qint64 m_envelopeTimeOn;
    qint64 m_envelopeTimeOff;
    qint64 m_envelopeTimeLastUpdated;
    double m_envelopeRatio;
    dgsSignalData m_envelopeDataIn;

    void envelopeStart(dgsSignalData dataIn);
    void envelopeCancel();
    bool envelopeIsRunning();
    dgsSignalData envelopeProcessOutputAnalog();
    dgsSignalData envelopeProcessOutputBinary();

    // output smoothing controls
    QTimer m_smoothingTimer;
    qint64 m_smoothingTimeStart;
    dgsSignalData m_smoothingDataOutFrom;
    dgsSignalData m_smoothingDataOutTo;

    void smoothingStart(dgsSignalData dataOut);
    void smoothingCancel();
    bool smoothingIsRunning();
    dgsSignalData smoothingProcessOutputAnalog();
};

#endif // DIGISHOWSLOT_H
