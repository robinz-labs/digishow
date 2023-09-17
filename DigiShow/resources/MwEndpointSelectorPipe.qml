import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemPipe

    property alias menuType:    menuPipeType
    property alias spinChannel: spinPipeChannel

    COptionButton {
        id: buttonPipeType
        width: 100
        height: 28
        anchors.left: parent.right
        anchors.top: parent.top
        text: menuPipeType.selectedItemText
        onClicked: menuPipeType.showOptions()

        COptionMenu {
            id: menuPipeType

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonPipeChannel
        width: 100
        height: 28
        anchors.left: buttonPipeType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Channel") + " " + spinPipeChannel.value
        onClicked: spinPipeChannel.visible = true

        COptionSpinBox {
            id: spinPipeChannel
        }
    }

    function refresh() {

        var items
        var n

        // init pipe type option menu
        if (menuPipeType.count === 0) {
            items = []
            items.push({ text: qsTr("Analog"), value: DigishowEnvironment.EndpointPipeAnalog, tag:"analog" })
            items.push({ text: qsTr("Binary"), value: DigishowEnvironment.EndpointPipeBinary, tag:"binary" })
            items.push({ text: qsTr("Note"  ), value: DigishowEnvironment.EndpointPipeNote,   tag:"note"   })
            menuPipeType.optionItems = items
            menuPipeType.selectedIndex = 0
        }

        // init pipe channel option spinbox
        spinPipeChannel.from = 1
        spinPipeChannel.to = 9999
        spinPipeChannel.visible = false

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuPipeType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointPipeBinary) {

            enables["optInitialB"] = true

        } else if (endpointType === DigishowEnvironment.EndpointPipeAnalog) {

            enables["optInitialA"] = true
            enables["optRange"] = true
        }

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }
}
