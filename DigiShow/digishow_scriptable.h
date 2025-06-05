/*
    Copyright 2024 Robin Zhang & Labs

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

#ifndef DIGISHOWSCRIPTABLE_H
#define DIGISHOWSCRIPTABLE_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>

class DigishowScriptable : public QObject
{
    Q_OBJECT
public:
    explicit DigishowScriptable(QObject *parent = nullptr);
    ~DigishowScriptable();

    void reset() { start(); }

    bool start(const QString &filepath = QString());
    void stop(bool engineOff = true);

    void engineOff();

    QVariant execute(const QString &script);
    int execute(const QString &expression, int inputValue, int inputRange, int lastValue, int slotIndex, int slotEnd, bool *ok = nullptr);

    // execute script in ui context
    QVariant executeUI(const QString &script);

signals:

private:
    QQmlEngine *m_qmlEngine;
    QQmlComponent *m_qmlComponent;
    QObject* m_qmlObject;

    bool m_uiScriptEnabled;
};

#endif // DIGISHOWSCRIPTABLE_H
