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

    Q_INVOKABLE static QString httpRequest(const QString & strUrl, const QString & strMethod = "get", const QString & strBody = "", int timeout = 3000);
    Q_INVOKABLE static QString httpUrlEncode(const QString & str);

    Q_INVOKABLE static QString createUUID();

    Q_INVOKABLE static bool makePath(const QString & path);
    Q_INVOKABLE static bool fileExists(const QString & path);

    Q_INVOKABLE static void showFileInShell(const QString & path);
    Q_INVOKABLE static void newAppInstance();

    Q_INVOKABLE static void setMacWindowIsModified(QWindow *window, bool isModified);
    Q_INVOKABLE static void setMacWindowTitleWithFile(QWindow *window, const QString &filepath);
    Q_INVOKABLE static void setMacWindowWithoutTitlebar(QWindow *window);

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
