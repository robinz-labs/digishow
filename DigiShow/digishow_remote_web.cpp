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

#include "digishow_remote_web.h"
#include "digishow_environment.h"
#include "digishow_scriptable.h"
#include "http_server.h"
#include "http_socket.h"

#define DRW_DEFAULT_PORT 8888

DigishowRemoteWeb::DigishowRemoteWeb(QObject *parent)
    : HttpServiceHandler(parent)
{
    m_running = false;
    m_port = DRW_DEFAULT_PORT;

    m_httpServer = new HttpServer();
    m_httpServer->setServiceHandler(this);
}

DigishowRemoteWeb::~DigishowRemoteWeb()
{
    delete m_httpServer;
}

void DigishowRemoteWeb::reset()
{
    setRunning(false);
    setPort(DRW_DEFAULT_PORT);
}

void DigishowRemoteWeb::setParameters(const QVariantMap &params)
{
    setPort(params.value("port", DRW_DEFAULT_PORT).toInt());
    setRunning(params.value("run", false).toBool());
}

QVariantMap DigishowRemoteWeb::getParameters()
{
    QVariantMap params;
    params["run"] = m_running;
    params["port"] = m_port;
    return params;
}

void DigishowRemoteWeb::setRunning(bool running)
{
    if (running) {
        // start or restart the server
        m_httpServer->close();
        m_httpServer->listen(QHostAddress::Any, m_port);

        QStringList fileDirPaths;
        fileDirPaths.append(DigishowEnvironment::appFilePath(g_app->filename(false) + ".web") + "/");
        fileDirPaths.append(DigishowEnvironment::appExecPath("web") + "/");
        m_httpServer->setFileDirPaths(fileDirPaths);
    } else {
        // stop the server
        m_httpServer->close();
    }

    m_running = running;
    emit isRunningChanged();
}

void DigishowRemoteWeb::setPort(int port)
{
    m_port = port;
    if (m_running) setRunning(true); // restart the server
    emit portChanged();
}

bool DigishowRemoteWeb::processHttpRequest(const QString &requestPath, const QStringList &requestParameters, QByteArray *responseContent, QString *responseContentType)
{
    if (requestPath.isEmpty()) {

        // default web page
        QString strHtml = AppUtilities::loadStringFromFile(DigishowEnvironment::appExecPath("web") + "/index.html");
        *responseContent = strHtml.toUtf8();
        *responseContentType = "text/html";
        return true;

    } else if (requestPath == "appdata") {

        QVariantMap data;

        // preset launcher data in json
        data["launches"] = g_app->getAllLaunchOptions();

        *responseContent = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact);
        *responseContentType = "application/json";
        return true;

    } else if (requestPath == "appexec") {

        QVariantMap data;

        // get parameter
        QString launch = HttpSocket::getParameter(requestParameters, "launch");
        QString script = HttpSocket::getParameter(requestParameters, "script");

        if (!launch.isEmpty()) {

            // launch a preset
            bool ok = g_app->startLaunch("launch" + launch);
            data["launch"] = launch;
            data["ok"] = ok;

        } else if (!script.isEmpty()) {

            QString context = HttpSocket::getParameter(requestParameters, "context");

            // execute a script
            DigishowScriptable* scriptable = g_app->scriptable();
            QVariant response = (context == "ui" ?
                                scriptable->executeUI(script) :  // execute the script in the app ui context
                                scriptable->execute(script));    // execute the script in the app core context
            data["script"] = script;

            if (response.canConvert<QJSValue>()) {
                QJSValue jsValue = response.value<QJSValue>();
                if (jsValue.isArray() ) { response = response.toList(); } else // convert js array to qt list
                if (jsValue.isObject()) { response = response.toMap();  }      // convert js object to qt map
            }
            data["response"] = response;
        }

        *responseContent = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact);
        *responseContentType = "application/json";
        return true;
    }

    return false;
}
