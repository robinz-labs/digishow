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

#include "digishow_data.h"

DigishowData::DigishowData(QObject *parent) : QObject(parent)
{

}

int DigishowData::slotCount()
{
    if (!m_data.contains("slots")) return 0;
    return m_data.value("slots").toList().length();
}

QVariantMap DigishowData::getSlotOptions(int slotIndex)
{
    if (!m_data.contains("slots")) return QVariantMap();

    QVariantList slotList = m_data.value("slots").toList();
    if (slotIndex < 0 || slotIndex >= slotList.length()) return QVariantMap();

    QVariantMap slotOptions = slotList.at(slotIndex).toMap();
    slotOptions.remove("launchDetails");
    return slotOptions;
}

QVariantMap DigishowData::getInterfaceOptions(const QString &interfaceName)
{
    if (!m_data.contains("interfaces") || interfaceName.isEmpty()) return QVariantMap();

    QVariantList interfaceList = m_data.value("interfaces").toList();
    for (int n = 0 ; n < interfaceList.length() ; n++) {
        QVariantMap interfaceOptions = interfaceList.at(n).toMap();
        if (interfaceOptions.value("name") == interfaceName) {
            interfaceOptions.remove("endpoints");
            interfaceOptions.remove("media");
            return interfaceOptions;
        }
    }

    return QVariantMap();
}

QVariantMap DigishowData::getEndpointOptions(const QString &interfaceName, const QString &endpointName)
{
    if (!m_data.contains("interfaces") || interfaceName.isEmpty() || endpointName.isEmpty()) return QVariantMap();

    QVariantList interfaceList = m_data.value("interfaces").toList();
    QVariantList endpointList;
    for (int n = 0 ; n < interfaceList.length() ; n++) {
        QVariantMap interfaceOptions = interfaceList.at(n).toMap();
        if (interfaceOptions.value("name") == interfaceName) {
            endpointList = interfaceOptions.value("endpoints").toList();
            break;
        }
    }

    if (endpointList.length()>0)
    for (int n = 0 ; n < endpointList.length() ; n++) {
        QVariantMap endpointOptions = endpointList.at(n).toMap();
        if (endpointOptions.value("name") == endpointName) {
            return endpointOptions;
        }
    }

    return QVariantMap();
}

QVariantMap DigishowData::getMediaOptions(const QString &interfaceName, const QString &mediaName)
{
    if (!m_data.contains("interfaces") || interfaceName.isEmpty() || mediaName.isEmpty()) return QVariantMap();

    QVariantList interfaceList = m_data.value("interfaces").toList();
    QVariantList mediaList;
    for (int n = 0 ; n < interfaceList.length() ; n++) {
        QVariantMap interfaceOptions = interfaceList.at(n).toMap();
        if (interfaceOptions.value("name") == interfaceName) {
            mediaList = interfaceOptions.value("media").toList();
            break;
        }
    }

    if (mediaList.length()>0)
    for (int n = 0 ; n < mediaList.length() ; n++) {
        QVariantMap mediaOptions = mediaList.at(n).toMap();
        if (mediaOptions.value("name") == mediaName) {
            return mediaOptions;
        }
    }

    return QVariantMap();

}
