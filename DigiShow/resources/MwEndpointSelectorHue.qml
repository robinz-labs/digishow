import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemHue

    property alias spinChannel: spinHueChannel
    property alias menuControl: menuHueControl

    COptionButton {
        id: buttonHueChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Light") + " " + spinHueChannel.value
        onClicked: spinHueChannel.visible = true

        COptionSpinBox {
            id: spinHueChannel
        }
    }

    COptionButton {
        id: buttonHueControl
        width: 100
        height: 28
        anchors.left: buttonHueChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuHueControl.selectedItemText
        onClicked: menuHueControl.showOptions()

        COptionMenu {
            id: menuHueControl
        }
    }


    function refresh() {

        // init hue light channel option spinbox
        spinHueChannel.from = 1
        spinHueChannel.to = 50
        spinHueChannel.visible = false

        // init screen control option menu
        if (menuHueControl.count === 0) {
            var items = []
            var v
            v = DigishowEnvironment.ControlLightR;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightG;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightB;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightSat; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightBri; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightHue; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightCt;  items.push({ text: digishow.getLightControlName(v), value: v })
            menuHueControl.optionItems = items
            menuHueControl.selectedIndex = 0
        }
    }
}

