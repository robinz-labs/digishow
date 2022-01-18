import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMidi

    property alias menuChannel: menuMidiChannel
    property alias menuType:    menuMidiType
    property alias menuNote:    menuMidiNote
    property alias menuControl: menuMidiControl

    COptionButton {
        id: buttonMidiChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuMidiChannel.selectedItemText
        onClicked: menuMidiChannel.showOptions()

        COptionMenu {
            id: menuMidiChannel
        }
    }

    COptionButton {
        id: buttonMidiType
        width: 100
        height: 28
        anchors.left: buttonMidiChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMidiType.selectedItemText
        onClicked: menuMidiType.showOptions()

        COptionMenu {
            id: menuMidiType

            onOptionSelected: refreshMoreOptions()
        }

    }

    COptionButton {
        id: buttonMidiNote
        width: 120
        height: 28
        anchors.left: buttonMidiType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMidiNote.selectedItemText
        visible: menuMidiType.selectedItemValue === DigishowEnvironment.EndpointMidiNote
        onClicked: menuMidiNote.showOptions()

        COptionMenu {
            id: menuMidiNote
        }
    }

    COptionButton {
        id: buttonMidiControl
        width: 130
        height: 28
        anchors.left: buttonMidiType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMidiControl.selectedItemText
        visible: menuMidiType.selectedItemValue === DigishowEnvironment.EndpointMidiControl
        onClicked: menuMidiControl.showOptions()

        COptionMenu {
            id: menuMidiControl
        }
    }

    function refresh() {

        var items
        var n

        // init midi channel option menu
        if (menuMidiChannel.count === 0) {
            items = []
            for (n=0 ; n<16 ; n++)
                items.push({ text: qsTr("Channel") + " " + (n+1), value: n })
            menuMidiChannel.optionItems = items
            menuMidiChannel.selectedIndex = 0
        }

        // init midi type option menu
        if (menuMidiType.count === 0) {
            items = []
            items.push({ text: qsTr("MIDI Note"   ), value: DigishowEnvironment.EndpointMidiNote,    tag:"note"    })
            items.push({ text: qsTr("MIDI Control"), value: DigishowEnvironment.EndpointMidiControl, tag:"control" })
            items.push({ text: qsTr("MIDI Program"), value: DigishowEnvironment.EndpointMidiProgram, tag:"program" })

            if (digishow.appExperimental()) {
                items.push({
                    text: qsTr("MIDI CC Pulse"),
                    value: DigishowEnvironment.EndpointMidiCcPulse,
                    tag:"cc_pulse" })
            }

            menuMidiType.optionItems = items
            menuMidiType.selectedIndex = 0
        }

        // init midi note option menu
        if (menuMidiNote.count === 0) {
            items = []
            for (n=127 ; n>=0 ; n--) {
                var noteName = digishow.getMidiNoteName(n, true)
                items.push({ text: noteName, value: n })
            }
            menuMidiNote.optionItems = items
            menuMidiNote.selectedIndex = 91 // C1
        }

        // init midi cc option menu
        if (menuMidiControl.count === 0) {
            items = []
            for (n=0 ; n<128 ; n++)
                items[n] = { text: qsTr("CC") + " " + n + "  " + digishow.getMidiControlName(n), value: n }
            menuMidiControl.optionItems = items
            menuMidiControl.selectedIndex = 0
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuMidiType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointMidiControl ||
            endpointType === DigishowEnvironment.EndpointMidiProgram) {

            enables["optInitialMidi"] = true
        }

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}

