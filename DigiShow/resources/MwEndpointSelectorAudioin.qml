import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.0

import DigiShow 1.0

import "components"

Item {
    id: itemAudioin

    COptionButton {
        id: buttonType
        width: 120
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuType.selectedItemText
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
        text: "-50 dB ~ 0 dB"
        color: "#666"
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointAudioInLevelDb ||
                 menuType.selectedItemValue === DigishowEnvironment.EndpointAudioInPeakDb
    }

    COptionButton {
        id: buttonChannel
        width: 240
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuChannel.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointAudioInSpectrum
        onClicked: menuChannel.showOptions()

        COptionMenu {
            id: menuChannel
        }
    }


    function refresh() {

        var items
        var n

        // init type menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Level")              , value: DigishowEnvironment.EndpointAudioInLevel,    tag:"level"    })
            items.push({ text: qsTr("Level")+" (dB)"      , value: DigishowEnvironment.EndpointAudioInLevelDb,  tag:"level_db" })
            items.push({ text: qsTr("Peak Level")         , value: DigishowEnvironment.EndpointAudioInPeak,     tag:"peak"     })
            items.push({ text: qsTr("Peak Level")+" (dB)" , value: DigishowEnvironment.EndpointAudioInPeakDb,   tag:"peak_db"  })
            items.push({ text: qsTr("Spectrum")           , value: DigishowEnvironment.EndpointAudioInSpectrum, tag:"spectrum" })

            menuType.optionItems = items
            menuType.selectedIndex = 2 // peak level
        }

        // init spectrum band menu
        if (menuChannel.count === 0) {
            items = []
            for (n=1 ; n<=24 ; n++) {
                var bandName = digishow.getSpectrumBandName(n, true)
                items.push({ text: bandName, value: n })
            }
            menuChannel.optionItems = items
            menuChannel.selectOption(8) // band of kick
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

        menuType.selectOption(endpointInfo["type"])
        if (endpointInfo["type"] === DigishowEnvironment.EndpointAudioInSpectrum) {
            menuChannel.selectOption(endpointInfo["channel"])
        }
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag
        if (menuType.selectedItemValue === DigishowEnvironment.EndpointAudioInSpectrum) {
            options["channel"] = menuChannel.selectedItemValue
        }
        return options
    }
}

