#ifndef DIGISHOWDATA_H
#define DIGISHOWDATA_H

#include <QVariant>

class DigishowData : public QObject
{
    Q_OBJECT
public:
    explicit DigishowData(QObject *parent = nullptr);

    Q_INVOKABLE void setData(const QVariantMap &data) { m_data = data; }
    Q_INVOKABLE QVariantMap getData() { return m_data; }

    Q_INVOKABLE int slotCount();
    Q_INVOKABLE QVariantMap getSlotOptions(int slotIndex);
    Q_INVOKABLE QVariantMap getInterfaceOptions(const QString &interfaceName);
    Q_INVOKABLE QVariantMap getEndpointOptions(const QString &interfaceName, const QString &endpointName);
    Q_INVOKABLE QVariantMap getMediaOptions(const QString &interfaceName, const QString &mediaName);

private:

    QVariantMap m_data;
};

#endif // DIGISHOWDATA_H
