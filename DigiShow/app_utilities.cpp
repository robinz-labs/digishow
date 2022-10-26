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

#include <QDebug>
#include <QUuid>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QtNetwork>
#include <QUrl>
#include <QClipboard>
#include <QGuiApplication>

#include "app_utilities.h"

#ifdef Q_OS_MAC
#include "mac_utilities.h"
#endif

AppUtilities::AppUtilities(QObject *parent) : QObject(parent)
{

}

AppUtilities::~AppUtilities()
{

}

QString AppUtilities::loadStringFromFile(const QString & filepath)
{
    QString data;
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        data = stream.readAll();
        file.close();
    }

    return data;
}

bool AppUtilities::saveStringToFile(const QString & data, const QString & filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << data;
        file.close();
        return true;
    }
    return false;
}

QVariantMap AppUtilities::loadJsonFromFile(const QString & filepath)
{
    QString strData = loadStringFromFile(filepath);
    if (strData.isNull()) return QVariantMap();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(strData.toUtf8());
    return jsonDoc.toVariant().toMap();
}

bool AppUtilities::saveJsonToFile(const QVariantMap & data, const QString & filepath)
{
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(data);
    if (jsonDoc.isNull()) return false;

    return (saveStringToFile(jsonDoc.toJson(), filepath));
}

QVariantMap AppUtilities::pasteJson(const QString & mimeType)
{
    const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData();
    if (!mimeData->hasFormat(mimeType)) return QVariantMap();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(mimeData->data(mimeType));
    return jsonDoc.toVariant().toMap();
}

bool AppUtilities::copyJson(const QVariantMap & data, const QString & mimeType)
{
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(data);
    if (jsonDoc.isNull()) return false;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(mimeType, jsonDoc.toJson());
    mimeData->setText(QString::fromUtf8(jsonDoc.toJson()));

    QGuiApplication::clipboard()->setMimeData(mimeData);
    return true;
}

/*
void AppUtilities::delay(int timeout)
{
    QEventLoop eventloop;
    QTimer::singleShot(timeout, &eventloop, SLOT(quit()));
    eventloop.exec();
}
*/

void AppUtilities::delay(int timeout)
{
    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(timeout)) {
        qApp->processEvents(QEventLoop::AllEvents);
    }
}

void AppUtilities::doEvents()
{
    qApp->processEvents(QEventLoop::AllEvents);
}

QList<QString> AppUtilities::upnpDiscover(int timeout)
{
    QList<QString> listRsp;

    QUdpSocket udp;
    QHostAddress address("239.255.255.250");
    QByteArray request = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n";
    udp.writeDatagram(request, address, 1900);

    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(timeout)) {
        if (udp.hasPendingDatagrams()) {
            QNetworkDatagram datagram = udp.receiveDatagram();
            QString strRspIp = QHostAddress(datagram.senderAddress().toIPv4Address()).toString();
            listRsp.append(strRspIp + "\r\n" + datagram.data());
        }
        doEvents();
    }

    return listRsp;
}

QString AppUtilities::upnpWaitResponse(QString strWaitRsp, int timeout)
{
    QUdpSocket udp;
    QHostAddress address("239.255.255.250");
    QByteArray request = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n";
    udp.writeDatagram(request, address, 1900);

    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(timeout)) {
        if (udp.hasPendingDatagrams()) {
            QNetworkDatagram datagram = udp.receiveDatagram();
            if (datagram.data().contains(strWaitRsp.toUtf8())) {
                QString strRspIp = QHostAddress(datagram.senderAddress().toIPv4Address()).toString();
                return strRspIp + "\r\n" + datagram.data();
            }
        }
        doEvents();
    }

    return "";
}

