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

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <QTcpServer>


class HttpServiceHandler;

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = 0);

    void setFileDirPaths(QStringList paths);
    QStringList fileDirPaths();

    void setServiceHandler(HttpServiceHandler *handler);
    HttpServiceHandler* serviceHandler();

signals:

public slots:

private:
    QStringList _fileDirPaths;
    HttpServiceHandler *_serviceHandler;

    void incomingConnection(qintptr socketId);

};


class HttpServiceHandler : public QObject
{
    Q_OBJECT
public:
    explicit HttpServiceHandler(QObject *parent = 0) : QObject(parent) {

    }

    virtual bool processHttpRequest(
            const QString &requestPath = QString(),
            const QStringList &requestParameters = QStringList(),
            QByteArray *responseContent = nullptr,
            QString *responseContentType = nullptr) {

        Q_UNUSED(requestPath)
        Q_UNUSED(requestParameters)
        Q_UNUSED(responseContent)
        Q_UNUSED(responseContentType)

        return false;
    }
};

#endif // HTTP_SERVER_H
