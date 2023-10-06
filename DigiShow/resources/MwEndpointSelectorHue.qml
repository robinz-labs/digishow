import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemHue

    COptionButton {
        id: buttonType
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType
        }
    }

    COptionButton {
        id: buttonChannel
        width: 100
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("ID") + " " + spinChannel.value
        onClicked: spinChannel.visible = true

        COptionSpinBox {
            id: spinChannel
        }
    }

    COptionButton {
        id: buttonControl
        width: 100
        height: 28
        anchors.left: buttonChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuControl.selectedItemText
        onClicked: menuControl.showOptions()

        COptionMenu {
            id: menuControl

            onOptionSelected: refreshMoreOptions()
        }
    }


    function refresh() {

        var items
        var v

        // init hue type option menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Light"), value: DigishowEnvironment.EndpointHueLight,  tag:"light" })
            items.push({ text: qsTr("Group"), value: DigishowEnvironment.EndpointHueGroup,  tag:"group" })
            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init hue light channel option spinbox
        spinChannel.from = 1
        spinChannel.to = 50
        spinChannel.visible = false

        // init hue control option menu
        if (menuControl.count === 0) {
            items = []
            v = DigishowEnvironment.ControlLightR;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightG;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightB;   items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightSat; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightBri; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightHue; items.push({ text: digishow.getLightControlName(v), value: v })
            //v = DigishowEnvironment.ControlLightCt;  items.push({ text: digishow.getLightControlName(v), value: v })
            menuControl.optionItems = items
            menuControl.selectedIndex = 4 // default is brightness
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var control = menuControl.selectedItemValue
        var enables = {}

        if (control === DigishowEnvironment.ControlLightBri) {

            enables["optInitialA"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuType.selectOption(endpointInfo["type"])
        spinChannel.value = endpointInfo["channel"]
        menuControl.selectOption(endpointInfo["control"])
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag
        options["channel"] = spinChannel.value
        options["control"] = menuControl.selectedItemValue

        return options
    }
}

