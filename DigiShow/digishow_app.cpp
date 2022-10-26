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

#include "digishow_app.h"
#include "digishow_environment.h"
#include "digishow_interface.h"
#include "digishow_slot.h"
#include "digishow_metronome.h"
#include "dgs_midi_interface.h"
#include "dgs_rioc_interface.h"
#include "dgs_modbus_interface.h"
#include "dgs_hue_interface.h"
#include "dgs_dmx_interface.h"
#include "dgs_artnet_interface.h"
#include "dgs_osc_interface.h"
#include "dgs_audioin_interface.h"
#include "dgs_screen_interface.h"
#include "dgs_pipe_interface.h"
#include "dgs_launch_interface.h"
#include "dgs_hotkey_interface.h"
#include "dgs_metronome_interface.h"

#ifdef DIGISHOW_EXPERIMENTAL
#include "dgs_aplay_interface.h"
#include "dgs_mplay_interface.h"
#endif

DigishowApp::DigishowApp(QObject *parent) : QObject(parent)
{
    m_autostart = false;
    m_starting = false;
    m_running = false;
    m_paused = false;

    // start a timer to handle the periodic actions
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerFired()));
    m_timer->setSingleShot(false);
    m_timer->setInterval(1000);
    m_timer->start();

    // create a metronome
    m_metronome = new DigishowMetronome();
    m_metronome->setLinkEnabled(true);

    // list all available online ports and write to file
    // AppUtilities::saveJsonToFile(
    //             DigishowEnvironment::listOnline(),
    //             DigishowEnvironment::appDataPath("online.json"));

#ifdef QT_DEBUG
    qDebug() << "app created";
#endif
}

DigishowApp::~DigishowApp()
{
    m_metronome->deleteLater();

    m_timer->stop();
    m_timer->deleteLater();

#ifdef QT_DEBUG
    qDebug() << "app released";
#endif
}

void DigishowApp::clear()
{
    m_launches.clear();
    emit launchListChanged();

    for (int n=0 ; n<m_slots.length() ; n++) delete m_slots[n];
    m_slots.clear();
    emit slotListChanged();

    for (int n=0 ; n<m_interfaces.length() ; n++) delete m_interfaces[n];
    m_interfaces.clear();
    emit interfaceListChanged();

    // reset metronome parameters
    m_metronome->reset();
}

