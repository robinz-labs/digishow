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
#include <QFileInfo>
#include <QTextStream>
#include <QJsonDocument>
#include <QtNetwork>
#include <QNetworkInterface>
#include <QUrl>
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>

#include "app_utilities.h"

#ifdef Q_OS_MAC
#include "mac_utilities.h"
#endif

#ifdef Q_OS_WIN
#include <dwmapi.h>
#endif

AppUtilities::AppUtilities(QObject *parent) : QObject(parent)
{

}

AppUtilities::~AppUtilities()
{
    // used for comOpen, comSend functions
    foreach (QSerialPort *serial, m_comConnections) {
        if (serial != nullptr) {
            serial->close();
            delete serial;
        }
    }
    // used for tcpOpen, tcpSend functions
    foreach (QTcpSocket *socket, m_tcpConnections) {
        if (socket != nullptr) {
            socket->close();
            delete socket;
        }
    }
}

QByteArray AppUtilities::decodeCStyleEscapes(const QString &escapedString)
{
    QByteArray result;
    bool escapeMode = false;

    for (int i = 0; i < escapedString.length(); ++i) {
        QChar currentChar = escapedString[i];
        if (escapeMode) {
            switch (currentChar.toLatin1()) {
            case 'n': result.append('\n'); break;  // Newline
            case 'r': result.append('\r'); break;  // Carriage return
            case 't': result.append('\t'); break;  // Tab
            case '\\': result.append('\\'); break; // Backslash
            case 'x': {
                // Process \xNN (hexadecimal escape)
                if (i + 2 < escapedString.length()) {
                    QString hexCode = escapedString.mid(i + 1, 2); // Extract two hex digits
                    bool ok;
                    result.append(char(hexCode.toUInt(&ok, 16))); // Convert to byte
                    if (ok) i += 2; // Skip processed characters
                }
                break;
            }
            default:
                result.append(currentChar.toLatin1()); // Treat unknown escape literally
            }
            escapeMode = false; // Exit escape mode
        } else if (currentChar == '\\') {
            escapeMode = true; // Enter escape mode
        } else {
            result.append(currentChar.toLatin1()); // Append regular character
        }
    }

    return result;
}

QString AppUtilities::encodeCStyleEscapes(const QByteArray &rawData)
{
    QString result;

    for (char byte : rawData) {
        switch (byte) {
        case '\n':
            result.append("\\n");
            break; // Newline character
        case '\r':
            result.append("\\r");
            break; // Carriage return character
        case '\t':
            result.append("\\t");
            break; // Tab character
        case '\\':
            result.append("\\\\");
            break; // Backslash character
        default:
            // If the character is non-printable or outside the visible ASCII range,
            // encode it in \xHH format (hexadecimal)
            if (byte < 0x20 || byte > 0x7E) {
                result.append(QString("\\x%1").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')));
            } else {
                // For regular printable ASCII characters, append them as-is
                result.append(byte);
            }
        }
    }

    return result;
}

QByteArray AppUtilities::loadDataFromFile(const QString & filepath)
{
    QByteArray data;
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        stream >> data;
        file.close();
    }

    return data;
}

bool AppUtilities::saveDataToFile(const QByteArray & data, const QString & filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream stream(&file);
        stream << data;
        file.close();
        return true;
    }
    return false;
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

bool AppUtilities::clipboardExists(const QString & mimeType)
{
    const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData();
    return mimeData->hasFormat(mimeType);
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

QString AppUtilities::hostIpAddress()
{
    QString hostName = QHostInfo::localHostName();

    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QList<QHostAddress> addresses = hostInfo.addresses();

    foreach (const QHostAddress &address, addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && !address.isLoopback()) {
            return address.toString();
        }
    }

    return "127.0.0.1";
}

bool AppUtilities::udpSend(const QString & ip, int port, const QByteArray & data)
{
    QUdpSocket udpSocket;
    int len = udpSocket.writeDatagram(data, QHostAddress(ip), (quint16)port);
    return (len>0);
}

bool AppUtilities::udpSendHex(const QString & ip, int port, const QByteArray &hexstr )
{
    return udpSend(ip, port, QByteArray::fromHex(hexstr));
}

int AppUtilities::tcpOpen(const QString & ip, int port)
{
    QTcpSocket *socket = new QTcpSocket();

    socket->connectToHost(QHostAddress(ip), port);
    if (!socket->waitForConnected(5000)) {
        socket->close();
        socket->deleteLater();
        return -1;
    }

    m_tcpConnections.append(socket);

    return m_tcpConnections.length()-1;
}

bool AppUtilities::tcpSend(int index, const QByteArray & data)
{
    if (index < 0 || index > m_tcpConnections.length()-1) return false;

    QTcpSocket *socket = m_tcpConnections[index];
    if (socket == nullptr) return false;

    socket->write(data);
    socket->flush();

    //qDebug() << "tcpSend" << data;
    return true;
}

bool AppUtilities::tcpSendHex(int index, const QByteArray & hexstr)
{
    return tcpSend(index, QByteArray::fromHex(hexstr));
}

void AppUtilities::tcpClose(int index)
{
    if (index < 0 || index > m_tcpConnections.length()-1) return;

    QTcpSocket *socket = m_tcpConnections[index];
    if (socket == nullptr) return;

    socket->close();
    delete socket;
    m_tcpConnections[index] = nullptr;
}


