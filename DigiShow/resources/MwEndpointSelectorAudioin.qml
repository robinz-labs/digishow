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


    function refresh() {

        var items
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Level"), value: DigishowEnvironment.EndpointAudioInLevel, tag:"level" })

            menuType.optionItems = items
            menuType.selectedIndex = 0
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

        menuType.selectOption(endpointInfo["type"])
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag

        return options
    }
}

