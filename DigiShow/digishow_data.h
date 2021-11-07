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

#ifndef DIGISHOWDATA_H
#define DIGISHOWDATA_H

#include <QVariant>

class DigishowData : public QObject
{
    Q_OBJECT
public:
    explicit DigishowData(QObject *parent = nullptr);

    Q_INVOKABLE void setData(const QVariantMap &data) { m_data = data; }
    Q_INVOKABLE QVariantMap getData() { return m_data; }

    Q_INVOKABLE int slotCount();
    Q_INVOKABLE QVariantMap getSlotOptions(int slotIndex);
    Q_INVOKABLE QVariantMap getInterfaceOptions(const QString &interfaceName);
    Q_INVOKABLE QVariantMap getEndpointOptions(const QString &interfaceName, const QString &endpointName);
    Q_INVOKABLE QVariantMap getMediaOptions(const QString &interfaceName, const QString &mediaName);

private:

    QVariantMap m_data;
};

#endif // DIGISHOWDATA_H
