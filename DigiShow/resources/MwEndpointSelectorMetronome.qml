import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMetronome

    property alias spinBeat: spinMetronomeBeat
    property alias spinSustain: spinMetronomeSustain

    COptionButton {
        id: buttonMetronomeBeat
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Beat %1").arg(spinMetronomeBeat.value)
        onClicked: spinMetronomeBeat.visible = true

        COptionSpinBox {
            id: spinMetronomeBeat
        }
    }

    COptionButton {
        id: buttonMetronomeSustain
        width: 100
        height: 28
        anchors.left: buttonMetronomeBeat.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Sustain %1").arg(spinMetronomeSustain.value)
        onClicked: spinMetronomeSustain.visible = true

        COptionSpinBox {
            id: spinMetronomeSustain
        }
    }


    function refresh() {

        // init metronome beat option spinbox
        spinMetronomeBeat.from = 1
        spinMetronomeBeat.to = 32
        spinMetronomeBeat.visible = false

        // init metronome sustain option spinbox
        spinMetronomeSustain.from = 0
        spinMetronomeSustain.to = 32
        spinMetronomeSustain.visible = false
        spinMetronomeSustain.value = 1

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        moreOptions.resetOptions()
        moreOptions.enableOptions({})
        buttonMoreOptions.visible = false
    }

}

