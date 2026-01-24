import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLaunch

    COptionButton {
        id: buttonControl
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuControl.selectedItemText
        onClicked: menuControl.showOptions()

        COptionMenu {
            id: menuControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonChannel
        width: 200
        height: 28
        anchors.left: buttonControl.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuChannel.selectedItemText
        visible: menuControl.selectedItemValue === DigishowEnvironment.ControlMediaStart ||
                 menuControl.selectedItemValue === DigishowEnvironment.ControlMediaStop
        onClicked: menuChannel.showOptions()

        COptionMenu {
            id: menuChannel
        }
    }


    function refresh() {

        var items
        var n
        var v

        // init control option menu
        if (menuControl.count === 0) {
            items = []

            v = DigishowEnvironment.ControlMediaStart;   items.push({ text: qsTr("Launch"),   value: v })
            v = DigishowEnvironment.ControlMediaStop;    items.push({ text: qsTr("Stop"),     value: v })
            v = DigishowEnvironment.ControlMediaStopAll; items.push({ text: qsTr("Stop All"), value: v })

            menuControl.optionItems = items
            menuControl.selectedIndex = 0
        }

        // init channel option menu
        items = []
        var count = quickLaunchView.dataModel.count
        for (n=1 ; n<=count ; n++) {
            var title = quickLaunchView.dataModel.get(n-1).title
            var name = quickLaunchView.defaultItemTitle(n-1)
            items.push({ text: name + (name === title ? "" : " - " + title), value: n })
        }
        menuChannel.optionItems = items
        menuChannel.selectedIndex = 0

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialB"] = true

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        var control = endpointInfo["control"]
        menuControl.selectOption(control !== 0 ? control : DigishowEnvironment.ControlMediaStart)
        menuChannel.selectOption(endpointInfo["channel"])
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = "preset"
        options["control"] = menuControl.selectedItemValue
        options["channel"] = menuChannel.selectedItemValue

        return options
    }
}

