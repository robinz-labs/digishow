import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemDmx

    property alias spinChannel: spinDmxChannel

    /*
    COptionButton {
        id: buttonDmxChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuDmxChannel.selectedItemText
        onClicked: menuDmxChannel.showOptions()

        COptionMenu {
            id: menuDmxChannel
        }
    }
    */

    COptionButton {
        id: buttonDmxChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Channel") + " " + spinDmxChannel.value
        onClicked: spinDmxChannel.visible = true

        COptionSpinBox {
            id: spinDmxChannel
        }
    }


    function refresh() {

        // init dmx channel option menu
        /*
        var items
        var n

        if (menuDmxChannel.count === 0) {
            items = []
            for (n=0 ; n<512 ; n++)
                items.push({ text: qsTr("Channel") + " " + (n+1), value: n })
            menuDmxChannel.optionItems = items
            menuDmxChannel.selectedIndex = 0
        }
        */

        // init dmx channel option spinbox
        spinDmxChannel.from = 1
        spinDmxChannel.to = 512
        spinDmxChannel.visible = false
    }
}