void DigishowApp::importData(const QVariantMap & data)
{
    // clear all in the app environment
    clear();

    // set up metronome
    m_metronome->setParameters(data.value("metronome").toMap());

    // set up interfaces
    QVariantList dataInterfaces = data.value("interfaces").toList();
    for (int n=0 ; n<dataInterfaces.length() ; n++) {

        // extract options of the interface and all included endpoints
        QVariantMap dataInterface = dataInterfaces[n].toMap();
        QVariantList dataEndpoints = dataInterface.value("endpoints").toList();
        dataInterface.remove("endpoints");

        // repair media list
        if (dataInterface.contains("media")) {
            QVariantList dataMediaList = dataInterface.value("media").toList();
            for (int i=0 ; i<dataMediaList.length() ; i++) {
                QVariantMap dataMedia = dataMediaList[i].toMap();
                QString url = dataMedia.value("url").toString();
                QString file = dataMedia.value("file").toString();
                if (validateFilePath(file)) {
                    //if (url.isEmpty() || (QUrl(url).isLocalFile() && !validateFileUrl(url))) {
                        dataMedia["url"] = convertFilePathToUrl(file);
                    //}
                }
                dataMediaList[i] = dataMedia;
            }
            dataInterface["media"] = dataMediaList;
        }

        // set up interface object
        QString interfaceType = dataInterface.value("type").toString();
        DigishowInterface *interface;

        // note:
        // all interface objects created here must be set the parent for keeping the ownership
        // to avoid it released in qml accidently !!!

        if      (interfaceType=="midi"     ) interface = new DgsMidiInterface(this);
        else if (interfaceType=="rioc"     ) interface = new DgsRiocInterface(this);
        else if (interfaceType=="modbus"   ) interface = new DgsModbusInterface(this);
        else if (interfaceType=="hue"      ) interface = new DgsHueInterface(this);
        else if (interfaceType=="dmx"      ) interface = new DgsDmxInterface(this);
        else if (interfaceType=="artnet"   ) interface = new DgsArtnetInterface(this);
        else if (interfaceType=="osc"      ) interface = new DgsOscInterface(this);
        else if (interfaceType=="audioin"  ) interface = new DgsAudioinInterface(this);
        else if (interfaceType=="screen"   ) interface = new DgsScreenInterface(this);
        else if (interfaceType=="pipe"     ) interface = new DgsPipeInterface(this);
        else if (interfaceType=="launch"   ) interface = new DgsLaunchInterface(this);
        else if (interfaceType=="hotkey"   ) interface = new DgsHotkeyInterface(this);
        else if (interfaceType=="metronome") interface = new DgsMetronomeInterface(this);
#ifdef DIGISHOW_EXPERIMENTAL
        else if (interfaceType=="aplay"    ) interface = new DgsAPlayInterface(this);
        else if (interfaceType=="mplay"    ) interface = new DgsMPlayInterface(this);
#endif
        else                                 interface = new DigishowInterface(this);

        interface->setInterfaceOptions(dataInterface);
        for (int m=0 ; m<dataEndpoints.length() ; m++) {
            QVariantMap dataEndpoint = dataEndpoints[m].toMap();
            interface->addEndpoint(dataEndpoint);
        }

        // build metadata after all options have been set
        interface->updateMetadata();

        // apply to the app environment
        m_interfaces.append(interface);
    }

    // set up slots
    QVariantList dataSlots = data.value("slots").toList();
    for (int n=0 ; n<dataSlots.length() ; n++) {

        // extract slot options
        QVariantMap dataSlot = dataSlots[n].toMap();

        QString src = dataSlot.value("source").toString();
        QString dst = dataSlot.value("destination").toString();

        int  srcInterfaceIndex = -1;
        int  srcEndpointIndex  = -1;
        bool srcAssigned = findInterfaceAndEndpoint(src, &srcInterfaceIndex, &srcEndpointIndex);

        int  dstInterfaceIndex = -1;
        int  dstEndpointIndex  = -1;
        bool dstAssigned = findInterfaceAndEndpoint(dst, &dstInterfaceIndex, &dstEndpointIndex);

        // set up slot object

        // note:
        // all slot objects created here must be set the parent for keeping the ownership
        // to avoid it released in qml accidently !!!

        DigishowSlot *slot = new DigishowSlot(this);
        if (srcAssigned) slot->setSource     (m_interfaces[srcInterfaceIndex], srcEndpointIndex);
        if (dstAssigned) slot->setDestination(m_interfaces[dstInterfaceIndex], dstEndpointIndex);
        slot->setSlotOptions(dataSlot);

        // set linked flag
        slot->setLinked(dataSlot.value("linked", true).toBool());

        // apply to the app environment
        m_slots.append(slot);
    }

    // set up launches
    if (data.contains("launches")) m_launches = data.value("launches").toMap();

    // notify ui data change
    emit interfaceListChanged();
    emit slotListChanged();
    emit launchListChanged();
}

QVariantMap DigishowApp::exportData(const QList<int> & slotListOrder, bool onlySelection)
{
    // prepare data of interfaces
    QVariantList dataInterfaces;
    for (int n=0 ; n<m_interfaces.length() ; n++) {

        QVariantMap dataInterface = QVariantMap(*m_interfaces[n]->interfaceOptions());

        // repair endpoint list
        QList<QVariantMap> dataEndpointsList = QList<QVariantMap>(*m_interfaces[n]->endpointOptionsList());
        QVariantList dataEndpoints;
        for (int i=0 ; i<dataEndpointsList.length() ; i++) {
            if (confirmEndpointIsEmployed(n, i))
                dataEndpoints.append(dataEndpointsList[i]); // remove the endpoint that is not employed
        }
        dataInterface["endpoints"] = dataEndpoints;

        // repair media list
        if (dataInterface.contains("media")) {
            QVariantList dataMediaList = dataInterface.value("media").toList();
            for (int i=0 ; i<dataMediaList.length() ; i++) {
                QVariantMap dataMedia = dataMediaList[i].toMap();
                dataMedia["file"] = convertFileUrlToPath(dataMedia.value("url").toString());
                dataMediaList[i] = dataMedia;
            }
            dataInterface["media"] = dataMediaList;
        }

        dataInterfaces.append(dataInterface);
    }

    // prepare data of slots
    QVariantList dataSlots;
    bool isCustomizedOrder = (!slotListOrder.isEmpty() && slotListOrder.length() == m_slots.length());
    for (int n=0 ; n<m_slots.length() ; n++) {

        DigishowSlot* slot = (isCustomizedOrder ? m_slots[slotListOrder[n]] : m_slots[n]);
        QVariantMap dataSlot = QVariantMap(*slot->slotOptions());

        dataSlot["linked"] = slot->isLinked(); // save linked flag

        if (!onlySelection || slot->isSelected())
            dataSlots.append(dataSlot);
    }

    // write data
    QVariantMap data;
    data["interfaces"] = dataInterfaces;
    data["slots"] = dataSlots;
    data["launches"] = m_launches;
    data["metronome"] = m_metronome->getParameters();

    return data;
}

