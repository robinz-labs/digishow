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

    enum SlotEndType {
        SlotOutputEnd  = 0,
        SlotInputEnd   = 1
    };
    Q_ENUM(SlotEndType)

    explicit DigishowSlot(QObject *parent = nullptr);
    ~DigishowSlot();

    int slotIndex() { return m_slotIndex; }
    void setSlotIndex(int slotIndex) { m_slotIndex = slotIndex; }

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
    Q_INVOKABLE int clearSlotOption(const QString &name);
    Q_INVOKABLE QVariantMap getSlotOptions() { return m_slotOptions; }
    QVariantMap *slotOptions() { return &m_slotOptions; }

    // slot info
    Q_INVOKABLE QVariantMap getSlotInfo();
    dgsSlotInfo *slotInfo() { return &m_slotInfo; }
    Q_INVOKABLE QString slotInputExpression() { return m_slotInputExpression; }
    Q_INVOKABLE QString slotOutputExpression() { return m_slotOutputExpression; }

    // slot controls
    Q_INVOKABLE int setEnabled(bool enabled);
    Q_INVOKABLE bool isEnabled() { return m_enabled; }
    Q_INVOKABLE int setLinked(bool linked);
    Q_INVOKABLE bool isLinked() { return m_linked; }
    Q_INVOKABLE int setSelected(bool selected);
    Q_INVOKABLE bool isSelected() { return m_selected; }

    Q_INVOKABLE int getEndpointInRange();
    Q_INVOKABLE int getEndpointOutRange();
    Q_INVOKABLE int getEndpointInValue(bool pre = false);
    Q_INVOKABLE int getEndpointOutValue(bool pre = false);
    Q_INVOKABLE bool isEndpointInBusy();
    Q_INVOKABLE bool isEndpointOutBusy();
    Q_INVOKABLE void setEndpointOutValue(int value, bool pre = true);

    Q_INVOKABLE bool hasTrafficError() { return m_trafficError; }
    Q_INVOKABLE bool hasInputExpressionError() { return m_inputExpressionError; }
    Q_INVOKABLE bool hasOutputExpressionError() { return m_outputExpressionError; }

signals:
    void metadataUpdated();

public slots:

    void onInterfaceMetadataUpdated();

    void onDataReceived(int endpointIndex, dgsSignalData dataIn);            // incoming data is received
    void onDataPrepared(int endpointIndex, dgsSignalData dataOut, bool pre); // outgoing data is pareared
    void onDataSent(int endpointIndex, dgsSignalData dataOut);               // outgoing data is sent

    void onDataOutTimerFired();
    void onEnvelopeTimerFired();
    void onSmoothingTimerFired();

private:

    int m_slotIndex;

    // source and destination
    DigishowInterface *m_sourceInterface;
    DigishowInterface *m_destinationInterface;
    int m_sourceEndpointIndex;
    int m_destinationEndpointIndex;

    // slot options (for read/write)
    QVariantMap m_slotOptions;

    // slot info (for fast read)
    dgsSlotInfo m_slotInfo;
    QString m_slotInputExpression;
    QString m_slotOutputExpression;

    // normalize options ==> info
    void updateSlotInfoItem(const QString &name, const QVariant &value);
    void updateSlotOuputInterval();

    // slot controls
    bool m_enabled;
    bool m_linked;
    bool m_selected;

    dgsSignalData m_lastDataInPre;  // input signal without expression processing
    dgsSignalData m_lastDataIn;
    dgsSignalData m_lastDataOutPre; // output signal without expression processing
    dgsSignalData m_lastDataOut;

    dgsSignalData processInputAnalog(dgsSignalData dataIn);
    dgsSignalData processInputBinary(dgsSignalData dataIn);
    dgsSignalData processInputNote(dgsSignalData dataIn);

    void prepareDataOut(dgsSignalData dataOut, bool pre = true);
    void sendDataOut(dgsSignalData dataOut, bool pre = true);

    // time controls
    QElapsedTimer m_elapsedTimer;
    qint64 elapsed() { return m_elapsedTimer.elapsed(); }

    // for traffic managment
    int    m_outputInterval;
    int    m_dataInProcessingStackLevel;
    qint64 m_dataInTimeLastReceived;
    qint64 m_dataOutTimeLastSent;
    QTimer m_dataOutTimer;
    bool   m_needSendDataOutLater;

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
    bool envelopeIsSet();
    dgsSignalData envelopeProcessOutputAnalog();
    dgsSignalData envelopeProcessOutputBinary();
    dgsSignalData envelopeProcessOutputNote();

    // output smoothing controls
    QTimer m_smoothingTimer;
    qint64 m_smoothingTimeStart;
    dgsSignalData m_smoothingDataOutFrom;
    dgsSignalData m_smoothingDataOutTo;

    void smoothingStart(dgsSignalData dataOut);
    void smoothingCancel();
    bool smoothingIsRunning();
    dgsSignalData smoothingProcessOutputAnalog();

    // expression executor
    dgsSignalData expressionExecute(const QString & expression, dgsSignalData dataIn, int slotEnd, bool *ok);

    // error flags
    bool m_trafficError;
    bool m_inputExpressionError;
    bool m_outputExpressionError;

};

#endif // DIGISHOWSLOT_H
