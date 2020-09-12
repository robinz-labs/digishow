#ifndef DGSSCREENINTERFACE_H
#define DGSSCREENINTERFACE_H

#include <QObject>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include "digishow_interface.h"

class DgsScreenInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsScreenInterface(QObject *parent = nullptr);
    ~DgsScreenInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;

    void loadMedia(const QVariantMap &mediaOptions);

    static QVariantList listOnline();

signals:

public slots:

private:

    QQmlEngine *m_qmlEngine;
    QQmlComponent *m_qmlComponentPlayer;
    QObject* m_player;

    QVariantList cleanMediaList();

    static QString getUniqueScreenName(int index);
};

#endif // DGSSCREENINTERFACE_H
