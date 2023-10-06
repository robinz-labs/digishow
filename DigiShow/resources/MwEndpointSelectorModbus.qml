import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemModbus

    COptionButton {
        id: buttonUnit
        width: 85
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuUnit.selectedItemText
        onClicked: menuUnit.showOptions()

        COptionMenu {
            id: menuUnit
        }
    }

    COptionButton {
        id: buttonType
        width: 115
        height: 28
        anchors.left: buttonUnit.right
        anchors.leftMargin: 10
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
        width: 120
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Address") + " " + spinChannel.value
        onClicked: spinChannel.visible = true

        COptionSpinBox {
            id: spinChannel
        }
    }

    function refresh() {
        var items
        var n

        // init modbus unit option menu
        if (menuUnit.count === 0) {
            items = []
            for (n=0 ; n<256 ; n++)
                items.push({ text: qsTr("Station") + " " + n, value: n })
            menuUnit.optionItems = items
            menuUnit.selectedIndex = 1
        }

        // init modbus type option menu
        if (menuType.count === 0) {
            items = []
            if (forInput ) items.push({ text: qsTr("Discrete Input"  ), value: DigishowEnvironment.EndpointModbusDiscreteIn, tag: "discrete_in" })
            if (forOutput) items.push({ text: qsTr("Coil"            ), value: DigishowEnvironment.EndpointModbusCoilOut,    tag: "coil_out"    })
            if (forInput ) items.push({ text: qsTr("Coil"            ), value: DigishowEnvironment.EndpointModbusCoilIn,     tag: "coil_in"     })
            if (forInput ) items.push({ text: qsTr("Input Register"  ), value: DigishowEnvironment.EndpointModbusRegisterIn, tag: "register_in" })
            if (forOutput) items.push({ text: qsTr("Holding Register"), value: DigishowEnvironment.EndpointModbusHoldingOut, tag: "holding_out" })
            if (forInput ) items.push({ text: qsTr("Holding Register"), value: DigishowEnvironment.EndpointModbusHoldingIn,  tag: "holding_in"  })

            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init modbus channel option spinbox
        spinChannel.from = 0
        spinChannel.to = 65535
        spinChannel.visible = false

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointModbusCoilOut) {

            enables["optInitialB"] = true

        } else if (endpointType === DigishowEnvironment.EndpointModbusHoldingOut) {

            enables["optInitialA"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuUnit.selectOption(endpointInfo["unit"])
        menuType.selectOption(endpointInfo["type"])
        spinChannel.value = endpointInfo["channel"]
    }

    function getEndpointOptions() {

        var options = {}
        options["unit"] = menuUnit.selectedItemValue
        options["type"] = menuType.selectedItemTag
        options["channel"] = spinChannel.value

        return options
    }
}