bool DigishowApp::loadFile(const QString & filepath)
{
    // confirm the system is not running
    if (m_running) return false;

    // confirm the file exists
    if (!AppUtilities::fileExists(filepath)) return false;

    // load data from the file
    QVariantMap data = AppUtilities::loadJsonFromFile(filepath);
    if (!data.contains("interfaces") || !data.contains("slots")) return false;

    m_filepath = filepath;

    // process the data
    importData(data);

    emit filepathChanged();

    // auto start
    if (m_autostart) start();

    return true;
}

bool DigishowApp::saveFile(const QString & filepath, const QList<int> & slotListOrder, bool onlySelection)
{
    // confirm filepath is set
    QString filepath1 = (filepath.isEmpty() ? m_filepath : filepath);
    if (filepath1.isEmpty()) return false;

    // prepare the data
    QVariantMap data = exportData(slotListOrder, onlySelection);

    // app version info
    QVariantMap info;
    info["appName"   ] = DigishowEnvironment::appName() + (DigishowEnvironment::appExperimental() ? " +" : "");
    info["appVersion"] = DigishowEnvironment::appVersion();
    data["appInfo"   ] = info;

    // save data to file
    if (!AppUtilities::saveJsonToFile(data, filepath1)) return false;

    m_filepath = filepath1;
    emit filepathChanged();
    return true;

}

int DigishowApp::start()
{
    if (m_running) return -1;

    // request privacy access permission
    for (int n=0 ; n<m_interfaces.length() ; n++) {
        if (m_interfaces[n]->interfaceOptions()->value("type") == "audioin") {
            if (AppUtilities::requestAccessMicrophone()) {
                messageNotify(tr("Requesting access to your microphone ..."), MsgToast);
                return -2;
            }
        }
    }

    // starting
    m_starting = true;
    emit isStartingChanged();

    int hasError = 0; // interface opening error count

    // open all interface    
    for (int n=0 ; n<m_interfaces.length() ; n++) {

        // close the interface if it is already opened
        if (m_interfaces[n]->isInterfaceOpened())
            m_interfaces[n]->closeInterface();

        // repair endpoints of the interface (disable endpoints that are not employed by any slot)
        for (int i=0 ; i<m_interfaces[n]->endpointCount() ; i++) {
            if (!confirmEndpointIsEmployed(n, i))
                m_interfaces[n]->setEndpointOption(i, "enabled", false);
        }

        // open the interface
        int err = m_interfaces[n]->openInterface();
        if (err == ERR_NONE) {

            qInfo("interface #%d %s ( %s ) opened: type=%s(%d) mode=%s(%d) endpoints=%d", n,
                  m_interfaces[n]->interfaceOptions()->value("name").toString().toLocal8Bit().constData(),
                  m_interfaces[n]->interfaceInfo()->label.toLocal8Bit().constData(),
                  m_interfaces[n]->interfaceOptions()->value("type").toString().toLocal8Bit().constData(),
                  m_interfaces[n]->interfaceInfo()->type,
                  m_interfaces[n]->interfaceOptions()->value("mode").toString().toLocal8Bit().constData(),
                  m_interfaces[n]->interfaceInfo()->mode,
                  m_interfaces[n]->endpointCount());
        } else {

            qWarning("interface #%d %s: error=%d", n,
                     m_interfaces[n]->interfaceOptions()->value("name").toString().toLocal8Bit().constData(),
                     err);
            messageNotify(tr("Error occurred when open interface %1 .").arg(m_interfaces[n]->interfaceInfo()->label));
            hasError++;
        }
    }

    // enable all slots
    for (int n=0 ; n<m_slots.length() ; n++) {

         int err = m_slots[n]->setEnabled(true);
         if (err == ERR_NONE) {

             dgsEndpointInfo srcEpInfo = m_slots[n]->sourceInterface()->endpointInfoList()->at(m_slots[n]->sourceEndpointIndex());
             dgsEndpointInfo dstEpInfo = m_slots[n]->destinationInterface()->endpointInfoList()->at(m_slots[n]->destinationEndpointIndex());

             qInfo("slot #%d enabled: source=[%c] %s ( %s %s ) \tdestination=[%c] %s ( %s %s )", n,
                   m_slots[n]->slotInfo()->inputSignal,
                   m_slots[n]->slotOptions()->value("source").toString().toLocal8Bit().constData(),
                   srcEpInfo.labelEPT.toUtf8().constData(),
                   srcEpInfo.labelEPI.toUtf8().constData(),
                   m_slots[n]->slotInfo()->outputSignal,
                   m_slots[n]->slotOptions()->value("destination").toString().toLocal8Bit().constData(),
                   dstEpInfo.labelEPT.toUtf8().constData(),
                   dstEpInfo.labelEPI.toUtf8().constData());
         }
    }

    // started
    m_starting = false;
    emit isStartingChanged();

    m_running = true;
    emit isRunningChanged();

    // initialize all interfaces
    for (int n=0 ; n<m_interfaces.length() ; n++) m_interfaces[n]->init();

    return hasError;
}

