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

#ifndef APP_UTILITIES_H
#define APP_UTILITIES_H

#include <QObject>
#include <QVariant>
#include <QElapsedTimer>
#include <QWindow>

class AppUtilities : public QObject
{
    Q_OBJECT
public:
    explicit AppUtilities(QObject *parent = nullptr);
    ~AppUtilities();


    Q_INVOKABLE static QString loadStringFromFile(const QString & filepath);
    Q_INVOKABLE static bool saveStringToFile(const QString & data, const QString & filepath);

    Q_INVOKABLE static QVariantMap loadJsonFromFile(const QString & filepath);
    Q_INVOKABLE static bool saveJsonToFile(const QVariantMap & data, const QString & filepath);

    Q_INVOKABLE static QVariantMap pasteJson(const QString & mimeType = "application/json");
    Q_INVOKABLE static bool copyJson(const QVariantMap & data, const QString & mimeType = "application/json");

    Q_INVOKABLE static void delay(int timeout);
    Q_INVOKABLE static void doEvents();

    Q_INVOKABLE static QList<QString> upnpDiscover(int timeout = 5000);
    Q_INVOKABLE static QString upnpWaitResponse(QString strWaitRsp, int timeout = 5000);

    Q_INVOKABLE static QString hostIpAddress();

    Q_INVOKABLE static QString httpRequest(const QString & strUrl, const QString & strMethod = "get", const QString & strBody = "", int timeout = 3000);
    Q_INVOKABLE static QString httpUrlEncode(const QString & str);
    Q_INVOKABLE static QString fileUrlPath(const QString & url);
    Q_INVOKABLE static QString filePathUrl(const QString & path);

    Q_INVOKABLE static QString createUUID();

    Q_INVOKABLE static bool makePath(const QString & path);
    Q_INVOKABLE static bool fileExists(const QString & path);
    Q_INVOKABLE static bool dirExists(const QString & path);

    Q_INVOKABLE static bool runInShell(const QString &program, const QStringList &arguments = QStringList());
    Q_INVOKABLE static void showFileInShell(const QString & path);
    Q_INVOKABLE static void openFileInShell(const QString & path);
    Q_INVOKABLE static void newAppInstance();

    Q_INVOKABLE static void setMacWindowIsModified(QWindow *window, bool isModified);
    Q_INVOKABLE static void setMacWindowTitleWithFile(QWindow *window, const QString &filepath);
    Q_INVOKABLE static void setMacWindowWithoutTitlebar(QWindow *window);

    Q_INVOKABLE static bool canAccessCamera();
    Q_INVOKABLE static bool canAccessMicrophone();
    Q_INVOKABLE static bool requestAccessCamera();
    Q_INVOKABLE static bool requestAccessMicrophone();

    Q_INVOKABLE static bool isMac() {
#ifdef Q_OS_MAC
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE static bool isWindows() {
#ifdef Q_OS_WIN
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE static bool isLinux() {
#ifdef Q_OS_LINUX
        return true;
#else
        return false;
#endif
    }

signals:

public slots:

};

#endif // APP_UTILITIES_H
