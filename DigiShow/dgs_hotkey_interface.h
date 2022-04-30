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

#ifndef DGSHOTKEYINTERFACE_H
#define DGSHOTKEYINTERFACE_H

#include "digishow_interface.h"

class QHotkey;

class DgsHotkeyInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsHotkeyInterface(QObject *parent = nullptr);
    ~DgsHotkeyInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

signals:

public slots:

    void onHotkeyPressed();
    void onHotkeyReleased();

private:

    QList<QHotkey*> m_hotkeys;

    int findEndpoint(int keyCode);
};

#endif // DGSHOTKEYINTERFACE_H
