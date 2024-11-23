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

#include "http_server.h"
#include "http_socket.h"

HttpServer::HttpServer(QObject *parent) :
    QTcpServer(parent)
{
    _fileDirPaths.append("./");
    _serviceHandler = nullptr;
}

void HttpServer::setFileDirPaths(QStringList paths)
{
    if (_fileDirPaths.isEmpty()) {
        _fileDirPaths = QStringList("./");
        return;
    }

    _fileDirPaths = paths;
}

QStringList HttpServer::fileDirPaths()
{
    return _fileDirPaths;
}

void HttpServer::setServiceHandler(HttpServiceHandler *handler)
{
    _serviceHandler = handler;
}

HttpServiceHandler* HttpServer::serviceHandler()
{
    return _serviceHandler;
}


void HttpServer::incomingConnection(qintptr socketId)
{
    HttpSocket *socket = new HttpSocket(this);
    socket->setSocketDescriptor(socketId);
}
