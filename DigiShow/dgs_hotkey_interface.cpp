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

#include "dgs_hotkey_interface.h"

#include <QHotkey>

DgsHotkeyInterface::DgsHotkeyInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "hotkey";

    // TODO:
}


DgsHotkeyInterface::~DgsHotkeyInterface()
{
    closeInterface();
}

int DgsHotkeyInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    m_hotkeys.clear();
    for (int n=0 ; n<m_endpointOptionsList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_HOTKEY_PRESS) {

            QKeySequence key = QKeySequence(m_endpointInfoList[n].address);
            m_endpointInfoList[n].control = key[0];

            QHotkey *hotkey = new QHotkey(key, true);
            connect(hotkey, SIGNAL(activated()), this, SLOT(onHotkeyPressed()));
            connect(hotkey, SIGNAL(released()), this, SLOT(onHotkeyReleased()));
            m_hotkeys.append(hotkey);
        }
    }


    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsHotkeyInterface::closeInterface()
{
    for (int n=0 ; n<m_hotkeys.length() ; n++) {
        m_hotkeys[n]->setRegistered(false);
        disconnect(m_hotkeys[n], 0, this, 0);
        delete m_hotkeys[n];
    }
    m_hotkeys.clear();

    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsHotkeyInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    // TODO:

    return ERR_NONE;
}

void DgsHotkeyInterface::onHotkeyPressed()
{
    QHotkey *hotkey = qobject_cast<QHotkey*>(sender());
    int keyCode = static_cast<int>(hotkey->keyCode() | hotkey->modifiers());

    //qDebug() << "onHotkeyPressed" << hotkey->shortcut().toString();

    int endpointIndex = findEndpoint(keyCode);
    if (endpointIndex==-1) return;

    dgsSignalData data;
    data.signal = DATA_SIGNAL_BINARY;
    data.bValue = true;
    emit dataReceived(endpointIndex, data);
}

void DgsHotkeyInterface::onHotkeyReleased()
{
    QHotkey *hotkey = qobject_cast<QHotkey*>(sender());
    int keyCode = static_cast<int>(hotkey->keyCode() | hotkey->modifiers());

    //qDebug() << "onHotkeyReleased" << hotkey->shortcut().toString();

    int endpointIndex = findEndpoint(keyCode);
    if (endpointIndex==-1) return;

    dgsSignalData data;
    data.signal = DATA_SIGNAL_BINARY;
    data.bValue = false;
    emit dataReceived(endpointIndex, data);
}

int DgsHotkeyInterface::findEndpoint(int keyCode)
{
    for (int n=0 ; n<m_endpointInfoList.length() ; n++) {
        if (m_endpointInfoList[n].type == ENDPOINT_HOTKEY_PRESS &&
            m_endpointInfoList[n].control == keyCode) return n;
    }

    return -1;
}
