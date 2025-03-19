import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: endpointSelector

    property int interfaceIndex: -1
    property int endpointIndex: -1
    property bool forInput: true
    property bool forOutput: true
    property bool isModified: true
    property bool isDetecting: false

    property string interfaceType: ""
    property string endpointIdentification: ""

    signal endpointUpdated()

    onInterfaceIndexChanged: {
        // reset interface type when clear interface index
        if (interfaceIndex === -1) {
            interfaceType = ""
            endpointIdentification = ""
        }
    }

    onIsDetectingChanged: {
        if (!forInput) return
        if (interfaceType === "midi") {
            itemMidi.visible = !isDetecting
        } else if (interfaceType === "osc") {
            itemOsc.visible = !isDetecting
        } else if (interfaceType === "messager") {
            itemMessager.visible = !isDetecting
        }
    }

    Image {
        id: iconInterface
        width: 64
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 12
        source: {
            if (forInput && !forOutput)
                return "qrc:///images/icon_ep_in_white.png"
            else if (!forInput && forOutput)
                return "qrc:///images/icon_ep_out_white.png"
        }
        opacity: 0.2
        visible: interfaceType === ""
    }

    Image {
        id: iconInterfaceType
        width: 32
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.top: parent.top
        anchors.topMargin: 12
        source: {
            if (interfaceType !== "")
                return "qrc:///images/icon_interface_" + interfaceType + "_white.png"
            else
                return ""
        }
        opacity: 0.2
        visible: interfaceType !== ""

        property bool mouseOver: false

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: iconInterfaceType.mouseOver = true
            onExited: iconInterfaceType.mouseOver = false
        }

        ToolTip.delay: 300
        ToolTip.visible: endpointIdentification !=="" && iconInterfaceType.mouseOver
        ToolTip.text: qsTr("Endpoint ID: ") + endpointIdentification
    }

    COptionButton {
        id: buttonInterface

        width: 210
        height: 28
        anchors.left: iconInterface.right
        anchors.leftMargin: 0
        anchors.verticalCenter: iconInterface.verticalCenter
        text: {
            var strSelect
            if (forInput && !forOutput)
                strSelect = qsTr("Select Source")
            else if (!forInput && forOutput)
                strSelect = qsTr("Select Destination")
            else
                strSelect = qsTr("Select")

            return menuInterface.selectedIndex === -1 ? strSelect : menuInterface.selectedItemText
        }

        onClicked: menuInterface.showOptions()

        COptionMenu {
            id: menuInterface

            onOptionSelected: {
                itemMidi     .visible = false
                itemDmx      .visible = false
                itemOsc      .visible = false
                itemArtnet   .visible = false
                itemModbus   .visible = false
                itemRioc     .visible = false
                itemHue      .visible = false
                itemAudioin  .visible = false
                itemScreen   .visible = false
                itemAPlay    .visible = false
                itemMPlay    .visible = false
                itemPipe     .visible = false
                itemLaunch   .visible = false
                itemHotkey   .visible = false
                itemMetronome.visible = false
                itemMessager .visible = false

                popupMoreOptions.resetOptions()
                popupMoreOptions.enableOptions({})
                buttonMoreOptions.visible = false

                interfaceType = ""

                var config = getSelectedInterfaceConfiguration()
                if (config !== undefined) {
                    switch (config["interfaceInfo"]["type"]) {
                    case DigishowEnvironment.InterfaceMidi:      itemMidi     .visible = true; itemMidi     .refresh(); break
                    case DigishowEnvironment.InterfaceDmx:       itemDmx      .visible = true; itemDmx      .refresh(); break
                    case DigishowEnvironment.InterfaceOsc:       itemOsc      .visible = true; itemOsc      .refresh(); break
                    case DigishowEnvironment.InterfaceArtnet:    itemArtnet   .visible = true; itemArtnet   .refresh(); break
                    case DigishowEnvironment.InterfaceModbus:    itemModbus   .visible = true; itemModbus   .refresh(); break
                    case DigishowEnvironment.InterfaceRioc:      itemRioc     .visible = true; itemRioc     .refresh(); break
                    case DigishowEnvironment.InterfaceHue:       itemHue      .visible = true; itemHue      .refresh(); break
                    case DigishowEnvironment.InterfaceAudioIn:   itemAudioin  .visible = true; itemAudioin  .refresh(); break
                    case DigishowEnvironment.InterfaceScreen:    itemScreen   .visible = true; itemScreen   .refresh(); break
                    case DigishowEnvironment.InterfaceAPlay:     itemAPlay    .visible = true; itemAPlay    .refresh(); break
                    case DigishowEnvironment.InterfaceMPlay:     itemMPlay    .visible = true; itemMPlay    .refresh(); break
                    case DigishowEnvironment.InterfacePipe:      itemPipe     .visible = true; itemPipe     .refresh(); break
                    case DigishowEnvironment.InterfaceLaunch:    itemLaunch   .visible = true; itemLaunch   .refresh(); break
                    case DigishowEnvironment.InterfaceHotkey:    itemHotkey   .visible = true; itemHotkey   .refresh(); break
                    case DigishowEnvironment.InterfaceMetronome: itemMetronome.visible = true; itemMetronome.refresh(); break
                    case DigishowEnvironment.InterfaceMessager:  itemMessager .visible = true; itemMessager .refresh(); break

                    }
                    interfaceType = config["interfaceOptions"]["type"];
                }

                if (forInput) stopDetectionAll()
            }

            function getSelectedInterfaceConfiguration() {
                var interfaceIndex = selectedItemValue
                if (interfaceIndex !== -1) return digishow.getInterfaceConfiguration(interfaceIndex)
                return undefined
            }
        }
    }

    CButton {
        id: buttonDetect
        width: 48
        height: 20
        anchors.right: buttonInterface.right
        anchors.rightMargin: 16
        anchors.verticalCenter: buttonInterface.verticalCenter
        label.font.bold: false
        label.font.pixelSize: 10
        label.text: qsTr("LEARN")
        box.radius: 3
        colorNormal: isDetecting ? "#990000" : "#484848"
        visible: app.isRunning && forInput && (interfaceType === "midi" || interfaceType === "osc" || interfaceType === "messager")
        onClicked: {
            if (isDetecting)
                stopDetection()
            else
                startDetection()
        }
    }

    CButton {
        id: buttonMoreOptions
        width: 28
        height: 28
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.top
        colorNormal: popupMoreOptions.isDefault ? "transparent" : "#666666"
        icon.width: 16
        icon.height: 16
        icon.source: "qrc:///images/icon_settings_white.png"
        box.border.width: 1
        box.radius: 3
        onClicked: popupMoreOptions.show()

        MwEndpointMoreOptions {
            id: popupMoreOptions
        }
    }


    MwEndpointSelectorMidi {

        id: itemMidi

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorDmx {

        id: itemDmx

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorOsc {

        id: itemOsc

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorArtnet {

        id: itemArtnet

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorModbus {

        id: itemModbus

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorRioc {

        id: itemRioc

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorHue {

        id: itemHue

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorAudioin {

        id: itemAudioin

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorScreen {

        id: itemScreen

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorAPlay {

        id: itemAPlay

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorMPlay {

        id: itemMPlay

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorPipe {

        id: itemPipe

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorLaunch {

        id: itemLaunch

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorHotkey {

        id: itemHotkey

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorMetronome {

        id: itemMetronome

        anchors.left: buttonInterface.left
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    MwEndpointSelectorMessager {

        id: itemMessager

        anchors.left: buttonInterface.left
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        visible: false
    }

    CButton {

        id: buttonSave

        width: 60
        height: 28
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: buttonInterface.bottom
        anchors.topMargin: 10
        label.font.bold: true
        label.font.pixelSize: 11
        label.text: qsTr("Apply")
        box.radius: 3
        colorNormal: Material.accent
        visible: isModified && !isDetecting

        onClicked: {
            if (menuInterface.selectedItemValue !== -1)
                save()
            else
                clear()
        }
    }

    Component.onCompleted: {

        digishow.interfaceDataInputDetected.connect(onInterfaceDataInputDetected)
    }

    function refresh() {

        var items
        var n

        // init interface option menu
        items = []
        items.push({ text: qsTr("None"), value: -1 })

        var interfaceCount = app.interfaceCount()
        for (n=0 ; n<interfaceCount ; n++) {

            var config = digishow.getInterfaceConfiguration(n)

            if ((forInput  && config["interfaceInfo"]["input"]) ||
                (forOutput && config["interfaceInfo"]["output"])) {
                items.push({ text:config["interfaceInfo"]["label"], value: n })
            }
        }
        menuInterface.optionItems = items

        var menuItemMore = Qt.createQmlObject("import \"components\"; COptionMenuItem {}", menuInterface)
        menuItemMore.text = qsTr("More ...")
        menuItemMore.triggered.connect(function() {

            menuInterface.close()
            dialogInterfaces.show()
        })
        menuInterface.addItem(menuItemMore)

        // select the specified interface
        // to show the particular endpoint options
        menuInterface.selectOption(interfaceIndex)

        // refresh endpoint options
        if (interfaceIndex !== -1 && endpointIndex !== -1 &&
            menuInterface.selectedItemValue !== -1) {

            // set ui of endpoint options in detail
            var type = menuInterface.getSelectedInterfaceConfiguration()["interfaceInfo"]["type"]
            var endpointInfo = menuInterface.getSelectedInterfaceConfiguration()["epInfoList"][endpointIndex]
            var endpointOptions = menuInterface.getSelectedInterfaceConfiguration()["epOptionsList"][endpointIndex]

            var itemEndpoint = null
            switch(type) {
            case DigishowEnvironment.InterfaceMidi:      itemEndpoint = itemMidi;      break
            case DigishowEnvironment.InterfaceDmx:       itemEndpoint = itemDmx;       break
            case DigishowEnvironment.InterfaceOsc:       itemEndpoint = itemOsc;       break
            case DigishowEnvironment.InterfaceArtnet:    itemEndpoint = itemArtnet;    break
            case DigishowEnvironment.InterfaceModbus:    itemEndpoint = itemModbus;    break
            case DigishowEnvironment.InterfaceRioc:      itemEndpoint = itemRioc;      break
            case DigishowEnvironment.InterfaceHue:       itemEndpoint = itemHue;       break
            case DigishowEnvironment.InterfaceAudioIn:   itemEndpoint = itemAudioin;   break
            case DigishowEnvironment.InterfaceScreen:    itemEndpoint = itemScreen;    break
            case DigishowEnvironment.InterfaceAPlay:     itemEndpoint = itemAPlay;     break
            case DigishowEnvironment.InterfaceMPlay:     itemEndpoint = itemMPlay;     break
            case DigishowEnvironment.InterfacePipe:      itemEndpoint = itemPipe;      break
            case DigishowEnvironment.InterfaceLaunch:    itemEndpoint = itemLaunch;    break
            case DigishowEnvironment.InterfaceHotkey:    itemEndpoint = itemHotkey;    break
            case DigishowEnvironment.InterfaceMetronome: itemEndpoint = itemMetronome; break
            case DigishowEnvironment.InterfaceMessager:  itemEndpoint = itemMessager;  break

            }
            if (itemEndpoint !== null)
                itemEndpoint.setEndpointOptions(endpointInfo, endpointOptions)

            // set ui with more options
            popupMoreOptions.setOptions(endpointOptions)

            endpointIdentification = getEndpointIdentification(endpointIndex)
        }

        // reset modified flag
        isModified = false
    }

    function save() {

        var slotIndex = slotDetailView.slotIndex
        var newInterfaceIndex = menuInterface.selectedItemValue
        var newEndpointOptions = {}
        var needRestartInterface = false
        var needStopApp = false

        // fill endpoint options
        var type = menuInterface.getSelectedInterfaceConfiguration()["interfaceInfo"]["type"]

        var itemEndpoint = null
        switch(type) {
        case DigishowEnvironment.InterfaceMidi:      itemEndpoint = itemMidi;      break
        case DigishowEnvironment.InterfaceDmx:       itemEndpoint = itemDmx;       needRestartInterface = true; break
        case DigishowEnvironment.InterfaceOsc:       itemEndpoint = itemOsc;       break
        case DigishowEnvironment.InterfaceArtnet:    itemEndpoint = itemArtnet;    break
        case DigishowEnvironment.InterfaceModbus:    itemEndpoint = itemModbus;    needRestartInterface = true; break
        case DigishowEnvironment.InterfaceRioc:      itemEndpoint = itemRioc;      needStopApp = true; break
        case DigishowEnvironment.InterfaceHue:       itemEndpoint = itemHue;       break
        case DigishowEnvironment.InterfaceAudioIn:   itemEndpoint = itemAudioin;   needRestartInterface = true; break
        case DigishowEnvironment.InterfaceScreen:    itemEndpoint = itemScreen;    break
        case DigishowEnvironment.InterfaceAPlay:     itemEndpoint = itemAPlay;     break
        case DigishowEnvironment.InterfaceMPlay:     itemEndpoint = itemMPlay;     break
        case DigishowEnvironment.InterfacePipe:      itemEndpoint = itemPipe;      break
        case DigishowEnvironment.InterfaceLaunch:    itemEndpoint = itemLaunch;    break
        case DigishowEnvironment.InterfaceHotkey:    itemEndpoint = itemHotkey;    needRestartInterface = true; break
        case DigishowEnvironment.InterfaceMetronome: itemEndpoint = itemMetronome; needRestartInterface = true; break
        case DigishowEnvironment.InterfaceMessager:  itemEndpoint = itemMessager;  needRestartInterface = forInput; break
        }
        if (itemEndpoint !== null)
            newEndpointOptions = itemEndpoint.getEndpointOptions()

        // append more options
        if (popupMoreOptions.isRevealed || !popupMoreOptions.isDefault) {
            newEndpointOptions = utilities.merge(newEndpointOptions, popupMoreOptions.getOptions())
            popupMoreOptions.setOptions(newEndpointOptions) // here refresh ui
        }

        // save the updated endpoint
        var newEndpointIndex = -1;
        if (forInput && !forOutput)
            newEndpointIndex = digishow.updateSourceEndpoint(slotIndex, newInterfaceIndex, newEndpointOptions)
        else if (!forInput && forOutput)
            newEndpointIndex = digishow.updateDestinationEndpoint(slotIndex, newInterfaceIndex, newEndpointOptions)

        // update the endpoint selector
        if (newEndpointIndex !== -1) {

            isModified = false
            interfaceIndex = newInterfaceIndex
            endpointIndex = newEndpointIndex
            endpointUpdated() // emit signal

            endpointIdentification = getEndpointIdentification(newEndpointIndex)
        }

        // restart interface if necessary
        if (app.isRunning && needRestartInterface) {
            digishow.restartInterface(interfaceIndex)
        }

        // stop digishow app service if necessary
        if (app.isRunning && needStopApp) {
            app.stop()
        }

        refresh()
    }

    function clear() {

        var slotIndex = slotDetailView.slotIndex

        if (forInput && !forOutput)
            digishow.clearSourceEndpoint(slotIndex)
        else if (!forInput && forOutput)
            digishow.clearDestinationEndpoint(slotIndex)

        isModified = false
        interfaceIndex = -1
        endpointIndex = -1
        endpointUpdated() // emit signal
    }

    function getEndpointIdentification(endpointIndex) {

        var config = menuInterface.getSelectedInterfaceConfiguration();
        if (config === undefined) return ""

        var interfaceOpt = config["interfaceOptions"]
        if (interfaceOpt === undefined) return ""

        var endpointOpt = config["epOptionsList"][endpointIndex]
        if (endpointOpt === undefined) return ""

        return interfaceOpt["name"] + "/" + endpointOpt["name"]
    }

    function startDetection() {

        var interfaceIndex = menuInterface.selectedItemValue
        if (interfaceIndex !== -1) digishow.startInterfaceDataInputDetection(interfaceIndex)
        isDetecting = true
    }

    function stopDetection() {

        var interfaceIndex = menuInterface.selectedItemValue
        if (interfaceIndex !== -1) digishow.stopInterfaceDataInputDetection(interfaceIndex)
        isDetecting = false
    }

    function stopDetectionAll() {

        digishow.stopInterfaceDataInputDetectionAll()
        isDetecting = false
    }

    function onInterfaceDataInputDetected(interfaceIndex, rawData) {

        console.log("InterfaceDataInputDetected", interfaceIndex, JSON.stringify(rawData))

        if (interfaceIndex === menuInterface.selectedItemValue) {

            var ok = false
            if (interfaceType === "midi") {

                ok = itemMidi.learn(rawData)

            } else if (interfaceType === "osc") {

                ok = itemOsc.learn(rawData)

            } else if (interfaceType === "messager") {

                ok = itemMessager.learn(rawData)
            }

            if (ok) {
                isModified = true
                stopDetection()
            }
        }
    }
}
