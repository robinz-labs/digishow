import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.0

import DigiShow 1.0

import "components"

Item {
    id: itemAudioin

    property alias menuType: menuAudioinType

    COptionButton {
        id: buttonAudioinType
        width: 120
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuAudioinType.selectedItemText
        onClicked: menuAudioinType.showOptions()

        COptionMenu {
            id: menuAudioinType

            onOptionSelected: refreshMoreOptions()
        }

    }


    function refresh() {

        var items
        if (menuAudioinType.count === 0) {
            items = []
            items.push({ text: qsTr("Sound Level"), value: DigishowEnvironment.EndpointAudioInSoundLevel, tag:"soundlevel" })

            menuAudioinType.optionItems = items
            menuAudioinType.selectedIndex = 0
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

