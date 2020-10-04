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

    property string interfaceType: ""

    signal endpointUpdated()

    onInterfaceIndexChanged: {
        // reset interface type when clear interface index
        if (interfaceIndex === -1) interfaceType = ""
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
                itemModbus.visible = false
                itemRioc  .visible = false
                itemHue   .visible = false
                itemScreen.visible = false
                itemPipe  .visible = false

                var config = getSelectedInterfaceConfiguration()
                if (config !== undefined) {
                    switch (config["interfaceInfo"]["type"]) {
                    case DigishowEnvironment.InterfaceMidi:   itemMidi  .visible = true; itemMidi  .refresh(); break
                    case DigishowEnvironment.InterfaceDmx:    itemDmx   .visible = true; itemDmx   .refresh(); break
                    case DigishowEnvironment.InterfaceModbus: itemModbus.visible = true; itemModbus.refresh(); break
                    case DigishowEnvironment.InterfaceRioc:   itemRioc  .visible = true; itemRioc  .refresh(); break
                    case DigishowEnvironment.InterfaceHue:    itemHue   .visible = true; itemHue   .refresh(); break
                    case DigishowEnvironment.InterfaceScreen: itemScreen.visible = true; itemScreen.refresh(); break
                    case DigishowEnvironment.InterfacePipe:   itemPipe  .visible = true; itemPipe  .refresh(); break
                    }

                    interfaceType = config["interfaceOptions"]["type"];
                }
            }

            function getSelectedInterfaceConfiguration() {
                var interfaceIndex = selectedItemValue
                if (interfaceIndex !== -1) return digishow.getInterfaceConfiguration(interfaceIndex)
                return undefined
            }
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

    MwEndpointSelectorPipe {

        id: itemPipe

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
        visible: isModified

        onClicked: {

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

            } else if (type === DigishowEnvironment.InterfaceModbus) {

                newEndpointOptions["unit"] = itemModbus.menuUnit.selectedItemValue
                newEndpointOptions["type"] = itemModbus.menuType.selectedItemTag
                newEndpointOptions["channel"] = itemModbus.spinChannel.value

                needRestartInterface = true

            } else if (type === DigishowEnvironment.InterfaceRioc) {

                newEndpointOptions["unit"] = itemRioc.menuUnit.selectedItemValue
                newEndpointOptions["type"] = itemRioc.menuType.selectedItemTag
                newEndpointOptions["channel"] = itemRioc.menuChannel.selectedItemValue

                needStopApp = true

            } else if (type === DigishowEnvironment.InterfaceHue) {

                newEndpointOptions["type"] = "light"
                newEndpointOptions["channel"] = itemHue.spinChannel.value
                newEndpointOptions["control"] = itemHue.menuControl.selectedItemValue

            } else if (type === DigishowEnvironment.InterfaceScreen) {

                newEndpointOptions["type"] = itemScreen.menuType.selectedItemTag

                switch (itemScreen.menuType.selectedItemValue) {
                case DigishowEnvironment.EndpointScreenLight:
                    newEndpointOptions["control"] = itemScreen.menuLightControl.selectedItemValue
                    break
                case DigishowEnvironment.EndpointScreenMedia:
                    newEndpointOptions["control"] = itemScreen.menuMediaControl.selectedItemValue

                    if (itemScreen.textMediaUrl.visible) {
                        var mediaUrl  = itemScreen.textMediaUrl.text
                        var mediaType = digishow.getMediaType(mediaUrl)
                        var mediaName = digishow.makeMedia(newInterfaceIndex, mediaUrl, mediaType)

                        if (mediaName !== "") {
                            newEndpointOptions["media"] = mediaName
                            if (itemScreen.menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaShow)
                                newEndpointOptions = utilities.merge(newEndpointOptions, itemScreen.getMediaOptions())
                        } else {
                            messageBox.show(qsTr("Please select a media clip file exists on your computer disks or enter a valid url of the media clip."), qsTr("OK"))
                        }
                    }
                    break
                }

            } else if (type === DigishowEnvironment.InterfacePipe) {

                newEndpointOptions["type"] = itemPipe.menuType.selectedItemTag
                newEndpointOptions["channel"] = itemPipe.spinChannel.value
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
    }

    Component.onCompleted: {

    }

    function refresh() {

        var items
        var n

        // init interface option menu
        items = []
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

            } else if (type === DigishowEnvironment.InterfaceModbus) {

                itemModbus.menuUnit.selectOption(endpointInfo["unit"])
                itemModbus.menuType.selectOption(endpointInfo["type"])
                itemModbus.spinChannel.value = endpointInfo["channel"]

            } else if (type === DigishowEnvironment.InterfaceRioc) {

                itemRioc.menuUnit.selectOption(endpointInfo["unit"])
                itemRioc.menuType.selectOption(endpointInfo["type"])
                itemRioc.menuChannel.selectOption(endpointInfo["channel"])

            } else if (type === DigishowEnvironment.InterfaceHue) {

                itemHue.spinChannel.value = endpointInfo["channel"]
                itemHue.menuControl.selectOption(endpointInfo["control"])

            } else if (type === DigishowEnvironment.InterfaceScreen) {

                itemScreen.menuType.selectOption(endpointInfo["type"])

                switch (endpointInfo["type"]) {
                case DigishowEnvironment.EndpointScreenLight:
                    itemScreen.menuLightControl.selectOption(endpointInfo["control"])
                    itemScreen.setMediaOptions({}) // clear media options
                    break
                case DigishowEnvironment.EndpointScreenMedia:
                    itemScreen.menuMediaControl.selectOption(endpointInfo["control"])

                    var mediaName = endpointOptions["media"]
                    var mediaOptions = digishow.findMedia(interfaceIndex, mediaName)
                    var mediaUrl = "file://"
                    if (mediaOptions["url"] !== undefined) mediaUrl = mediaOptions["url"]
                    itemScreen.textMediaUrl.text = mediaUrl

                    itemScreen.setMediaOptions(endpointOptions)

                    break
                }

            } else if (type === DigishowEnvironment.InterfacePipe) {

                itemPipe.menuType.selectOption(endpointInfo["type"])
                itemPipe.spinChannel.value = endpointInfo["channel"]
            }
        }

        // reset modified flag
        isModified = false
    }
}
