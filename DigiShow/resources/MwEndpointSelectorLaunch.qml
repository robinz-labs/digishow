import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLaunch

    COptionButton {
        id: buttonChannel
        width: 210
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuChannel.selectedItemText
        onClicked: menuChannel.showOptions()

        COptionMenu {
            id: menuChannel
        }
    }


    function refresh() {

        // init launch channel option menu
        var items = []
        var n
        var count = quickLaunchView.dataModel.count
        for (n=1 ; n<=count ; n++) {
            var title = quickLaunchView.dataModel.get(n-1).title
            var name = quickLaunchView.defaultItemTitle(n-1)
            items.push({ text: name + (name === title ? "" : " - " + title), value: n })
        }
        menuChannel.optionItems = items
        menuChannel.selectedIndex = 0

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialB"] = true

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuChannel.selectOption(endpointInfo["channel"])
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = "preset"
        options["channel"] = menuChannel.selectedItemValue

        return options
    }
}

