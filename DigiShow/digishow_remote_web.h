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

#ifndef DIGISHOWREMOTEWEB_H
#define DIGISHOWREMOTEWEB_H

#include "http_server.h"

class DigishowRemoteWeb : public HttpServiceHandler
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning WRITE setRunning NOTIFY isRunningChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

public:
    explicit DigishowRemoteWeb(QObject *parent = nullptr);
    ~DigishowRemoteWeb();

    Q_INVOKABLE void reset();
    Q_INVOKABLE void setParameters(const QVariantMap &params);
    Q_INVOKABLE QVariantMap getParameters();

    Q_INVOKABLE bool isRunning() { return m_running; }
    Q_INVOKABLE void setRunning(bool running);
    Q_INVOKABLE int  port() { return m_port; }
    Q_INVOKABLE void setPort(int port);

    // called by http socket
    bool processHttpRequest(
            const QString &requestPath = QString(),
            const QStringList &requestParameters = QStringList(),
            QByteArray *responseContent = nullptr,
            QString *responseContentType = nullptr) override;

signals:

    void isRunningChanged();
    void portChanged();

private:
    bool m_running;
    int  m_port;

    HttpServer *m_httpServer;

};

#endif // DIGISHOWREMOTEWEB_H
