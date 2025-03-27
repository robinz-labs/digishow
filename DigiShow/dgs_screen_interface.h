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

#ifndef DGSSCREENINTERFACE_H
#define DGSSCREENINTERFACE_H

#include <QObject>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include "digishow_interface.h"

class DgsScreenInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsScreenInterface(QObject *parent = nullptr);
    ~DgsScreenInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    int loadMedia(const QVariantMap &mediaOptions) override;

    static QVariantList listOnline();
    static QString getUniqueScreenName(int index);

signals:

public slots:

private:

    QQmlEngine *m_qmlEngine;
    QQmlComponent *m_qmlComponentPlayer;
    QObject* m_player;

    static QString getPropertyName(int control);
    static double  getPropertyValue(int control, dgsSignalData data);

    void updateMetadata_() override;
};

#endif // DGSSCREENINTERFACE_H
