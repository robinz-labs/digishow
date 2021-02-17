#ifndef MACUTILITIES_H
#define MACUTILITIES_H

#include <QObject>
#include <QWindow>

class MacUtilities : public QObject
{
    Q_OBJECT
public:
    explicit MacUtilities(QObject *parent = nullptr);

    Q_INVOKABLE static void showFileInFinder(const QString &filepath);

    Q_INVOKABLE static void setWindowIsModified(QWindow *window, bool isModified);
    Q_INVOKABLE static void setWindowTitleWithFile(QWindow *window, const QString &filepath);
    Q_INVOKABLE static void setWindowWithoutTitlebar(QWindow *window);

signals:

};

#endif // MACUTILITIES_H
