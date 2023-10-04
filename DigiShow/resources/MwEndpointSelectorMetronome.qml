import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMetronome

    COptionButton {
        id: buttonType
        width: 85
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        visible: forOutput

        COptionMenu {
            id: menuType
        }
    }

    COptionButton {
        id: buttonBeat
        width: 85
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuBeat.selectedItemText
        onClicked: menuBeat.showOptions()

        visible: forInput

        COptionMenu {
            id: menuBeat
        }
    }

    COptionButton {
        id: buttonSustain
        width: 115
        height: 28
        anchors.left: buttonBeat.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuSustain.selectedItemText
        onClicked: menuSustain.showOptions()

        visible: forInput

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
            items.push({ text: qsTr("BPM"    ), value: DigishowEnvironment.EndpointMetronomeBPM,     tag: "bpm" })
            items.push({ text: qsTr("Quantum"), value: DigishowEnvironment.EndpointMetronomeQuantum, tag: "quantum" })
            items.push({ text: qsTr("Run"    ), value: DigishowEnvironment.EndpointMetronomeRun,     tag: "run" })
            items.push({ text: qsTr("Link"   ), value: DigishowEnvironment.EndpointMetronomeLink,    tag: "link" })

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

        moreOptions.resetOptions()
        moreOptions.enableOptions({})
        buttonMoreOptions.visible = false
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        if (forInput) {
            menuBeat.selectOption(endpointInfo["channel"])
            menuSustain.selectOption(endpointInfo["control"])
        }

        if (forOutput) {
            menuType.selectOption(endpointInfo["type"])
        }
    }

    function getEndpointOptions() {

        var options = {}

        if (forInput) {
            options["type"] = "beat"
            options["channel"] = menuBeat.selectedItemValue
            options["control"] = menuSustain.selectedItemValue
        }

        if (forOutput) {
            options["type"] = menuType.selectedItemTag
        }

        return options
    }
}