void DigishowApp::stop()
{
    if (!m_running) return;

    // disable all slots
    for (int n=0 ; n<m_slots.length() ; n++) {

        m_slots[n]->setEnabled(false);
    }

    // close all interface
    for (int n=0 ; n<m_interfaces.length() ; n++) {

        m_interfaces[n]->closeInterface();
    }

    m_running = false;
    emit isRunningChanged();
}

void DigishowApp::pause(bool paused)
{
    // link or unlink all slots
    for (int n=0 ; n<m_slots.length() ; n++) {

        m_slots[n]->setLinked(!paused);
    }

    m_paused = paused;
    emit isPausedChanged();
}

void DigishowApp::newShow()
{
    stop();
    clear();

    m_filepath.clear();
    emit filepathChanged();

    newInterface("metronome");
    newInterface("hotkey");
    newInterface("launch");
    newInterface("pipe");
    emit interfaceListChanged();
}

int DigishowApp::newInterface(const QString &interfaceType)
{
    // create a new interface object
    DigishowInterface *interface;

    if (interfaceType=="midi")   {

        interface = new DgsMidiInterface(this);
        interface->setInterfaceOption("mode", "output");
        interface->setInterfaceOption("outputInterval", 20);

    } else if (interfaceType=="rioc") {

        interface = new DgsRiocInterface(this);
        interface->setInterfaceOption("mode", "general");
        interface->setInterfaceOption("outputInterval", 20);

    } else if (interfaceType=="modbus") {

        interface = new DgsModbusInterface(this);
        interface->setInterfaceOption("mode", "rtu");
        interface->setInterfaceOption("comBaud", 9600);
        interface->setInterfaceOption("comParity", "8N1");
        interface->setInterfaceOption("outputInterval", 20);

    } else if (interfaceType=="hue") {

        interface = new DgsHueInterface(this);
        interface->setInterfaceOption("mode", "");
        //interface->setInterfaceOption("outputInterval", 150);

    } else if (interfaceType=="dmx") {

        interface = new DgsDmxInterface(this);
        interface->setInterfaceOption("mode", "enttec");
        //interface->setInterfaceOption("outputInterval", 20);

    } else if (interfaceType=="osc") {

        interface = new DgsOscInterface(this);
        interface->setInterfaceOption("mode", "input");
        interface->setInterfaceOption("udpPort", 8000);

    } else if (interfaceType=="artnet") {

        interface = new DgsArtnetInterface(this);
        interface->setInterfaceOption("mode", "input");
        interface->setInterfaceOption("udpPort", 6454);

    } else if (interfaceType=="pipe") {

        interface = new DgsPipeInterface(this);
        interface->setInterfaceOption("mode", "local");

    } else if (interfaceType=="launch") {

        interface = new DgsLaunchInterface(this);
        interface->setInterfaceOption("mode", "");

    } else if (interfaceType=="hotkey") {

        interface = new DgsHotkeyInterface(this);
        interface->setInterfaceOption("mode", "");

    } else if (interfaceType=="metronome") {

        interface = new DgsMetronomeInterface(this);
        interface->setInterfaceOption("mode", "");

    } else if (interfaceType=="audioin") {

        interface = new DgsAudioinInterface(this);
        interface->setInterfaceOption("mode", "");

    } else if (interfaceType=="screen") {

        interface = new DgsScreenInterface(this);
        interface->setInterfaceOption("mode", "");

        // each screen interface must have a player endpoint
        QVariantMap endpointOptions;
        endpointOptions["type"] = "player";
        endpointOptions["name"] = "player1";
        interface->addEndpoint(endpointOptions);

#ifdef DIGISHOW_EXPERIMENTAL

    } else if (interfaceType=="aplay") {

        interface = new DgsAPlayInterface(this);
        interface->setInterfaceOption("mode", "");

    } else if (interfaceType=="mplay") {

        interface = new DgsMPlayInterface(this);
        interface->setInterfaceOption("mode", "");
#endif

    } else { interface = new DigishowInterface(this);  }

    // assign a name to the new interface
    for (int n=1 ; ; n++) {
        QString name = interfaceType + QString::number(n);
        bool nameExisted = false;
        for (int i=0 ; i<m_interfaces.length() ; i++) {
            if (m_interfaces[i]->interfaceOptions()->value("name") == name) {
                nameExisted = true;
                break;
            }
        }
        if (!nameExisted) {
            interface->setInterfaceOption("name", name);
            break;
        }
    }

    interface->updateMetadata();
    m_interfaces.append(interface);
    return m_interfaces.length()-1; // return new interface index
}

