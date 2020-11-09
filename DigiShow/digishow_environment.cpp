#include "digishow_environment.h"
#include "digishow_slot.h"
#include "digishow_interface.h"
#include "dgs_midi_interface.h"
#include "dgs_rioc_interface.h"
#include "dgs_modbus_interface.h"
#include "dgs_dmx_interface.h"
#include "dgs_artnet_interface.h"
#include "dgs_screen_interface.h"
#include "dgs_pipe_interface.h"
#include "shared/rioc_aladdin2560_def.h"

DigishowEnvironment::DigishowEnvironment(QObject *parent) : QObject(parent)
{

}

bool DigishowEnvironment::needLogs()
{
    return g_needLogCtl;
}

void DigishowEnvironment::setNeedLogs(bool need)
{
    g_needLogCtl = need;
    emit needLogsChanged();
}

QString DigishowEnvironment::appDataPath(const QString &filename)
{
    QString path = QDir::home().filePath(g_appname);
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(".");

    if (filename.isEmpty())
        return path;
    else
        return dir.filePath(filename);
}

void DigishowEnvironment::setAppOptions(const QVariantMap &options)
{
    AppUtilities::saveJsonToFile(options, QDir(appDataPath()).filePath("options.json"));
}

QVariantMap DigishowEnvironment::getAppOptions()
{
    return AppUtilities::loadJsonFromFile(QDir(appDataPath()).filePath("options.json"));
}

QVariantMap DigishowEnvironment::getSlotConfiguration(int slotIndex)
{
    QVariantMap data;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) {
        data["slotOptions"] = slot->getSlotOptions();
        data["slotInfo"   ] = slot->getSlotInfo();

        DigishowInterface *interfaceIn = slot->sourceInterface();
        if (interfaceIn != nullptr) {
            int epInIndex = slot->sourceEndpointIndex();
            data["epInOptions"] = interfaceIn->getEndpointOptionsAt(epInIndex);
            data["epInInfo"   ] = interfaceIn->getEndpointInfoAt(epInIndex);
        }

        DigishowInterface *interfaceOut = slot->destinationInterface();
        if (interfaceOut != nullptr) {
            int epOutIndex = slot->destinationEndpointIndex();
            data["epOutOptions"] = interfaceOut->getEndpointOptionsAt(epOutIndex);
            data["epOutInfo"   ] = interfaceOut->getEndpointInfoAt(epOutIndex);
        }
    }

    return data;
}

QVariantMap DigishowEnvironment::getSlotRuntimeData(int slotIndex)
{
    QVariantMap data;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) {

        bool epInAvailable  = (slot->getEndpointInValue() != -1);
        bool epOutAvailable = (slot->getEndpointOutValue() != -1);

        data["epInAvailable" ] = epInAvailable;
        data["epOutAvailable"] = epOutAvailable;
        data["epInValue"     ] = (epInAvailable  ? slot->getEndpointInValue()  : 0);
        data["epOutValue"    ] = (epOutAvailable ? slot->getEndpointOutValue() : 0);
        data["epInBusy"      ] = slot->isEndpointInBusy();
        data["epOutBusy"     ] = slot->isEndpointOutBusy();
        data["slotEnabled"   ] = slot->isEnabled();
        data["slotLinked"    ] = slot->isLinked();
    }

    return data;
}


QVariantMap DigishowEnvironment::getInterfaceConfiguration(int interfaceIndex)
{
    QVariantMap data;

    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface != nullptr) {
        data["interfaceOptions"] = interface->getInterfaceOptions();
        data["interfaceInfo"   ] = interface->getInterfaceInfo();

        QVariantList listOptions, listInfo;
        for (int n=0 ; n<interface->endpointCount() ; n++) {
            listOptions.append(interface->getEndpointOptionsAt(n));
            listInfo.append(interface->getEndpointInfoAt(n));
        }
        data["epOptionsList"] = listOptions;
        data["epInfoList"]    = listInfo;
    }

    return data;
}

int DigishowEnvironment::getSourceInterfaceIndex(int slotIndex)
{
    int interfaceIndex = -1;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) {
        DigishowInterface *interface = slot->sourceInterface();
        if (interface != nullptr) {
            int interfaceCount = g_app->interfaceCount();
            for (int n=0 ; n<interfaceCount ; n++) {
                if (g_app->interfaceAt(n) == interface) {
                    interfaceIndex = n;
                    break;
                }
            }
        }
    }

    return interfaceIndex;
}

