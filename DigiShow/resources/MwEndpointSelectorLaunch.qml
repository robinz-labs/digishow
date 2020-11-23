import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLaunch

    property alias menuChannel: menuLaunchChannel


    COptionButton {
        id: buttonLaunchChannel
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuLaunchChannel.selectedItemText
        onClicked: menuLaunchChannel.showOptions()

        COptionMenu {
            id: menuLaunchChannel
        }
    }


    function refresh() {

        // init launch channel option menu
        var items
        var n

        if (menuLaunchChannel.count === 0) {
            items = []
            for (n=1 ; n<=50 ; n++)
                items.push({ text: qsTr("Preset") + " " + n, value: n })
            menuLaunchChannel.optionItems = items
            menuLaunchChannel.selectedIndex = 0
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialB"] = true

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}

