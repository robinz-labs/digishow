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

#ifndef MACUTILITIES_H
#define MACUTILITIES_H

#include <QObject>
#include <QWindow>

class MacUtilities : public QObject
{
    Q_OBJECT
public:
    explicit MacUtilities(QObject *parent = nullptr);

    Q_INVOKABLE static void showFileInFinder(const QString &filepath);

    Q_INVOKABLE static void setWindowIsModified(QWindow *window, bool isModified);
    Q_INVOKABLE static void setWindowTitleWithFile(QWindow *window, const QString &filepath);
    Q_INVOKABLE static void setWindowWithoutTitlebar(QWindow *window);

    Q_INVOKABLE static bool canAccessCamera();
    Q_INVOKABLE static bool canAccessMicrophone();
    Q_INVOKABLE static bool requestAccessCamera();
    Q_INVOKABLE static bool requestAccessMicrophone();

signals:

};

#endif // MACUTILITIES_H
