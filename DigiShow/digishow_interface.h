#ifndef DIGISHOWINTERFACE_H
#define DIGISHOWINTERFACE_H

#include <QObject>
#include <QVariantMap>
#include "digishow.h"

class DigishowInterface : public QObject
{
    Q_OBJECT
public:
    explicit DigishowInterface(QObject *parent = nullptr);
    ~DigishowInterface();

    // interface options
    Q_INVOKABLE int setInterfaceOptions(const QVariantMap &options);
    Q_INVOKABLE int setInterfaceOption(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariantMap getInterfaceOptions() { return m_interfaceOptions; }
    QVariantMap *interfaceOptions() { return &m_interfaceOptions; }

    // interface info
    Q_INVOKABLE QVariantMap getInterfaceInfo();
    dgsInterfaceInfo *interfaceInfo() { return &m_interfaceInfo; }

    // endpoint options
    Q_INVOKABLE int addEndpoint(const QVariantMap &options, int *endpointIndex = nullptr);
    Q_INVOKABLE int setEndpointOptions(int endpointIndex, const QVariantMap &options);
    Q_INVOKABLE int setEndpointOption(int endpointIndex, const QString &name, const QVariant &value);
    Q_INVOKABLE int endpointCount();
    Q_INVOKABLE QVariantMap getEndpointOptionsAt(int endpointIndex);
    QList<QVariantMap> *endpointOptionsList() { return &m_endpointOptionsList; }

    // endpoint info
    Q_INVOKABLE QVariantMap getEndpointInfoAt(int endpointIndex);
    QList<dgsEndpointInfo> *endpointInfoList() { return &m_endpointInfoList; }

    // normalize options ==> info
    Q_INVOKABLE void updateMetadata();

    // controls
    virtual int openInterface();
    virtual int closeInterface();
    virtual int init(); // called after slot service got ready
    virtual int sendData(int endpointIndex, dgsSignalData data);

    // media
    virtual int loadMedia(const QVariantMap &mediaOptions);

    Q_INVOKABLE bool isInterfaceOpened() { return m_isInterfaceOpened; }

signals:

    void dataReceived(int endpointIndex, dgsSignalData dataIn);
    void dataPrepared(int endpointIndex, dgsSignalData dataOut);

public slots:

protected:

    // interface options (for read/write)
    QVariantMap m_interfaceOptions;
    QList<QVariantMap> m_endpointOptionsList;

    // interface info (for fast read)
    dgsInterfaceInfo m_interfaceInfo;
    QList<dgsEndpointInfo> m_endpointInfoList;

    bool m_isInterfaceOpened;

    // initialize endpoint signal
    int initEndpointValue(int endpointIndex);

    // clean up media list (delete unused media)
    // only for the interface that contains a media list
    QVariantList cleanMediaList();
};

#endif // DIGISHOWINTERFACE_H
