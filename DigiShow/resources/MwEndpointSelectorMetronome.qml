import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMetronome

    property alias menuBeat: menuMetronomeBeat
    property alias menuSustain: menuMetronomeSustain

    COptionButton {
        id: buttonMetronomeBeat
        width: 85
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuMetronomeBeat.selectedItemText
        onClicked: menuMetronomeBeat.showOptions()

        COptionMenu {
            id: menuMetronomeBeat
        }
    }

    COptionButton {
        id: buttonMetronomeSustain
        width: 115
        height: 28
        anchors.left: buttonMetronomeBeat.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMetronomeSustain.selectedItemText
        onClicked: menuMetronomeSustain.showOptions()

        COptionMenu {
            id: menuMetronomeSustain
        }
    }

    function refresh() {

        var items
        var n

        // init metronome beat option menu
        if (menuMetronomeBeat.count === 0) {
            items = []
            for (n=0 ; n<48 ; n++) {
                var beatNumber = (Math.floor(n / 4) + 1).toString()
                if (n % 4 !== 0) beatNumber += "." + (n % 4 + 1).toString()
                items.push({ text: qsTr("Beat %1").arg(beatNumber) , value: n+1 })
            }
            menuMetronomeBeat.optionItems = items
            menuMetronomeBeat.selectedIndex = 0
        }

        // init metronome sustain option menu
        if (menuMetronomeSustain.count === 0) {
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
            menuMetronomeSustain.optionItems = items
            menuMetronomeSustain.selectedIndex = 3
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        moreOptions.resetOptions()
        moreOptions.enableOptions({})
        buttonMoreOptions.visible = false
    }

}