bool DigishowApp::deleteInterface(int interfaceIndex)
{
    if (interfaceIndex < 0 || interfaceIndex >= m_interfaces.length()) return false;

    // confirm the interface is never employed by any slot
    QString interfaceName = m_interfaces[interfaceIndex]->interfaceOptions()->value("name").toString();
    for (int n=0 ; n<m_slots.length() ; n++) {

        QString src = m_slots[n]->slotOptions()->value("source").toString();
        QString dst = m_slots[n]->slotOptions()->value("destination").toString();

        if (src.startsWith(interfaceName+"/") || dst.startsWith(interfaceName+"/")) return false;
    }

    // remove the interface
    DigishowInterface *interface = m_interfaces[interfaceIndex];
    m_interfaces.removeAt(interfaceIndex);
    delete interface;

    return true;
}

int DigishowApp::newSlot()
{
    DigishowSlot *slot = new DigishowSlot(this);
    m_slots.append(slot);
    return m_slots.length()-1; // new slot index
}

int DigishowApp::duplicateSlot(int slotIndex)
{
    if (slotIndex >= 0 && slotIndex < m_slots.length()) {

        // get the source slot
        DigishowSlot *slot = m_slots[slotIndex];

        // create a new slot
        int newSlotIndex = newSlot();
        DigishowSlot *slotNew = m_slots[newSlotIndex];

        // transfer all to the new slot
        slotNew->setSource(slot->sourceInterface(), slot->sourceEndpointIndex());
        slotNew->setDestination(slot->destinationInterface(), slot->destinationEndpointIndex());
        slotNew->setSlotOptions(*slot->slotOptions());

        return newSlotIndex;
    }
    return -1;
}

bool DigishowApp::deleteSlot(int slotIndex)
{
    if (slotIndex >= 0 && slotIndex < m_slots.length()) {
        DigishowSlot *slot = m_slots[slotIndex];
        m_slots.removeAt(slotIndex);
        delete slot;
        return true;
    }
    return false;
}

int DigishowApp::interfaceCount()
{
    return m_interfaces.length();
}

