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

#ifndef HTTP_SOCKET_H
#define HTTP_SOCKET_H

#include <QTcpSocket>
#include <QMap>

class HttpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit HttpSocket(QObject *parent = 0);
    ~HttpSocket();

    static QString getFileType(const QString & strFileName);
    static QString getParameter(const QStringList & lstParameters, const QString & strKey);

signals:

public slots:
    void readClient();

private:

    QByteArray _requestData;
    QString _requestStartLine;
    QMap<QString, QString> _requestHeaders;
    bool _requestHeadersAllExtracted;
    QByteArray _requestBody;
    int _requestBodySize;

    void processHttpRequest();
    void transferFile(QDataStream & out, const QString & strFilePath, const QString & strFileType);
    void outputHttpHead(QDataStream & out, const QString & strFileType =  NULL);
    void outputHttpContent(QDataStream & out, const QByteArray & content);
    void outputHttp404(QDataStream & out);
};

#endif // HTTP_SOCKET_H