int AppUtilities::comOpen(const QString & port, int baud, const QString & setting)
{
    QSerialPort *serial = new QSerialPort();

    serial->setPortName(port);
    if (!serial->open(QIODevice::ReadWrite)) return -1;

    serial->setBaudRate(baud);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (setting == "8N1") {
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
    } else if (setting == "8E1") {
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::EvenParity);
        serial->setStopBits(QSerialPort::OneStop);
    } else if (setting == "8O1") {
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::OddParity);
        serial->setStopBits(QSerialPort::OneStop);
    } else if (setting == "7E1") {
        serial->setDataBits(QSerialPort::Data7);
        serial->setParity(QSerialPort::EvenParity);
        serial->setStopBits(QSerialPort::OneStop);
    } else if (setting == "7O1") {
        serial->setDataBits(QSerialPort::Data7);
        serial->setParity(QSerialPort::OddParity);
        serial->setStopBits(QSerialPort::OneStop);
    } else if (setting == "8N2") {
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::TwoStop);
    } else { // default is 8N1
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
    }

    m_comConnections.append(serial);

    return m_comConnections.length()-1;
}

bool AppUtilities::comSend(int index, const QByteArray & data)
{
    if (index < 0 || index > m_comConnections.length()-1) return false;

    QSerialPort *serial = m_comConnections[index];
    if (serial == nullptr) return false;

    serial->write(data);
    serial->flush();

    //qDebug() << "comSend" << data;
    return true;
}

bool AppUtilities::comSendHex(int index, const QByteArray & hexstr)
{
    return comSend(index, QByteArray::fromHex(hexstr));
}

void AppUtilities::comClose(int index)
{
    if (index < 0 || index > m_comConnections.length()-1) return;

    QSerialPort *serial = m_comConnections[index];
    if (serial == nullptr) return;

    serial->close();
    delete serial;
    m_comConnections[index] = nullptr;
}

bool AppUtilities::isValidJson(const QString &str)
{
    if (str.isEmpty()) return false;
    QJsonParseError parseError;
    QJsonDocument::fromJson(str.toUtf8(), &parseError);
    return (parseError.error == QJsonParseError::NoError);
}

QString AppUtilities::httpRequest(const QString & strUrl, const QString & strMethod, const QString & strBody, int timeout)
{
    QString strReply;

    QNetworkRequest request(strUrl);
    QNetworkAccessManager* connection = new QNetworkAccessManager();

    QString strCcontentType = isValidJson(strBody) ? "application/json" : "text/plain; charset=utf-8";
    QNetworkReply* reply;
    if (strMethod=="post") {
        // POST
        request.setHeader(QNetworkRequest::ContentTypeHeader, strCcontentType);
        reply = connection->post(request, strBody.toUtf8());
    } else if (strMethod=="put") {
        // PUT
        request.setHeader(QNetworkRequest::ContentTypeHeader, strCcontentType);
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

QString AppUtilities::fileUrlPath(const QString & strUrl)
{

#ifdef Q_OS_WIN

    // repair the url like file://host/folder/file ==> file:////host/folder/file

    QUrl url;
    if (strUrl.startsWith("file://") && strUrl[7] != '/' && strUrl[8] != ':') {
       url = QUrl("file:////" + strUrl.mid(7));
    } else {
       url = QUrl(strUrl);
    }

#else
    QUrl url(strUrl);
#endif

    if (!url.isValid() || !url.isLocalFile()) return "";

    QString filePath = url.path();

#ifdef Q_OS_WIN

    // repair the path like /C:/folder/file ==> C:/folder/file

    if (filePath[0] == '/' && filePath[2] == ':') {
        filePath = filePath.mid(1);
    }
#endif
    return filePath;
}

QString AppUtilities::filePathUrl(const QString & path)
{
    QString strUrl = QUrl::fromLocalFile(path).toString();

#ifdef Q_OS_WIN

    // repair the url like file://host/folder/file ==> file:////host/folder/file

    if (strUrl.startsWith("file://") && strUrl[7] != '/' && strUrl[8] != ':') {
       strUrl = "file:////" + strUrl.mid(7);
    }

#endif

    return strUrl;
}

QString AppUtilities::createUUID()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool AppUtilities::makePath(const QString & path)
{
    return QDir(path).mkpath(".");
}

bool AppUtilities::fileExists(const QString & path)
{
    return QFile(path).exists();
}


bool AppUtilities::dirExists(const QString & path)
{
    return QDir(path).exists();
}

bool AppUtilities::runInShell(const QString &program, const QStringList &arguments)
{
    return QProcess::startDetached(program, arguments);
}

void AppUtilities::showFileInShell(const QString & path)
{
#ifdef Q_OS_MAC
    MacUtilities::showFileInFinder(path);
#endif
#ifdef Q_OS_WIN
    QStringList params;
    params << "/select," << QDir::toNativeSeparators(path);
    QProcess::startDetached("explorer.exe", params);
#endif
#ifdef Q_OS_LINUX
    QStringList params;
    params << QFileInfo(path).absolutePath();
    QProcess::startDetached("xdg-open", params);
#endif
}

void AppUtilities::openFileInShell(const QString & path)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void AppUtilities::newAppInstance()
{
    QString exeFilePath = QCoreApplication::applicationFilePath();
    QProcess::startDetached(exeFilePath, QStringList());
}

void AppUtilities::setWindowDarkTitlebar(QWindow *window)
{
#if defined(Q_OS_WIN) && (_WIN32_WINNT >= 0x0A00)
    if (window && window->handle()) {
        HWND hwnd = (HWND)window->winId();
        BOOL value = TRUE;
        DwmSetWindowAttribute(hwnd, 20, &value, sizeof(value)); // DWMWA_USE_IMMERSIVE_DARK_MODE
    }
#elif defined(Q_OS_MAC)
    MacUtilities::setWindowDarkTitlebar(window);
#else
    Q_UNUSED(window)
#endif

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


