import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemOsc

    property alias textAddress: textOscAddress
    property alias spinChannel: spinOscChannel
    property alias menuType:    menuOscType

    CTextInputBox {
        id: textOscAddress
        anchors.bottom: parent.top
        anchors.bottomMargin: 10
        anchors.left: buttonOscType.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 38
        text: "/osc/address"
        onTextEdited: isModified = true
        onEditingFinished: if (text === "") text = "/osc/address"
    }

    COptionButton {
        id: buttonOscChannel
        width: 70
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Value") + " " + spinOscChannel.value
        onClicked: spinOscChannel.visible = true

        COptionSpinBox {
            id: spinOscChannel
        }
    }

    COptionButton {
        id: buttonOscType
        width: 130
        height: 28
        anchors.left: buttonOscChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuOscType.selectedItemText
        onClicked: menuOscType.showOptions()

        COptionMenu {
            id: menuOscType
            onOptionSelected: refreshMoreOptions()
        }
    }

    function refresh() {

        var items
        var n

        // init osc channel option spinbox
        spinOscChannel.from = 1
        spinOscChannel.to = 999
        spinOscChannel.visible = false

        // init osc type option menu
        if (menuOscType.count === 0) {
            items = []
            items.push({ text: qsTr("Integer") + " ( 32-bit )",   value: DigishowEnvironment.EndpointOscInt,   tag:"int"  })
            items.push({ text: qsTr("Float") + " ( 0 ~ 1.0000 )", value: DigishowEnvironment.EndpointOscFloat, tag:"float"})
            items.push({ text: qsTr("Boolean"),                   value: DigishowEnvironment.EndpointOscBool,  tag:"bool" })
            menuOscType.optionItems = items
            menuOscType.selectedIndex = 1
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuOscType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointOscInt ||
            endpointType === DigishowEnvironment.EndpointOscFloat) {

            enables["optInitialA"] = true

        } else if (endpointType === DigishowEnvironment.EndpointOscBool) {

            enables["optInitialB"] = true
        }


        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }
}
