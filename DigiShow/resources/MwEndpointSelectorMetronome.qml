import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMetronome

    COptionButton {
        id: buttonType
        width: 115
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
        id: buttonBeat
        width: 85
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuBeat.selectedItemText
        onClicked: menuBeat.showOptions()

        visible: menuType.selectedItemTag === "beat"

        COptionMenu {
            id: menuBeat
        }
    }

    COptionButton {
        id: buttonSustain
        width: 130
        height: 28
        anchors.left: buttonBeat.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuSustain.selectedItemText
        onClicked: menuSustain.showOptions()

        visible: menuType.selectedItemTag === "beat"

        COptionMenu {
            id: menuSustain
        }
    }

    function refresh() {

        var items
        var n

        // init metronome type menu (for control output)
        if (menuType.count === 0) {
            items = []

            if (forInput) {

                items.push({ text: qsTr("Beat Trigger"  ), value: DigishowEnvironment.EndpointMetronomeBeat,    tag: "beat" })
                items.push({ text: qsTr("Beat Count"    ), value: DigishowEnvironment.EndpointMetronomeCount,   tag: "count" })

            } else if (forOutput) {

                items.push({ text: qsTr("Reset"         ), value: DigishowEnvironment.EndpointMetronomeReset,   tag: "reset" })
                items.push({ text: qsTr("BPM Change"    ), value: DigishowEnvironment.EndpointMetronomeBPM,     tag: "bpm" })
                items.push({ text: qsTr("Quantum Change"), value: DigishowEnvironment.EndpointMetronomeQuantum, tag: "quantum" })
                items.push({ text: qsTr("Run ON"        ), value: DigishowEnvironment.EndpointMetronomeRun,     tag: "run" })
                items.push({ text: qsTr("Link ON"       ), value: DigishowEnvironment.EndpointMetronomeLink,    tag: "link" })
                items.push({ text: qsTr("Tap"           ), value: DigishowEnvironment.EndpointMetronomeTap,     tag: "tap" })
            }

            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init metronome beat option menu
        if (menuBeat.count === 0) {
            items = []
            for (n=0 ; n<48 ; n++) {
                var beatNumber = (Math.floor(n / 4) + 1).toString()
                if (n % 4 !== 0) beatNumber += "." + (n % 4 + 1).toString()
                items.push({ text: qsTr("Beat %1").arg(beatNumber) , value: n+1 })
            }
            menuBeat.optionItems = items
            menuBeat.selectedIndex = 0
        }

        // init metronome sustain option menu
        if (menuSustain.count === 0) {
            items = []
            for (n=0 ; n<48 ; n++) {
                var sb = Math.floor((n+1) / 4)
                var sq = n % 4 + 1
                var sustainNumber
                if (sq === 4)
                    sustainNumber = sb.toString() + " " + (sb === 1 ? qsTr("beat") : qsTr("beats"))
                else
                    sustainNumber = (sb === 0 ? "" : sb.toString() + " ") + sq.toString() + "/4"
                items.push({ text: qsTr("Sustain %1").arg(sustainNumber) , value: n+1 })
            }
            menuSustain.optionItems = items
            menuSustain.selectedIndex = 3
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions({})
        buttonMoreOptions.visible = false
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        var type = endpointInfo["type"]
        menuType.selectOption(type)

        if (type === "beat") {
            menuBeat.selectOption(endpointInfo["channel"])
            menuSustain.selectOption(endpointInfo["control"])
        }
    }

    function getEndpointOptions() {

        var options = {}

        var type = menuType.selectedItemTag
        options["type"] = type

        if (type === "beat") {
            options["channel"] = menuBeat.selectedItemValue
            options["control"] = menuSustain.selectedItemValue
        }

        return options
    }
}

