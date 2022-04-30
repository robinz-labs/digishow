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
                itemMidi  .visible = false
                itemDmx   .visible = false
                itemOsc   .visible = false
                itemArtnet.visible = false
                itemModbus.visible = false
                itemRioc  .visible = false
                itemHue   .visible = false
                itemScreen.visible = false
                itemAPlay .visible = false
                itemMPlay .visible = false
                itemPipe  .visible = false
                itemLaunch.visible = false
                itemHotkey.visible = false

                moreOptions.resetOptions()
                moreOptions.enableOptions({})
                buttonMoreOptions.visible = false

                interfaceType = ""

                var config = getSelectedInterfaceConfiguration()
                if (config !== undefined) {
                    switch (config["interfaceInfo"]["type"]) {
                    case DigishowEnvironment.InterfaceMidi:   itemMidi  .visible = true; itemMidi  .refresh(); break
                    case DigishowEnvironment.InterfaceDmx:    itemDmx   .visible = true; itemDmx   .refresh(); break
                    case DigishowEnvironment.InterfaceOsc:    itemOsc   .visible = true; itemOsc   .refresh(); break
                    case DigishowEnvironment.InterfaceArtnet: itemArtnet.visible = true; itemArtnet.refresh(); break
                    case DigishowEnvironment.InterfaceModbus: itemModbus.visible = true; itemModbus.refresh(); break
                    case DigishowEnvironment.InterfaceRioc:   itemRioc  .visible = true; itemRioc  .refresh(); break
                    case DigishowEnvironment.InterfaceHue:    itemHue   .visible = true; itemHue   .refresh(); break
                    case DigishowEnvironment.InterfaceScreen: itemScreen.visible = true; itemScreen.refresh(); break
                    case DigishowEnvironment.InterfaceAPlay:  itemAPlay .visible = true; itemAPlay .refresh(); break
                    case DigishowEnvironment.InterfaceMPlay:  itemMPlay .visible = true; itemMPlay .refresh(); break
                    case DigishowEnvironment.InterfacePipe:   itemPipe  .visible = true; itemPipe  .refresh(); break
                    case DigishowEnvironment.InterfaceLaunch: itemLaunch.visible = true; itemLaunch.refresh(); break
                    case DigishowEnvironment.InterfaceHotkey: itemHotkey.visible = true; itemHotkey.refresh(); break
                    }
                    interfaceType = config["interfaceOptions"]["type"];
                }

                if (forInput) stopDetection()
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
        colorNormal: isDetecting ? "#990000" : "transparent"
        visible: app.isRunning && forInput && (interfaceType === "midi" || interfaceType === "osc")
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
        colorNormal: moreOptions.isDefault ? "transparent" : "#666666"
        icon.width: 16
        icon.height: 16
        icon.source: "qrc:///images/icon_settings_white.png"
        box.border.width: 1
        box.radius: 3
        onClicked: moreOptions.show()

        MwEndpointMoreOptions {
            id: moreOptions
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

            if (type === DigishowEnvironment.InterfaceMidi) {

                itemMidi.menuChannel.selectOption(endpointInfo["channel"])
                itemMidi.menuType.selectOption(endpointInfo["type"])

                switch (endpointInfo["type"]) {
                case DigishowEnvironment.EndpointMidiNote:
                    itemMidi.menuNote.selectOption(endpointInfo["note"])
                    break
                case DigishowEnvironment.EndpointMidiControl:
                    itemMidi.menuControl.selectOption(endpointInfo["control"])
                    break
                }

            } else if (type === DigishowEnvironment.InterfaceDmx) {

                //itemDmx.menuChannel.selectOption(endpointInfo["channel"])
                itemDmx.spinChannel.value = endpointInfo["channel"] + 1

            } else if (type === DigishowEnvironment.InterfaceOsc) {

                itemOsc.spinChannel.value = endpointInfo["channel"] + 1
                itemOsc.menuType.selectOption(endpointInfo["type"])

                var oscAddress = endpointInfo["address"]
                if (oscAddress === undefined || oscAddress === "") oscAddress = "/osc/address"
                itemOsc.textAddress.text = oscAddress

            } else if (type === DigishowEnvironment.InterfaceArtnet) {

                itemArtnet.spinUnit.value = endpointInfo["unit"]
                itemArtnet.spinChannel.value = endpointInfo["channel"] + 1

            } else if (type === DigishowEnvironment.InterfaceModbus) {

                itemModbus.menuUnit.selectOption(endpointInfo["unit"])
                itemModbus.menuType.selectOption(endpointInfo["type"])
                itemModbus.spinChannel.value = endpointInfo["channel"]

            } else if (type === DigishowEnvironment.InterfaceRioc) {

                itemRioc.spinUnit.value = endpointInfo["unit"]
                itemRioc.menuType.selectOption(endpointInfo["type"])
                itemRioc.menuChannel.selectOption(endpointInfo["channel"])

            } else if (type === DigishowEnvironment.InterfaceHue) {

                itemHue.menuType.selectOption(endpointInfo["type"])
                itemHue.spinChannel.value = endpointInfo["channel"]
                itemHue.menuControl.selectOption(endpointInfo["control"])

            } else if (type === DigishowEnvironment.InterfaceScreen) {

                itemScreen.menuType.selectOption(endpointInfo["type"])

                switch (endpointInfo["type"]) {
                case DigishowEnvironment.EndpointScreenLight:
                    itemScreen.menuLightControl.selectOption(endpointInfo["control"])
                    itemScreen.setEndpointMediaOptions({}) // clear media options
                    break
                case DigishowEnvironment.EndpointScreenCanvas:
                    itemScreen.menuCanvasControl.selectOption(endpointInfo["control"])
                    itemScreen.setEndpointMediaOptions({}) // clear media options
                    break
                case DigishowEnvironment.EndpointScreenMedia:
                    itemScreen.menuMediaControl.selectOption(endpointInfo["control"])

                    var mediaName = endpointOptions["media"]
                    var mediaIndex = digishow.findMediaWithName(interfaceIndex, mediaName)
                    var mediaOptions = digishow.getMediaOptions(interfaceIndex, mediaIndex)
                    var mediaUrl = "file://"
                    if (mediaOptions["url"] !== undefined) mediaUrl = mediaOptions["url"]
                    itemScreen.textMediaUrl.text = mediaUrl

                    itemScreen.setEndpointMediaOptions(endpointOptions)

                    break
                }

            } else if (type === DigishowEnvironment.InterfaceAPlay) {

                itemAPlay.menuType.selectOption(endpointInfo["type"])

                switch (endpointInfo["type"]) {
                case DigishowEnvironment.EndpointAPlayMedia:
                    itemAPlay.menuMediaControl.selectOption(endpointInfo["control"])

                    var mediaName = endpointOptions["media"]
                    var mediaIndex = digishow.findMediaWithName(interfaceIndex, mediaName)
                    var mediaOptions = digishow.getMediaOptions(interfaceIndex, mediaIndex)
                    var mediaUrl = "file://"
                    if (mediaOptions["url"] !== undefined) mediaUrl = mediaOptions["url"]
                    itemAPlay.textMediaUrl.text = mediaUrl

                    itemAPlay.setEndpointMediaOptions(endpointOptions)

                    break
                }

            } else if (type === DigishowEnvironment.InterfaceMPlay) {

                itemMPlay.menuType.selectOption(endpointInfo["type"])

                switch (endpointInfo["type"]) {
                case DigishowEnvironment.EndpointMPlayMedia:
                    itemMPlay.menuMediaControl.selectOption(endpointInfo["control"])

                    var mediaName = endpointOptions["media"]
                    var mediaIndex = digishow.findMediaWithName(interfaceIndex, mediaName)
                    var mediaOptions = digishow.getMediaOptions(interfaceIndex, mediaIndex)
                    var mediaUrl = "file://"
                    if (mediaOptions["url"] !== undefined) mediaUrl = mediaOptions["url"]
                    itemMPlay.textMediaUrl.text = mediaUrl

                    itemMPlay.setEndpointMediaOptions(endpointOptions)

                    break
                }

            } else if (type === DigishowEnvironment.InterfacePipe) {

                itemPipe.menuType.selectOption(endpointInfo["type"])
                itemPipe.spinChannel.value = endpointInfo["channel"]

            } else if (type === DigishowEnvironment.InterfaceLaunch) {

                itemLaunch.menuChannel.selectOption(endpointInfo["channel"])

            } else if (type === DigishowEnvironment.InterfaceHotkey) {

                var hotkey = endpointInfo["address"].split("+")
                itemHotkey.menuKey.selectedIndex = 0
                itemHotkey.menuModifier1.selectedIndex = 0
                itemHotkey.menuModifier2.selectedIndex = 0
                if (hotkey.length > 0) itemHotkey.menuKey.selectOptionWithTag(hotkey.pop())
                if (hotkey.length > 0) itemHotkey.menuModifier1.selectOptionWithTag(hotkey.shift())
                if (hotkey.length > 0) itemHotkey.menuModifier2.selectOptionWithTag(hotkey.shift())
            }

            // set ui with more options
            moreOptions.setOptions(endpointOptions)

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
        if (type === DigishowEnvironment.InterfaceMidi) {

            newEndpointOptions["channel"] = itemMidi.menuChannel.selectedItemValue
            newEndpointOptions["type"] = itemMidi.menuType.selectedItemTag

            switch (itemMidi.menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointMidiNote:
                newEndpointOptions["note"] = itemMidi.menuNote.selectedItemValue
                break
            case DigishowEnvironment.EndpointMidiControl:
                newEndpointOptions["control"] = itemMidi.menuControl.selectedItemValue
                break
            }

        } else if (type === DigishowEnvironment.InterfaceDmx) {

            newEndpointOptions["type"] = "dimmer"
            //newEndpointOptions["channel"] = itemDmx.menuChannel.selectedItemValue
            newEndpointOptions["channel"] = itemDmx.spinChannel.value - 1

            needRestartInterface = true

        } else if (type === DigishowEnvironment.InterfaceOsc) {

            newEndpointOptions["channel"] = itemOsc.spinChannel.value - 1
            newEndpointOptions["type"] = itemOsc.menuType.selectedItemTag
            newEndpointOptions["address"] = itemOsc.textAddress.text.trim()

        } else if (type === DigishowEnvironment.InterfaceArtnet) {

            newEndpointOptions["type"] = "dimmer"
            newEndpointOptions["unit"] = itemArtnet.spinUnit.value
            newEndpointOptions["channel"] = itemArtnet.spinChannel.value - 1

        } else if (type === DigishowEnvironment.InterfaceModbus) {

            newEndpointOptions["unit"] = itemModbus.menuUnit.selectedItemValue
            newEndpointOptions["type"] = itemModbus.menuType.selectedItemTag
            newEndpointOptions["channel"] = itemModbus.spinChannel.value

            needRestartInterface = true

        } else if (type === DigishowEnvironment.InterfaceRioc) {

            newEndpointOptions["unit"] = itemRioc.spinUnit.value
            newEndpointOptions["type"] = itemRioc.menuType.selectedItemTag
            newEndpointOptions["channel"] = itemRioc.menuChannel.selectedItemValue

            needStopApp = true

        } else if (type === DigishowEnvironment.InterfaceHue) {

            newEndpointOptions["type"] = itemHue.menuType.selectedItemTag
            newEndpointOptions["channel"] = itemHue.spinChannel.value
            newEndpointOptions["control"] = itemHue.menuControl.selectedItemValue

        } else if (type === DigishowEnvironment.InterfaceScreen) {

            newEndpointOptions["type"] = itemScreen.menuType.selectedItemTag

            switch (itemScreen.menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointScreenLight:
                newEndpointOptions["control"] = itemScreen.menuLightControl.selectedItemValue
                break
            case DigishowEnvironment.EndpointScreenCanvas:
                newEndpointOptions["control"] = itemScreen.menuCanvasControl.selectedItemValue
                break
            case DigishowEnvironment.EndpointScreenMedia:
                newEndpointOptions["control"] = itemScreen.menuMediaControl.selectedItemValue

                if (itemScreen.textMediaUrl.visible) {
                    var mediaUrl = itemScreen.textMediaUrl.text
                    var mediaType = digishow.getScreenMediaType(mediaUrl)
                    var mediaIndex = digishow.makeMedia(newInterfaceIndex, mediaUrl, mediaType)

                    if (mediaIndex !== -1) {
                        newEndpointOptions["media"] = digishow.getMediaName(newInterfaceIndex, mediaIndex)
                        if (itemScreen.menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart)
                            newEndpointOptions = utilities.merge(newEndpointOptions, itemScreen.getEndpointMediaOptions())
                    } else {
                        messageBox.show(qsTr("Please select a media clip file exists on your computer disks or enter a valid url of the media clip."), qsTr("OK"))
                    }
                }
                break
            }

        } else if (type === DigishowEnvironment.InterfaceAPlay) {

            newEndpointOptions["type"] = itemAPlay.menuType.selectedItemTag

            switch (itemAPlay.menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointAPlayMedia:
                newEndpointOptions["control"] = itemAPlay.menuMediaControl.selectedItemValue

                if (itemAPlay.textMediaUrl.visible) {
                    var mediaUrl = itemAPlay.textMediaUrl.text
                    var mediaIndex = digishow.makeMedia(newInterfaceIndex, mediaUrl, "audio")

                    if (mediaIndex !== -1) {
                        newEndpointOptions["media"] = digishow.getMediaName(newInterfaceIndex, mediaIndex)
                        if (itemAPlay.menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart)
                            newEndpointOptions = utilities.merge(newEndpointOptions, itemAPlay.getEndpointMediaOptions())
                    } else {
                        messageBox.show(qsTr("Please select an audio clip file exists on your computer disks or enter a valid url of the audio clip."), qsTr("OK"))
                    }
                }
                break
            }

        } else if (type === DigishowEnvironment.InterfaceMPlay) {

            newEndpointOptions["type"] = itemMPlay.menuType.selectedItemTag

            switch (itemMPlay.menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointMPlayMedia:
                newEndpointOptions["control"] = itemMPlay.menuMediaControl.selectedItemValue

                if (itemMPlay.textMediaUrl.visible) {
                    var mediaUrl = itemMPlay.textMediaUrl.text
                    var mediaIndex = digishow.makeMedia(newInterfaceIndex, mediaUrl, "midi")

                    if (mediaIndex !== -1) {
                        newEndpointOptions["media"] = digishow.getMediaName(newInterfaceIndex, mediaIndex)
                        if (itemMPlay.menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart)
                            newEndpointOptions = utilities.merge(newEndpointOptions, itemMPlay.getEndpointMediaOptions())
                    } else {
                        messageBox.show(qsTr("Please select a MIDI clip file exists on your computer disks or enter a valid url of the MIDI clip."), qsTr("OK"))
                    }
                }
                break
            }

        } else if (type === DigishowEnvironment.InterfacePipe) {

            newEndpointOptions["type"] = itemPipe.menuType.selectedItemTag
            newEndpointOptions["channel"] = itemPipe.spinChannel.value

        } else if (type === DigishowEnvironment.InterfaceLaunch) {

            newEndpointOptions["type"] = "preset"
            newEndpointOptions["channel"] = itemLaunch.menuChannel.selectedItemValue

        } else if (type === DigishowEnvironment.InterfaceHotkey) {

            newEndpointOptions["type"] = "press"
            var hotkey = [];
            if (itemHotkey.menuModifier1.selectedIndex > 0) hotkey.push(itemHotkey.menuModifier1.selectedItemTag)
            if (itemHotkey.menuModifier2.selectedIndex > 0) hotkey.push(itemHotkey.menuModifier2.selectedItemTag)
            hotkey.push(itemHotkey.menuKey.selectedItemTag)
            newEndpointOptions["address"] = hotkey.join("+")

            needRestartInterface = true
        }

        // append more options
        newEndpointOptions = utilities.merge(newEndpointOptions, moreOptions.getOptions())
        moreOptions.setOptions(newEndpointOptions) // here refresh ui

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

    function onInterfaceDataInputDetected(interfaceIndex, rawData) {

        console.log("InterfaceDataInputDetected", interfaceIndex, JSON.stringify(rawData))

        if (interfaceIndex === menuInterface.selectedItemValue) {

            if (interfaceType === "midi") {

                var event = rawData["event"]
                if (event === "note_on" || event === "note_off") {

                    itemMidi.menuType.selectOptionWithTag("note")
                    itemMidi.menuNote.selectOption(rawData["note"])

                } else if (event === "control_change") {

                    itemMidi.menuType.selectOptionWithTag("control")
                    itemMidi.menuControl.selectOption(rawData["control"])

                } else if (event === "program_change") {

                    itemMidi.menuType.selectOptionWithTag("program")

                }
                itemMidi.menuChannel.selectOption(rawData["channel"])

            } else if (interfaceType === "osc") {

                itemOsc.textAddress.text = rawData["address"]

                var numValues = rawData["values"].length
                if (numValues > 0) {

                    var typeTag = rawData["values"][numValues-1]["tag"]
                    var typeName
                    if      (typeTag === "i") typeName = "int"
                    else if (typeTag === "f") typeName = "float"
                    else if (typeTag === "T" ||
                             typeTag === "F") typeName = "bool"

                    itemOsc.spinChannel.value = numValues
                    itemOsc.menuType.selectOptionWithTag(typeName)
                }
            }

            isModified = true
        }

        stopDetection()
    }
}
