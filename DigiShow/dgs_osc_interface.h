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

#ifndef DGSOSCINTERFACE_H
#define DGSOSCINTERFACE_H

#include <QObject>
#include <QUdpSocket>
#include "digishow_interface.h"

class DgsOscInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsOscInterface(QObject *parent = nullptr);
    ~DgsOscInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

signals:

public slots:
    void onUdpDataReceived(); // for input
    void onTimerFired();      // for output

private:

    QUdpSocket *m_udp;
    QHostAddress m_udpHost;
    int m_udpPort;

    // timer for osc message output
    QTimer *m_timer;
    bool m_outputContinuous;

    // data buffer of all addresses
    QVariantMap m_dataAll;
    QStringList m_dataUpdatedAddresses;
};

#endif // DGSOSCINTERFACE_H
