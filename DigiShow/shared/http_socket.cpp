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

#include <string.h>
#include <QtNetwork>

#include "http_socket.h"
#include "http_server.h"

HttpSocket::HttpSocket(QObject *parent) :
    QTcpSocket(parent)
{   
    // init buffer
    _requestData.clear();
    _requestStartLine.clear();
    _requestHeaders.clear();
    _requestHeadersAllExtracted = false;
    _requestBody.clear();
    _requestBodySize = 0;

    // create connections
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

HttpSocket::~HttpSocket()
{

}

void HttpSocket::readClient()
{
    // read incoming data
    char buf[0x10000];
    QDataStream in(this);
    while (true) {
        int len = in.readRawData(buf, sizeof(buf)-1);
        if (len>0) {
            _requestData.append(buf, len);
        } else break;
    }

    // extract http request header
    if (!_requestHeadersAllExtracted) {

        int p0 = 0;
        while (!_requestHeadersAllExtracted) {

            // finding line break
            int p1 = _requestData.indexOf("\n", p0);

            if (p1 == -1) {

                // no more line found
                break;

            } else {

                // a line found
                QByteArray line = _requestData.mid(p0, p1-p0).trimmed();

                if (p0==0 && _requestStartLine.isEmpty()) {

                    // extract start-line
                    _requestStartLine = QString::fromUtf8(line);

                } else if (!line.isEmpty()) {

                    // extract headers
                    int sep = line.indexOf(":");
                    if (sep >= 0) {
                        QString key = QString::fromUtf8(line.left(sep).trimmed());
                        QString val = QString::fromUtf8(line.mid(sep+1).trimmed());
                        _requestHeaders[key] = val;

                        if (key == "Content-Length") _requestBodySize = val.toInt();
                    }

                } else if (line.isEmpty()) {

                    // all headers are extracted
                    // get ready to extract body
                    _requestHeadersAllExtracted = true;
                    _requestBody.clear();
                }

                // go ahead or finish
                p0 = p1+1;
                if (p0 >= _requestData.length()) break;
            }
        }

        _requestData = _requestData.mid(p0);
    }

    // extract http request body
    if (_requestHeadersAllExtracted) {
        _requestBody.append(_requestData);
        _requestData.clear();
    }

    // confirm all got ready to process the request
    if ((_requestBodySize==0 && _requestHeadersAllExtracted) ||
        (_requestBodySize >0 && _requestBody.length() >= _requestBodySize)) {
        processHttpRequest();
    }
}

void HttpSocket::processHttpRequest()
{    

    QString strRequest;         // example: /myapp/test.js?id=12345
    QString strContentPath;     //          myapp/test.js
    QString strContentType;     //          application/javascript
    QStringList lstParameters;  //          id=12345

    // process http head
    QStringList lstRequest = _requestStartLine.split(" ");

    if (lstRequest.count()>=2 && (lstRequest[0]=="GET" || lstRequest[0]=="POST")) {
        strRequest = lstRequest[1];

        if (strRequest.startsWith("/")) {

            QStringList lstRequestParts = strRequest.split("?");

            // get content path
            strContentPath = lstRequestParts[0].mid(1); // remove heading slash

            // determine content type
            strContentType = getFileType(strContentPath);

            // get query parameters
            if (lstRequestParts.count()==2) {
                lstParameters = lstRequestParts[1].split("&"); // get parameters
            }
        }
    }

    // process http body (optional for POST method only)
    if (lstRequest[0]=="POST" &&
        !_requestBody.isEmpty() &&
        _requestHeaders["Content-Type"].contains("x-www-form-urlencoded")) {
        lstParameters.append(QString::fromUtf8(_requestBody).split("&"));
    }

    // prepare and outuput http response
    bool found = false;
    QDataStream out(this);

    if (!strContentPath.isEmpty() && !strContentType.isEmpty()) {

        // transfer a file
        QStringList fileDirPaths = ((HttpServer*)parent())->fileDirPaths();
        foreach (const QString &strDirPath, fileDirPaths) {
            QString strFilePath = strDirPath + strContentPath;
            if (!strContentPath.contains("../") && !strContentPath.contains("/..") &&
                QFile::exists(strFilePath)) {
                transferFile(out, strFilePath, strContentType);
                found = true;
                break;
            }
        }

    } else if (strContentType.isEmpty()) {

        // get service response
        HttpServiceHandler *serviceHandler = ((HttpServer*)parent())->serviceHandler();

        QByteArray responseContent;
        QString responseContentType = "text/plain";

        if (serviceHandler->processHttpRequest(strContentPath, lstParameters, &responseContent, &responseContentType)) {
            outputHttpHead(out, responseContentType);
            outputHttpContent(out, responseContent);
            found = true;
        }
    }

    if (!found) outputHttp404(out); // 404 not found

    close();
}

void HttpSocket::transferFile(QDataStream & out, const QString & strFilePath, const QString & strFileType)
{
    QString strHttpHeadFileType = (strFileType.isNull() ? "text/html" : strFileType);
    if (strHttpHeadFileType.startsWith("text/")) strHttpHeadFileType += "; charset=utf-8";

    QString strHttpHead = "HTTP/1.0 200 OK\r\nContent-Type: " + strHttpHeadFileType + "\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    QByteArray bytes = strHttpHead.toUtf8();
    out.writeRawData(bytes.constData(), bytes.size());

    QFile file(strFilePath);
    if (file.open(QIODevice::ReadOnly)) {

        char buf[1024];
        while (!file.atEnd()) {
            int len = file.read(buf, sizeof(buf));
            if (len > 0)
                out.writeRawData(buf, len);
        }

        file.close();
    }
}

void HttpSocket::outputHttpHead(QDataStream & out, const QString & strFileType)
{
    QString strHttpHeadFileType = (strFileType.isNull() ? "text/html" : strFileType);
    if (strHttpHeadFileType.startsWith("text/")) strHttpHeadFileType += "; charset=utf-8";

    QString strHttpHead =
            "HTTP/1.0 200 OK\r\nContent-Type: " +
            strHttpHeadFileType +
            "\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    QByteArray bytes = strHttpHead.toUtf8();
    out.writeRawData(bytes.constData(), bytes.size());
}

void HttpSocket::outputHttpContent(QDataStream & out, const QByteArray &content)
{
    out.writeRawData(content.constData(), content.size());
}

void HttpSocket::outputHttp404(QDataStream & out)
{
    QString strHttpHead =
            "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\nHTTP 404";
    QByteArray bytes = strHttpHead.toUtf8();
    out.writeRawData(bytes.constData(), bytes.size());
}


QString HttpSocket::getFileType(const QString & strFileName)
{
    QString strFileName1 = strFileName.toLower();

    if (strFileName1.endsWith(".html") || strFileName1.endsWith(".htm"))
                                             return "text/html";
    else if (strFileName1.endsWith(".txt" )) return "text/plain";
    else if (strFileName1.endsWith(".xml" )) return "text/xml";
    else if (strFileName1.endsWith(".css" )) return "text/css";
    else if (strFileName1.endsWith(".js"  )) return "application/javascript";
    else if (strFileName1.endsWith(".json")) return "application/json";
    else if (strFileName1.endsWith(".zip" )) return "application/zip";
    else if (strFileName1.endsWith(".swf" )) return "application/x-shockwave-flash";
    else if (strFileName1.endsWith(".gif" )) return "image/gif";
    else if (strFileName1.endsWith(".jpg" )) return "image/jpeg";
    else if (strFileName1.endsWith(".png" )) return "image/png";

    else if (strFileName1.contains("." ) && !strFileName1.endsWith("/" ))
                                             return "application/octet-stream";

    return NULL;
}

QString HttpSocket::getParameter(const QStringList & lstParameters, const QString & strKey)
{
    if (!lstParameters.isEmpty()){
        for (int n=0 ; n<lstParameters.count() ; n++) {
            if (lstParameters[n].startsWith(strKey+"=")) {
                QString str = lstParameters[n].mid(strKey.length()+1).replace('+', "%20");
                return QUrl::fromPercentEncoding(str.toUtf8());
            }

        }
    }

    return NULL;
}
