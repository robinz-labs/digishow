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

#ifndef DGSLAUNCHINTERFACE_H
#define DGSLAUNCHINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class DgsLaunchInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsLaunchInterface(QObject *parent = nullptr);
    ~DgsLaunchInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

public slots:
    void onStatusUpdated();

private:
    void updateMetadata_() override;
};


#endif // DGSLAUNCHINTERFACE_H