QString AppUtilities::httpRequest(const QString & strUrl, const QString & strMethod, const QString & strBody, int timeout)
{
    QString strReply;

    QNetworkRequest request(strUrl);
    QNetworkAccessManager* connection = new QNetworkAccessManager();

    QNetworkReply* reply;
    if (strMethod=="post") {
        // POST
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=utf-8");
        reply = connection->post(request, strBody.toUtf8());
    } else if (strMethod=="put") {
        // PUT
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=utf-8");
        reply = connection->put(request, strBody.toUtf8());
    } else {
        // GET
        reply = connection->get(request);
    }

    QTimer* timer = new QTimer();
    timer->setSingleShot(true);

    QEventLoop* loop = new QEventLoop();
    QObject::connect(timer, SIGNAL(timeout()), loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), loop, SLOT(quit()));

    timer->start(timeout);
    loop->exec();

    if(timer->isActive()) {
        timer->stop();
        if(reply->error() > 0) {
          // error
        }
        else {
          int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
          if (v >= 200 && v < 300) {
              // success
              strReply = QString(reply->readAll());
          }
        }
    } else {
       // timeout
       QObject::disconnect(reply, SIGNAL(finished()), loop, SLOT(quit()));
       reply->abort();
    }

    delete timer;
    delete loop;

    delete reply;
    delete connection;

    // qDebug() << done << strReply;

    return strReply;
}

QString AppUtilities::httpUrlEncode(const QString & str)
{
    return QString::fromUtf8(QUrl::toPercentEncoding(str));
}

Q_INVOKABLE QString AppUtilities::fileUrlPath(const QString & strUrl)
{
    QUrl url(strUrl);
    if (!url.isValid() || !url.isLocalFile()) return "";

    QString filePath = url.path();

#ifdef Q_OS_WIN
    if (filePath.startsWith("/") && filePath.mid(2,1)==":") {
        filePath = filePath.mid(1);
    }
#endif

    return filePath;
}

QString AppUtilities::createUUID()
{
    return QUuid::createUuid().toString();
}

bool AppUtilities::makePath(const QString & path)
{
    return QDir(path).mkpath(".");
}

bool AppUtilities::fileExists(const QString & path)
{
    return QFile(path).exists();
}

void AppUtilities::showFileInShell(const QString & path)
{
#ifdef Q_OS_MAC
    MacUtilities::showFileInFinder(path);
#endif
#ifdef Q_OS_WIN
    QProcess::startDetached("explorer.exe", {"/select,", QDir::toNativeSeparators(path)});
#endif
}

void AppUtilities::newAppInstance()
{
    QString exeFilePath = QCoreApplication::applicationFilePath();
    QProcess::startDetached(QString("\"%1\"").arg(exeFilePath));
}

void AppUtilities::setMacWindowIsModified(QWindow *window, bool isModified)
{
#ifdef Q_OS_MAC
    MacUtilities::setWindowIsModified(window, isModified);
#else
    Q_UNUSED(window)
    Q_UNUSED(isModified)
#endif
}

void AppUtilities::setMacWindowTitleWithFile(QWindow *window, const QString &filepath)
{
#ifdef Q_OS_MAC
    MacUtilities::setWindowTitleWithFile(window, filepath);
#else
    Q_UNUSED(window)
    Q_UNUSED(filepath)
#endif
}

void AppUtilities::setMacWindowWithoutTitlebar(QWindow *window)
{
#ifdef Q_OS_MAC
    MacUtilities::setWindowWithoutTitlebar(window);
#else
    Q_UNUSED(window)
#endif
}

bool AppUtilities::canAccessCamera()
{
#ifdef Q_OS_MAC
    return MacUtilities::canAccessCamera();
#else
    return true;
#endif
}

bool AppUtilities::canAccessMicrophone()
{
#ifdef Q_OS_MAC
    return MacUtilities::canAccessMicrophone();
#else
    return true;
#endif
}

bool AppUtilities::requestAccessCamera()
{
#ifdef Q_OS_MAC
    return MacUtilities::requestAccessCamera();
#else
    return false;
#endif
}

bool AppUtilities::requestAccessMicrophone()
{
#ifdef Q_OS_MAC
    return MacUtilities::requestAccessMicrophone();
#else
    return false;
#endif
}