int DigishowEnvironment::getSourceEndpointIndex(int slotIndex)
{
    int endpointIndex = -1;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) endpointIndex = slot->sourceEndpointIndex();

    return endpointIndex;
}

int DigishowEnvironment::getDestinationInterfaceIndex(int slotIndex)
{
    int interfaceIndex = -1;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) {
        DigishowInterface *interface = slot->destinationInterface();
        if (interface != nullptr) {
            int interfaceCount = g_app->interfaceCount();
            for (int n=0 ; n<interfaceCount ; n++) {
                if (g_app->interfaceAt(n) == interface) {
                    interfaceIndex = n;
                    break;
                }
            }
        }
    }

    return interfaceIndex;
}

int DigishowEnvironment::getDestinationEndpointIndex(int slotIndex)
{
    int endpointIndex = -1;

    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot != nullptr) endpointIndex = slot->destinationEndpointIndex();

    return endpointIndex;
}


int DigishowEnvironment::findInterfaceWithName(const QString &interfaceName)
{
    for (int i=0 ; i<g_app->interfaceCount() ; i++) {
        if (g_app->interfaceAt(i)->interfaceOptions()->value("name") == interfaceName) {
            return i;
        }
    }
    return -1;
}

bool DigishowEnvironment::restartInterface(int interfaceIndex)
{
    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return false;

    interface->closeInterface();
    return (interface->openInterface() == ERR_NONE);
}


int DigishowEnvironment::updateSourceEndpoint(int slotIndex, int interfaceIndex, QVariantMap endpointOptions)
{
    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot == nullptr) return -1;

    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return -1;

    int endpointIndex = makeEndpoint(interfaceIndex, endpointOptions);
    if (endpointIndex != -1) {
        slot->setSource(interface, endpointIndex);
        slot->setSlotOption(
                    "source",
                    interface->interfaceOptions()->value("name").toString() + "/" +
                    interface->endpointOptionsList()->at(endpointIndex).value("name").toString());
        if (!slot->isEnabled() && g_app->isRunning()) slot->setEnabled(true);
    }
    return endpointIndex;
}

int DigishowEnvironment::updateDestinationEndpoint(int slotIndex, int interfaceIndex, QVariantMap endpointOptions)
{
    DigishowSlot *slot = g_app->slotAt(slotIndex);
    if (slot == nullptr) return -1;

    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return -1;

    int endpointIndex = makeEndpoint(interfaceIndex, endpointOptions);
    if (endpointIndex != -1) {
        slot->setDestination(interface, endpointIndex);
        slot->setSlotOption(
                    "destination",
                    interface->interfaceOptions()->value("name").toString() + "/" +
                    interface->endpointOptionsList()->at(endpointIndex).value("name").toString());
        if (!slot->isEnabled() && g_app->isRunning()) slot->setEnabled(true);
    }
    return endpointIndex;
}

int DigishowEnvironment::makeEndpoint(int interfaceIndex, QVariantMap endpointOptions)
{
    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return -1;

    // look for the matched endpoint that already exists in the interface
    int endpointCount = interface->endpointCount();
    for (int i=0 ; i<endpointCount ; i++) {

        // find endpoint
        QStringList keys = endpointOptions.keys();
        bool matched = true;
        for (int n=0 ; n<keys.length() ; n++) {
            QString key = keys[n];
            if (key != "initial" && key != "range" && !key.startsWith("opt")) // ignore these options
            if (endpointOptions[key] != interface->endpointOptionsList()->at(i).value(key)) {

                matched = false;
                break;
            }
        }

        // endpoint is found
        if (matched) {

            // update these options
            for (int n=0 ; n<keys.length() ; n++) {
                QString key = keys[n];
                if (key == "initial" || key == "range" || key.startsWith("opt")) {
                    interface->setEndpointOption(i, key, endpointOptions[key]);
                }
            }

            // enable the endpoint
            interface->setEndpointOption(i, "enabled", true);

            interface->updateMetadata();
            return i;
        }
    }

    // no matched endpoint found, now need make a new one
    QString endpointType = endpointOptions["type"].toString();
    QString endpointName = endpointOptions["name"].toString();
    if (endpointType.isEmpty()) return -1;
    if (endpointName.isEmpty()) {

        // assign a name to the new endpoint
        for (int n=1 ; ; n++) {
            QString name = endpointType + QString::number(n);
            bool nameExisted = false;
            for (int i=0 ; i<endpointCount ; i++) {
                if (interface->endpointOptionsList()->at(i).value("name") == name) {
                    nameExisted = true;
                    break;
                }
            }
            if (!nameExisted) {
                endpointOptions["name"] = name;
                break;
            }
        }
    }

    int endpointIndex = -1;
    endpointOptions["enabled"] = true;
    interface->addEndpoint(endpointOptions, &endpointIndex);
    interface->updateMetadata();

    return endpointIndex;
}

