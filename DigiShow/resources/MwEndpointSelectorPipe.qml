import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemPipe

    COptionButton {
        id: buttonType
        width: 100
        height: 28
        anchors.left: parent.right
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonChannel
        width: 100
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Channel") + " " + spinChannel.value
        onClicked: spinChannel.visible = true

        COptionSpinBox {
            id: spinChannel
        }
    }

    function refresh() {

        var items
        var n

        // init pipe type option menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Analog"), value: DigishowEnvironment.EndpointPipeAnalog, tag:"analog" })
            items.push({ text: qsTr("Binary"), value: DigishowEnvironment.EndpointPipeBinary, tag:"binary" })
            items.push({ text: qsTr("Note"  ), value: DigishowEnvironment.EndpointPipeNote,   tag:"note"   })
            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init pipe channel option spinbox
        spinChannel.from = 1
        spinChannel.to = 9999
        spinChannel.visible = false

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointPipeBinary) {

            enables["optInitialB"] = true

        } else if (endpointType === DigishowEnvironment.EndpointPipeAnalog) {

            enables["optInitialA"] = true
            enables["optRange"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuType.selectOption(endpointInfo["type"])
        spinChannel.value = endpointInfo["channel"]
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag
        options["channel"] = spinChannel.value
        return options
    }
}
