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

#ifndef DGSMESSENGERINTERFACE_H
#define DGSMESSENGERINTERFACE_H

#include "digishow_interface.h"

class AbstractMessenger;

class DgsMessengerInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsMessengerInterface(QObject *parent = nullptr);
    ~DgsMessengerInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    static QVariantList listOnline();

public slots:

    void onMessageReceived(int index);
    void onRawDataReceived(const QByteArray &data);


private:

    AbstractMessenger *m_messenger;
    QList<int> m_subscribedMessageIndexes;

    static QByteArray getMessageBytes(const QString & message, bool isHex);

    void updateMetadata_() override;
};

#endif // DGSMESSENGERINTERFACE_H