DigishowInterface *DigishowApp::interfaceAt(int index)
{
    if (index>=0 && index<m_interfaces.length()) return m_interfaces[index];
    return nullptr;
}

int DigishowApp::slotCount()
{
    return m_slots.length();
}

DigishowSlot *DigishowApp::slotAt(int index)
{
    if (index>=0 && index<m_slots.length()) return  m_slots[index];
    return nullptr;
}

bool DigishowApp::updateLaunch(const QString &launchName, const QVariantList &slotLaunchOptions)
{
    // clear launch item details if it already exists
    QVariantMap launchOptions = getLaunchOptions(launchName);
    deleteLaunch(launchName);
    launchOptions["assigned"] = true;
    m_launches[launchName] = launchOptions;

    // update launch item details
    if (slotLaunchOptions.length() != m_slots.length()) return false;

    for (int n=0 ; n<m_slots.length() ; n++) {
        DigishowSlot* slot = m_slots[n];

        // make launch detail for the slot
        QVariantMap launchDetail;
        bool optRememberLink = slotLaunchOptions[n].toMap().value("rememberLink").toBool();
        if (optRememberLink) launchDetail["linked"] = slot->isLinked();

        bool optRememberOutput = slotLaunchOptions[n].toMap().value("rememberOutput").toBool();
        if (optRememberOutput) launchDetail["outputValue"] = slot->getEndpointOutValue();

        // write launch details into the slot
        QVariantMap launchDetails;
        if (slot->slotOptions()->contains("launchDetails"))
            launchDetails = slot->slotOptions()->value("launchDetails").toMap();

        launchDetails[launchName] = launchDetail;
        slot->setSlotOption("launchDetails", launchDetails);
    }

    return true;
}

bool DigishowApp::deleteLaunch(const QString &launchName)
{
    // delete the launch item with the specific name
    m_launches.remove(launchName);

    // delete the launch item details stored in slots
    for (int n=0 ; n<m_slots.length() ; n++) {
        DigishowSlot* slot = m_slots[n];
        if (slot->slotOptions()->contains("launchDetails")) {
            QVariantMap launchDetails = slot->slotOptions()->value("launchDetails").toMap();
            launchDetails.remove(launchName);
            slot->setSlotOption("launchDetails", launchDetails);
        }
    }

    return true;
}

bool DigishowApp::startLaunch(const QString &launchName)
{
    if (!m_launches.contains(launchName)) return false;

    for (int n=0 ; n<m_slots.length() ; n++) {
        DigishowSlot* slot = m_slots[n];
        if (slot->slotOptions()->contains("launchDetails")) {
            QVariantMap launchDetails = slot->slotOptions()->value("launchDetails").toMap();
            if (launchDetails.contains(launchName)) {
                QVariantMap launchDetail = launchDetails.value(launchName).toMap();
                if (launchDetail.contains("linked"))
                    slot->setLinked(launchDetail.value("linked").toBool());
                if (launchDetail.contains("outputValue"))
                    slot->setEndpointOutValue(launchDetail.value("outputValue").toInt());
            }
        }
    }

    return true;
}

QVariantMap DigishowApp::getLaunchOptions(const QString &launchName)
{
    if (!m_launches.contains(launchName)) return QVariantMap();
    return m_launches.value(launchName).toMap();
}

bool DigishowApp::setLaunchOption(const QString &launchName, const QString &optionName, const QVariant &optionValue)
{
    QVariantMap launch;
    if (m_launches.contains(launchName)) launch = m_launches.value(launchName).toMap();
    launch[optionName] = optionValue;
    m_launches[launchName] = launch;

    return true;
}

QVariantList DigishowApp::getSlotLaunchDetails(const QString &launchName)
{
    QVariantList listDetails;

    if (!m_launches.contains(launchName)) return listDetails;

    for (int n=0 ; n<m_slots.length() ; n++) {
        DigishowSlot* slot = m_slots[n];
        QVariantMap details;
        if (slot->slotOptions()->contains("launchDetails")) {
            QVariantMap launchDetails = slot->slotOptions()->value("launchDetails").toMap();
            if (launchDetails.contains(launchName))
                details = launchDetails.value(launchName).toMap();
        }
        listDetails.append(details);
    }

    return listDetails;
}

