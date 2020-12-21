import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLaunch

    property alias menuChannel: menuLaunchChannel


    COptionButton {
        id: buttonLaunchChannel
        width: 120
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
        var items = []
        var n
        var count = quickLaunchView.dataModel.count
        for (n=1 ; n<=count ; n++) {
            var title = quickLaunchView.dataModel.get(n-1).title
            items.push({ text: (title === "" ? qsTr("Preset") + " " + n : title), value: n })
        }
        menuLaunchChannel.optionItems = items
        menuLaunchChannel.selectedIndex = 0

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

