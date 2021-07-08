import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemHue

    property alias menuType: menuHueType
    property alias spinChannel: spinHueChannel
    property alias menuControl: menuHueControl

    COptionButton {
        id: buttonHueType
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuHueType.selectedItemText
        onClicked: menuHueType.showOptions()

        COptionMenu {
            id: menuHueType
        }
    }

    COptionButton {
        id: buttonHueChannel
        width: 100
        height: 28
        anchors.left: buttonHueType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("ID") + " " + spinHueChannel.value
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

            onOptionSelected: refreshMoreOptions()
        }
    }


    function refresh() {

        var items
        var v

        // init hue type option menu
        if (menuHueType.count === 0) {
            items = []
            items.push({ text: qsTr("Light"), value: DigishowEnvironment.EndpointHueLight,  tag:"light" })
            items.push({ text: qsTr("Group"), value: DigishowEnvironment.EndpointHueGroup,  tag:"group" })
            menuHueType.optionItems = items
            menuHueType.selectedIndex = 0
        }

        // init hue light channel option spinbox
        spinHueChannel.from = 1
        spinHueChannel.to = 50
        spinHueChannel.visible = false

        // init hue control option menu
        if (menuHueControl.count === 0) {
            items = []
            v = DigishowEnvironment.ControlLightR;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightG;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightB;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightSat; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightBri; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightHue; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightCt;  items.push({ text: digishow.getLightControlName(v), value: v })
            menuHueControl.optionItems = items
            menuHueControl.selectedIndex = 4 // default is brightness
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var control = menuHueControl.selectedItemValue
        var enables = {}

        if (control === DigishowEnvironment.ControlLightBri) {

            enables["optInitialA"] = true
        }

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}