QVariantList DigishowApp::getSlotLaunchOptions(const QString &launchName)
{
    QVariantList listDetails = getSlotLaunchDetails(launchName);
    QVariantList listOptions;

    if (listDetails.length() == 0) return listOptions;

    for (int n=0 ; n<listDetails.length() ; n++) {
        QVariantMap launchDetails = listDetails[n].toMap();
        QVariantMap launchOptions;
        launchOptions["rememberLink"]   = launchDetails.contains("linked");
        launchOptions["rememberOutput"] = launchDetails.contains("outputValue");
        listOptions.append(launchOptions);
    }
    return listOptions;
}


bool DigishowApp::findInterfaceAndEndpoint(const QString &name, int *pInterfaceIndex, int *pEndpointIndex)
{
    QStringList parts = name.split("/");
    if (parts.length() != 2) return false;

    QString interfaceName = parts[0];
    QString endpointName = parts[1];

    // find interface by name
    for (int n=0 ; n<m_interfaces.length() ; n++) {

        QVariantMap interfaceOptions = *(m_interfaces[n]->interfaceOptions());
        if (interfaceOptions["name"] == interfaceName) {

            if (pInterfaceIndex != nullptr) *pInterfaceIndex = n;

            // find endpoint by name
            QList<QVariantMap> endpointOptionsList = *(m_interfaces[n]->endpointOptionsList());
            for (int m=0 ; m<endpointOptionsList.length() ; m++) {

                QVariantMap endpointOptions = endpointOptionsList[m];
                if (endpointOptions["name"] == endpointName) {

                    if (pEndpointIndex != nullptr) *pEndpointIndex = m;
                    return true;
                }
            }

            return false;
        }
    }

    return false;
}

bool DigishowApp::confirmEndpointIsEmployed(int interfaceIndex, int endpointIndex)
{
    if (interfaceIndex<0 && interfaceIndex>=m_interfaces.length()) return false;
    DigishowInterface* interface = m_interfaces[interfaceIndex];

    if (endpointIndex<0 && endpointIndex>=interface->endpointCount()) return false;
    QString interfaceName = interface->interfaceOptions()->value("name").toString();
    QString endpointName = interface->endpointOptionsList()->at(endpointIndex).value("name").toString();

    return confirmEndpointIsEmployed(interfaceName, endpointName);
}

bool DigishowApp::confirmEndpointIsEmployed(const QString &interfaceName, const QString &endpointName)
{
    QString name = interfaceName + "/" + endpointName;

    for (int n=0 ; n<m_slots.length() ; n++) {
        QString src = m_slots[n]->slotOptions()->value("source").toString();
        QString dst = m_slots[n]->slotOptions()->value("destination").toString();
        if (src == name || dst == name) return true;
    }
    return false;
}

QString DigishowApp::convertFileUrlToPath(const QString &url)
{
    // get base dir of the current app file
    if (m_filepath.isEmpty()) return "";
    QDir baseDir = QFileInfo(m_filepath).dir();

    // get absolute path of the file
    QString filePath = AppUtilities::fileUrlPath(url);
    if (filePath.isEmpty()) return "";

    // get relative path of the file
    return baseDir.relativeFilePath(filePath);
}

QString DigishowApp::convertFilePathToUrl(const QString &path)
{
    // get base dir of the current app file
    if (m_filepath.isEmpty()) return "";
    QDir baseDir = QFileInfo(m_filepath).dir();

    // get absolute path of the file
    QString filePath = baseDir.absoluteFilePath(path);

    // get url of the file
    return QUrl::fromLocalFile(filePath).toString();
}

bool DigishowApp::validateFileUrl(const QString &url)
{
    // get absolute path of the file
    QString filePath = AppUtilities::fileUrlPath(url);
    if (filePath.isEmpty()) return false;

    // confirm the file exists
    return QFile(filePath).exists();
}

bool DigishowApp::validateFilePath(const QString &path)
{
    // get base dir of the current app file
    if (m_filepath.isEmpty()) return false;
    QDir baseDir = QFileInfo(m_filepath).dir();

    // get absolute path of the file
    QString filePath = baseDir.absoluteFilePath(path);

    // confirm the file exists
    return QFile(filePath).exists();
}


void DigishowApp::onTimerFired()
{

}

