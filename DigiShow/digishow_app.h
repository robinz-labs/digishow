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

#ifndef DIGISHOWAPP_H
#define DIGISHOWAPP_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "digishow_common.h"

class DigishowInterface;
class DigishowSlot;
class DigishowMetronome;
class DigishowRemoteWeb;
class DigishowScriptable;

class DigishowApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filepath READ filepath NOTIFY filepathChanged)
    Q_PROPERTY(bool isStarting READ isStarting NOTIFY isStartingChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged)

    Q_PROPERTY(QList<DigishowSlot*> slotList READ slotList NOTIFY slotListChanged)
    Q_PROPERTY(QList<DigishowInterface*> interfaceList READ interfaceList NOTIFY interfaceListChanged)

public:
    enum MsgType {
        MsgLog   = 0,
        MsgAlert = 1,
        MsgToast = 2,
        MsgPopup = 3
    };
    Q_ENUM(MsgType)

    explicit DigishowApp(QObject *parent = nullptr);
    ~DigishowApp();

    Q_INVOKABLE void clear();

    Q_INVOKABLE void importData(const QVariantMap & data);
    Q_INVOKABLE QVariantMap exportData(const QList<int> & slotListOrder = QList<int>(), bool onlySelection = false);

    Q_INVOKABLE bool loadFile(const QString & filepath);
    Q_INVOKABLE bool saveFile(const QString & filepath = QString(), const QList<int> & slotListOrder = QList<int>(), bool onlySelection = false);

    Q_INVOKABLE QString scriptableFilePath();
    Q_INVOKABLE bool    scriptableFileExists();
    Q_INVOKABLE bool    createScriptableFile();

    Q_INVOKABLE void enableAutostart(bool autostart = true) { m_autostart = autostart; }

    Q_INVOKABLE int  start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE int  restart();
    Q_INVOKABLE void pause(bool paused);

    Q_INVOKABLE void newShow();

    Q_INVOKABLE int newInterface(const QString &interfaceType);
    Q_INVOKABLE bool deleteInterface(int interfaceIndex);

    Q_INVOKABLE int newSlot();
    Q_INVOKABLE int duplicateSlot(int slotIndex);
    Q_INVOKABLE bool deleteSlot(int slotIndex);

    Q_INVOKABLE QString filename(bool withExtension = true);
    Q_INVOKABLE QString filepath() { return m_filepath; }
    Q_INVOKABLE bool isStarting() { return m_starting; }
    Q_INVOKABLE bool isRunning() { return m_running; }
    Q_INVOKABLE bool isPaused() { return m_paused; }

    Q_INVOKABLE int interfaceCount();
    Q_INVOKABLE DigishowInterface *interfaceAt(int index);
    Q_INVOKABLE QList<DigishowInterface*> interfaceList() { return m_interfaces; }

    Q_INVOKABLE int slotCount();
    Q_INVOKABLE DigishowSlot *slotAt(int index);
    Q_INVOKABLE DigishowSlot *slotTitled(const QString &title);
    Q_INVOKABLE QList<DigishowSlot*> slotList() { return m_slots; }
    Q_INVOKABLE int getSlotIndex(DigishowSlot *slot);

    Q_INVOKABLE bool updateLaunch(const QString &launchName, const QVariantList &slotLaunchOptions);
    Q_INVOKABLE bool deleteLaunch(const QString &launchName);
    Q_INVOKABLE bool startLaunch(const QString &launchName);
    Q_INVOKABLE QVariantMap getAllLaunchOptions();
    Q_INVOKABLE QVariantMap getLaunchOptions(const QString &launchName);
    Q_INVOKABLE bool setLaunchOption(const QString &launchName, const QString &optionName, const QVariant &optionValue);
    Q_INVOKABLE QVariantList getSlotLaunchDetails(const QString &launchName);
    Q_INVOKABLE QVariantList getSlotLaunchOptions(const QString &launchName);

    Q_INVOKABLE qint64 elapsed() { return m_elapsedTimer->elapsed(); }

    Q_INVOKABLE DigishowMetronome *metronome() { return m_metronome; }
    Q_INVOKABLE DigishowRemoteWeb *remoteWeb() { return m_remoteWeb; }
    DigishowScriptable *scriptable() { return m_scriptable; }

    Q_INVOKABLE void messageNotify(const QString & msgText, int msgType = MsgAlert) {
        emit messageNotified(msgText, msgType);
    }

    bool findInterfaceAndEndpoint(const QString &name, int *pInterfaceIndex = nullptr, int *pEndpointIndex = nullptr);
    bool confirmEndpointIsEmployed(int interfaceIndex, int endpointIndex);
    bool confirmEndpointIsEmployed(const QString &interfaceName, const QString &endpointName);

signals:
    void filepathChanged();

    void isStartingChanged();
    void isRunningChanged();
    void isPausedChanged();

    void interfaceListChanged();
    void slotListChanged();
    void launchListChanged();

    void slotRefreshRequest(int slotIndex);

    void messageNotified(QString msgText, int msgType);

public slots:
    void onTimerFired();
    void onSlotMetadataUpdated();

private:

    QString m_filepath;

    QList<DigishowInterface*> m_interfaces;
    QList<DigishowSlot*> m_slots;

    QVariantMap m_launches;

    bool m_autostart;
    bool m_starting;
    bool m_running;
    bool m_paused;

    QTimer *m_timer;
    QElapsedTimer *m_elapsedTimer;

    // metronome (beat maker)
    DigishowMetronome *m_metronome;

    // web service for remote control
    DigishowRemoteWeb *m_remoteWeb;

    // scriptable
    DigishowScriptable *m_scriptable;
    bool startScriptable();
    void stopScriptable();

    // add-on process
    qint64 m_pidAddon;
    bool startAddon();
    void stopAddon();

    // conversion between the relative path and the absolute url
    QString convertFileUrlToPath(const QString &url);
    QString convertFilePathToUrl(const QString &path);
    bool validateFileUrl(const QString &url);
    bool validateFilePath(const QString &path);

};

#endif // DIGISHOWAPP_H
