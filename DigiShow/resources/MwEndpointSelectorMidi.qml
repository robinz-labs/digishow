import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMidi

    COptionButton {
        id: buttonChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuChannel.selectedItemText
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
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: refreshMoreOptions()
        }

    }

    COptionButton {
        id: buttonNote
        width: 120
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuNote.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointMidiNote
        onClicked: menuNote.showOptions()

        COptionMenu {
            id: menuNote
        }
    }

    COptionButton {
        id: buttonControl
        width: 130
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuControl.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointMidiControl
        onClicked: menuControl.showOptions()

        COptionMenu {
            id: menuControl
        }
    }

    function refresh() {

        var items
        var n

        // init midi channel option menu
        if (menuChannel.count === 0) {
            items = []
            for (n=0 ; n<16 ; n++)
                items.push({ text: qsTr("Channel") + " " + (n+1), value: n })
            menuChannel.optionItems = items
            menuChannel.selectedIndex = 0
        }

        // init midi type option menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("MIDI Note"   ), value: DigishowEnvironment.EndpointMidiNote,    tag:"note"    })
            items.push({ text: qsTr("MIDI Control"), value: DigishowEnvironment.EndpointMidiControl, tag:"control" })
            items.push({ text: qsTr("MIDI Program"), value: DigishowEnvironment.EndpointMidiProgram, tag:"program" })
            items.push({ text: qsTr("MIDI Pitch"  ), value: DigishowEnvironment.EndpointMidiPitch,   tag:"pitch"   })

            if (digishow.appExperimental()) {
                items.push({
                    text: qsTr("MIDI CC Pulse"),
                    value: DigishowEnvironment.EndpointMidiCcPulse,
                    tag:"cc_pulse" })
            }

            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init midi note option menu
        if (menuNote.count === 0) {
            items = []
            for (n=127 ; n>=0 ; n--) {
                var noteName = digishow.getMidiNoteName(n, true)
                items.push({ text: noteName, value: n })
            }
            menuNote.optionItems = items
            menuNote.selectedIndex = 91 // C1
        }

        // init midi cc option menu
        if (menuControl.count === 0) {
            items = []
            for (n=0 ; n<128 ; n++)
                items[n] = { text: qsTr("CC") + " " + n + "  " + digishow.getMidiControlName(n), value: n }
            menuControl.optionItems = items
            menuControl.selectedIndex = 0
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointMidiControl ||
            endpointType === DigishowEnvironment.EndpointMidiProgram) {

            enables["optInitialMidi"] = true

        } else if (endpointType === DigishowEnvironment.EndpointMidiPitch) {

           enables["optInitialA"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuChannel.selectOption(endpointInfo["channel"])
        menuType.selectOption(endpointInfo["type"])

        switch (endpointInfo["type"]) {
        case DigishowEnvironment.EndpointMidiNote:
            menuNote.selectOption(endpointInfo["note"])
            break
        case DigishowEnvironment.EndpointMidiControl:
            menuControl.selectOption(endpointInfo["control"])
            break
        }
    }

    function getEndpointOptions() {

        var options = {}
        options["channel"] = menuChannel.selectedItemValue
        options["type"] = menuType.selectedItemTag

        switch (menuType.selectedItemValue) {
        case DigishowEnvironment.EndpointMidiNote:
            options["note"] = menuNote.selectedItemValue
            break
        case DigishowEnvironment.EndpointMidiControl:
            options["control"] = menuControl.selectedItemValue
            break
        }

        return options
    }

    function learn(rawData) {

        var event = rawData["event"]
        if (event === "note_on" || event === "note_off") {

            menuType.selectOptionWithTag("note")
            menuNote.selectOption(rawData["note"])

        } else if (event === "control_change") {

            menuType.selectOptionWithTag("control")
            menuControl.selectOption(rawData["control"])

        } else if (event === "program_change") {

            menuType.selectOptionWithTag("program")

        } else if (event === "pitch_bend") {

            menuType.selectOptionWithTag("pitch")

        }
        menuChannel.selectOption(rawData["channel"])

    }

}

