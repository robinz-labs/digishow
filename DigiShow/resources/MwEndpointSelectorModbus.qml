import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemModbus

    property alias menuUnit:    menuModbusUnit
    property alias menuType:    menuModbusType
    property alias spinChannel: spinModbusChannel

    COptionButton {
        id: buttonModbusUnit
        width: 85
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuModbusUnit.selectedItemText
        onClicked: menuModbusUnit.showOptions()

        COptionMenu {
            id: menuModbusUnit
        }
    }

    COptionButton {
        id: buttonModbusType
        width: 115
        height: 28
        anchors.left: buttonModbusUnit.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuModbusType.selectedItemText
        onClicked: menuModbusType.showOptions()

        COptionMenu {
            id: menuModbusType
        }
    }

    COptionButton {
        id: buttonModbusChannel
        width: 120
        height: 28
        anchors.left: buttonModbusType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Address") + " " + spinModbusChannel.value
        onClicked: spinModbusChannel.visible = true

        COptionSpinBox {
            id: spinModbusChannel
        }
    }

    function refresh() {
        var items
        var n

        // init modbus unit option menu
        if (menuModbusUnit.count === 0) {
            items = []
            for (n=0 ; n<256 ; n++)
                items.push({ text: qsTr("Station") + " " + n, value: n })
            menuModbusUnit.optionItems = items
            menuModbusUnit.selectedIndex = 1
        }

        // init modbus type option menu
        if (menuModbusType.count === 0) {
            items = []
            if (forInput ) items.push({ text: qsTr("Discrete Input"  ), value: DigishowEnvironment.EndpointModbusDiscreteIn, tag: "discrete_in" })
            if (forOutput) items.push({ text: qsTr("Coil"            ), value: DigishowEnvironment.EndpointModbusCoilOut,    tag: "coil_out"    })
            if (forInput ) items.push({ text: qsTr("Coil"            ), value: DigishowEnvironment.EndpointModbusCoilIn,     tag: "coil_in"     })
            if (forInput ) items.push({ text: qsTr("Input Register"  ), value: DigishowEnvironment.EndpointModbusRegisterIn, tag: "register_in" })
            if (forOutput) items.push({ text: qsTr("Holding Register"), value: DigishowEnvironment.EndpointModbusHoldingOut, tag: "holding_out" })
            if (forInput ) items.push({ text: qsTr("Holding Register"), value: DigishowEnvironment.EndpointModbusHoldingIn,  tag: "holding_in"  })

            menuModbusType.optionItems = items
            menuModbusType.selectedIndex = 0
        }

        // init modbus channel option spinbox
        spinModbusChannel.from = 0
        spinModbusChannel.to = 65535
        spinModbusChannel.visible = false
    }
}