QString DigishowEnvironment::makeMedia(int interfaceIndex, const QString &mediaUrl, const QString &mediaType)
{
    QString mediaName = "";

    // confirm interface exists
    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return mediaName;

    // confirm media url and type are valid
    if (mediaUrl.isEmpty() || mediaType.isEmpty()) return mediaName;

    // get media list
    QVariantList mediaList = interface->interfaceOptions()->value("media").toList();

    // look for the url-matched media item in the list
    for (int n=0 ; n<mediaList.length() ; n++) {
        QVariantMap media = mediaList[n].toMap();
        if (media.value("url") == mediaUrl && media.value("type") == mediaType) {
            mediaName = media.value("name").toString();
            break;
        }
    }

    // no matched media found, now need make a new one
    if (mediaName.isEmpty()) {

        // assign a name to the new media
        for (int n=1 ; ; n++) {
            QString name = mediaType + QString::number(n);
            bool nameExisted = false;
            for (int i=0 ; i<mediaList.length() ; i++) {
                if (mediaList[i].toMap().value("name") == name) {
                    nameExisted = true;
                    break;
                }
            }
            if (!nameExisted) {
                mediaName = name;
                break;
            }
        }

        // add the new one to media list
        QVariantMap mediaOptions;
        mediaOptions["name"] = mediaName;
        mediaOptions["url" ] = mediaUrl;
        mediaOptions["type"] = mediaType;
        mediaList.append(mediaOptions);
        interface->setInterfaceOption("media", mediaList);

        // add the new media view to screen
        if (interface->interfaceInfo()->type == INTERFACE_SCREEN) {
            static_cast<DgsScreenInterface*>(interface)->loadMedia(mediaOptions);
        }
    }

    return mediaName;
}

QVariantMap DigishowEnvironment::findMedia(int interfaceIndex, const QString &mediaName)
{
    QVariantMap mediaOptions;

    // confirm interface exists
    DigishowInterface *interface = g_app->interfaceAt(interfaceIndex);
    if (interface == nullptr) return mediaOptions;

    // get media list
    QVariantList mediaList = interface->interfaceOptions()->value("media").toList();

    // look for the name-matched media item in the list
    for (int n=0 ; n<mediaList.length() ; n++) {
        QVariantMap media = mediaList[n].toMap();
        if (media.value("name") == mediaName) {
            mediaOptions = media;
            break;
        }
    }

    return mediaOptions;
}

QVariantMap DigishowEnvironment::listOnline()
{
    QVariantMap info;
    info["midi"  ] = DgsMidiInterface  ::listOnline();
    info["dmx"   ] = DgsDmxInterface   ::listOnline();
    info["rioc"  ] = DgsRiocInterface  ::listOnline();
    info["modbus"] = DgsModbusInterface::listOnline();
    info["screen"] = DgsScreenInterface::listOnline();
    return info;
}

QString DigishowEnvironment::getLightControlName(int control, bool shortName)
{
    switch(control) {
        case CONTROL_LIGHT_R:    return shortName ? tr("R")   : tr("Red");
        case CONTROL_LIGHT_G:    return shortName ? tr("G")   : tr("Green");
        case CONTROL_LIGHT_B:    return shortName ? tr("B")   : tr("Blue");
        case CONTROL_LIGHT_W:    return shortName ? tr("W")   : tr("White");
        case CONTROL_LIGHT_HUE:  return shortName ? tr("Hue") : tr("Hue");
        case CONTROL_LIGHT_SAT:  return shortName ? tr("Sat") : tr("Saturation");
        case CONTROL_LIGHT_BRI:  return shortName ? tr("Bri") : tr("Brightness");
        case CONTROL_LIGHT_CT:   return shortName ? tr("CT")  : tr("C. Temp");
    }
    return "";
}

