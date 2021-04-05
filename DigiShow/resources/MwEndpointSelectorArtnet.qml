import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemArtnet

    property alias spinUnit: spinArtnetUnit
    property alias spinChannel: spinArtnetChannel

    COptionButton {
        id: buttonArtnetUnit
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Universe") + " " + spinArtnetUnit.value
        onClicked: spinArtnetUnit.visible = true

        COptionSpinBox {
            id: spinArtnetUnit
        }
    }

    COptionButton {
        id: buttonArtnetChannel
        width: 100
        height: 28
        anchors.left: buttonArtnetUnit.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Channel") + " " + spinArtnetChannel.value
        onClicked: spinArtnetChannel.visible = true

        COptionSpinBox {
            id: spinArtnetChannel
        }
    }


    function refresh() {

        // init artnet universe option spinbox
        spinArtnetUnit.from = 0
        spinArtnetUnit.to = 32767
        spinArtnetUnit.visible = false

        // init artnet channel option spinbox
        spinArtnetChannel.from = 1
        spinArtnetChannel.to = 512
        spinArtnetChannel.visible = false

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialDmx"] = true

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}

