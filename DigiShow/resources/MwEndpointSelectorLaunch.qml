import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLaunch

    COptionButton {
        id: buttonChannel
        width: 210
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuChannel.selectedItemText
        visible: menuType.selectedItemValue !== DigishowEnvironment.EndpointLaunchPreset ||
                 menuControl.selectedItemValue !== DigishowEnvironment.ControlMediaStopAll
        onClicked: menuChannel.showOptions()

        COptionMenu {
            id: menuChannel
        }
    }

    COptionButton {
        id: buttonType
        width: 100
        height: 28
        anchors.left: buttonChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuType.selectedItemText
        visible: menuType.selectedItemValue !== DigishowEnvironment.EndpointLaunchPreset
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: refreshMoreOptions()
        }
    }

    Text {
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.verticalCenter: buttonType.verticalCenter
        font.pixelSize: 12
        font.bold: false
        text: {
            switch (menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointLaunchTime:
                return qsTr("ms")
            }
            return ""
        }
        color: "#666"
    }

    COptionButton {
        id: buttonControl
        width: 100
        height: 28
        anchors.left: menuControl.selectedItemValue === DigishowEnvironment.ControlMediaStopAll ? parent.left : buttonChannel.right
        anchors.leftMargin: menuControl.selectedItemValue === DigishowEnvironment.ControlMediaStopAll ? 0 : 10
        anchors.top: parent.top
        text: menuControl.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointLaunchPreset
        onClicked: menuControl.showOptions()

        COptionMenu {
            id: menuControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    function refresh() {

        var items
        var n
        var v

        // init type option menu
        if (menuType.count === 0) {
            items = []
            if (forInput) {
                items.push({ text: qsTr("Playing"), value: DigishowEnvironment.EndpointLaunchPlaying, tag:"playing" })
                //items.push({ text: qsTr("Time"), value: DigishowEnvironment.EndpointLaunchTime, tag:"time" })
            } else if (forOutput) {
                items.push({ text: qsTr("Preset"), value: DigishowEnvironment.EndpointLaunchPreset, tag:"preset" })
            }
            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init control option menu
        if (menuControl.count === 0) {
            items = []

            v = DigishowEnvironment.ControlMediaStart;   items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaRestart; items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaStop;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaStopAll; items.push({ text: digishow.getMediaControlName(v), value: v })

            menuControl.optionItems = items
            menuControl.selectedIndex = 1
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

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointLaunchPreset) {
            enables["optInitialB"] = true
        } else if (endpointType === DigishowEnvironment.EndpointLaunchTime) {
            enables["optRangeMSec"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuType.selectOption(endpointInfo["type"])

        if (buttonControl.visible)
            menuControl.selectOption(endpointInfo["control"])

        if (buttonChannel.visible)
            menuChannel.selectOption(endpointInfo["channel"])
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag

        if (buttonControl.visible)
            options["control"] = menuControl.selectedItemValue

        if (buttonChannel.visible)
            options["channel"] = menuChannel.selectedItemValue

        return options
    }
}