QString DigishowEnvironment::getMediaControlName(int control, bool shortName)
{
    switch(control) {
        case CONTROL_MEDIA_OPACITY:  return tr("Opacity");
        case CONTROL_MEDIA_SCALE:    return tr("Scale");
        case CONTROL_MEDIA_ROTATION: return tr("Rotation");
        case CONTROL_MEDIA_XOFFSET:  return tr("X Offset");
        case CONTROL_MEDIA_YOFFSET:  return tr("Y Offset");

        case CONTROL_MEDIA_SHOW:     return tr("Show");
        case CONTROL_MEDIA_HIDE:     return tr("Hide");
        case CONTROL_MEDIA_CLEAR:    return tr("Clear All");

        case CONTROL_VIDEO_PLAY:     return shortName ? tr("Play")     : tr("Video Play");
        case CONTROL_VIDEO_REPEAT:   return shortName ? tr("Repeat")   : tr("Video Repeat");
        case CONTROL_VIDEO_VOLUME:   return shortName ? tr("Volume")   : tr("Video Volume");
        case CONTROL_VIDEO_POSITION: return shortName ? tr("Position") : tr("Video Position");
    }
    return "";
}

QString DigishowEnvironment::getMidiControlName(int control)
{
    switch(control) {
        case CONTROL_MIDI_BANK:        return tr("Bank");
        case CONTROL_MIDI_MODULATION:  return tr("Modulation");
        case CONTROL_MIDI_BREATH:      return tr("Breath");
        case CONTROL_MIDI_FOOT:        return tr("Foot");
        case CONTROL_MIDI_PORTAMENTO:  return tr("Portamento");
        case CONTROL_MIDI_VOLUME:      return tr("Volume");
        case CONTROL_MIDI_BALANCE:     return tr("Balance");
        case CONTROL_MIDI_PAN:         return tr("Pan");
    }
    return "";
}

QString DigishowEnvironment::getMidiNoteName(int noteNumber)
{
    static QString notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = noteNumber / 12;   // 0 ~
    int semitone = noteNumber % 12; // 0 ~ 11
    return notes[semitone] + QString::number(octave-2);
}

QString DigishowEnvironment::getRiocPinName(int mode, int pinNumber)
{
    QString pinName;
    if (mode == INTERFACE_RIOC_ARDUINO_UNO) {

        if (pinNumber>=14 && pinNumber<=21)
            pinName = "A" + QString::number(pinNumber-14);
        else
            pinName = "D" + QString::number(pinNumber);

    } else if (mode == INTERFACE_RIOC_ARDUINO_MEGA) {

        if (pinNumber>=54 && pinNumber<=69)
            pinName = "A" + QString::number(pinNumber-54);
        else
            pinName = "D" + QString::number(pinNumber);

    } else if (mode == INTERFACE_RIOC_ALADDIN) {

        for (int n=0 ; n<16 ; n++)
            if (pinNumber==PIN_UD_DI[n]) { pinName = "DI" + QString::number(n); break; }

        for (int n=0 ; n<8 ; n++)
            if (pinNumber==PIN_UD_DO[n]) { pinName = "DO" + QString::number(n); break; }

        for (int n=0 ; n<4 ; n++)
            if (pinNumber==PIN_UD_ADC[n]) { pinName = "ADC" + QString::number(n); break; }
    }

    if (pinName.isEmpty()) pinName = QString::number(pinNumber);

    return pinName;
}

QString DigishowEnvironment::getMediaType(const QString &mediaUrl)
{
    if (!mediaUrl.startsWith("file://") &&
        !mediaUrl.startsWith("http://") &&
        !mediaUrl.startsWith("https://")) return "";

    if (mediaUrl.length()<10) return "";

    QString mediaUrl1 = mediaUrl.toLower();

    if (mediaUrl1.endsWith(".mp4") || mediaUrl1.endsWith(".mov"))
        return "video";
    else if (mediaUrl1.endsWith(".jpg") || mediaUrl1.endsWith(".png") || mediaUrl1.endsWith(".svg"))
        return "image";
    else
        return "web";
}
