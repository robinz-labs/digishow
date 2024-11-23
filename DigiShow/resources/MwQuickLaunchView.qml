import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"
import "components/CColor.js" as CColor

Item {

    id: quickLaunchView

    property string editingLaunchName: "" // name of the launch item that is being edited
    property bool opened: height > 0
    property alias dataModel: dataModel

    width: 710
    height: 300

    Behavior on height { NumberAnimation { duration: 120 } }

    Rectangle {
        anchors.fill: parent
        color: "#181818"
    }

    ListModel {

        id: dataModel

        ListElement {
            name: ""
            title: ""
            color: "#000000"
            assigned: false
        }
    }

    GridView {
        id: gridView
        model: dataModel
        anchors.fill: parent
        anchors.margins: 20
        anchors.bottomMargin: 5
        cellWidth: width / 5
        cellHeight: height / 5
        clip: true
        snapMode: ListView.SnapToItem
        focus: true
        enabled: editingLaunchName === ""

        delegate: Rectangle {

            width: gridView.cellWidth
            height: gridView.cellHeight
            color: "transparent"

            CButton {
                id: buttonLaunch

                property bool isEditing: model.name === editingLaunchName

                anchors.fill: parent
                anchors.margins: 3
                colorNormal: isEditing ? "transparent" : model.color
                colorActivated: "#000000"
                box.border.color: "#ffffff"
                box.border.width: isEditing || mouseOver ? 1 : 0
                label.font.pixelSize: 12
                label.font.bold: model.assigned
                label.text: model.title
                label.visible: !textLaunchTitle.visible
                opacity: model.assigned || isEditing ? 1.0 : 0.25
                supportLongPress: true

                onLongPressed: {

                    //gridView.currentIndex = model.index
                    //popupOptions.open()

                    gridView.currentIndex = model.index
                    menu.x = mouseX
                    menu.y = mouseY
                    menu.open()
                }

                onRightClicked: {

                    gridView.currentIndex = model.index
                    menu.x = mouseX
                    menu.y = mouseY
                    menu.open()
                }

                onClicked: {

                    gridView.currentIndex = model.index
                    app.startLaunch(model.name)
                }

                TextInput {
                    id: textLaunchTitle
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    font.bold: true
                    color: "#eeeeee"
                    selectionColor: "#666666"
                    selectedTextColor: "#ffffff"
                    selectByMouse: true
                    focus: true
                    visible: false

                    onEditingFinished: { doRename() }
                    onFocusChanged: { if (!textLaunchTitle.focus) textLaunchTitle.visible = false }
                    Keys.onPressed: { if (event.key === Qt.Key_Escape) textLaunchTitle.visible = false }

                    function showRename() {
                        textLaunchTitle.text = model.title
                        textLaunchTitle.visible = true
                        textLaunchTitle.selectAll()
                        textLaunchTitle.forceActiveFocus()
                    }

                    function doRename() {
                        var title = textLaunchTitle.text.trim()
                        model.title = title
                        app.setLaunchOption(model.name, "title", title)
                        textLaunchTitle.visible = false
                        window.isModified = true
                    }
                }

                CMenu {
                    id: menu
                    width: 160
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    CMenuItem {
                        text: model.assigned ? qsTr("Edit Preset") : qsTr("Create Preset")
                        onTriggered: {
                            menu.close()
                            popupOptions.open()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Rename")
                        onTriggered: {
                            menu.close()
                            textLaunchTitle.showRename()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Set Color")
                        onTriggered: {

                            dialogColorPicker.callbackAfterPicked = function() {
                                var color = dialogColorPicker.color.toString()
                                model.color = color
                                app.setLaunchOption(model.name, "color", color)
                            }
                            dialogColorPicker.color = model.color
                            dialogColorPicker.open()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Delete")
                        enabled: model.assigned
                        onTriggered: {
                            menu.close()
                            model.assigned = false
                            model.title = defaultItemTitle(model.index)
                            model.color = defaultItemColor(model.index)
                            app.deleteLaunch(model.name)
                            window.isModified = true
                        }
                    }
                    MenuSeparator {}
                    CMenuItem {
                        spacing: 7
                        text: qsTr("Remote Control ...")
                        checked: remoteWeb.isRunning
                        onTriggered: {
                            menu.close()
                            popupRemote.open()
                        }
                    }
                }
            }
        }

        Popup {
            id: popupRemote

            width: 500
            height: 200
            anchors.centerIn: parent
            modal: false
            focus: true
            //closePolicy: Popup.NoAutoClose

            onVisibleChanged: {

                if (popupRemote.visible) {

                    // when opened the popup
                    txtRemotePort.text = remoteWeb.port

                } else {

                    // when closed the popup
                }
            }

            background: Rectangle {
                anchors.fill: parent
                color: "#222222"
                opacity: 0.9
                radius: 5
                border.width: 1
                border.color: "#999999"
            }

            Column {
                anchors.fill: parent
                spacing: 10

                Label {
                    height: 80
                    anchors.left: parent.left
                    anchors.right: parent.right
                    topPadding: 10
                    leftPadding: 10
                    rightPadding: 10
                    lineHeight: 1.5
                    wrapMode: Text.WordWrap
                    font.bold: false
                    font.pixelSize: 12
                    text: qsTr("Enable DigiShow built-in web service, and you can remotely operate the Preset Launcher through a web browser on your smart phone or mobile tablet. Please enter the service port number and click the Start button.")
                }

                CTextInputBox {
                    id: txtRemotePort
                    width: 120
                    anchors.left: parent.left
                    anchors.margins: 10
                    validator: IntValidator {
                        bottom: 0
                        top: 65535
                    }
                    text: ""

                    Label {
                        anchors.left: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 10
                        color: "#999"
                        text: "http://" + utilities.hostIpAddress() + ":" + parseInt(txtRemotePort.text)
                    }
                }

                Item {
                    width: 220
                    height: 40
                    anchors.right: parent.right

                    CButton {

                        property bool isFirstTime: true

                        width: 120
                        height: 28

                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        label.font.bold: true
                        label.font.pixelSize: 11
                        label.text: qsTr("Start")
                        colorNormal: Material.accent
                        box.radius: 3

                        onClicked: {
                            popupRemote.close()

                            var port = parseInt(txtRemotePort.text)
                            remoteWeb.setPort(port)
                            remoteWeb.setRunning(true)

                            var url = "http://" + utilities.hostIpAddress() + ":" + remoteWeb.port
                            Qt.openUrlExternally(url)

                            window.isModified = true
                        }
                    }

                    CButton {
                        width: 80
                        height: 28

                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        label.font.bold: true
                        label.font.pixelSize: 11
                        label.text: qsTr("Stop")
                        box.radius: 3

                        onClicked: {
                            popupRemote.close()
                            remoteWeb.setRunning(false)
                            window.isModified = true
                        }
                    }
                }
            }
        }

        Popup {
            id: popupOptions

            width: 320
            height: 250
            anchors.centerIn: parent
            modal: false
            focus: true
            closePolicy: Popup.NoAutoClose

            onVisibleChanged: {

                if (popupOptions.visible) {

                    // when opened the options popup

                    var model = dataModel.get(gridView.currentIndex)
                    quickLaunchView.editingLaunchName = model.name

                    var slotLaunchOptions = app.getSlotLaunchOptions(model.name)
                    slotListView.setSlotLaunchOptions(slotLaunchOptions)

                    /*
                    if (slotLaunchOptions.length > 0) {
                        var rememberLink = false
                        var rememberOutput = false
                        for (var n=0 ; n<slotLaunchOptions.length ; n++) {
                            var options = slotLaunchOptions[n]
                            if (options["rememberLink"]) rememberLink = true
                            if (options["rememberOutput"]) rememberOutput = true
                        }
                        //checkRememberLink.checked = rememberLink
                        //checkRememberOutput.checked = rememberOutput
                    }
                    */

                } else {

                    // when closed the options popup

                    quickLaunchView.editingLaunchName = "" // nothing is being edited
                }
            }

            background: Rectangle {
                anchors.fill: parent
                color: "#222222"
                opacity: 0.9
                radius: 5
                border.width: 1
                border.color: "#999999"
            }

            Column {
                anchors.fill: parent
                spacing: 10

                Label {
                    height: 80
                    width: 300
                    topPadding: 10
                    leftPadding: 10
                    rightPadding: 10
                    bottomPadding: 5
                    lineHeight: 1.5
                    wrapMode: Text.WordWrap
                    font.bold: false
                    font.pixelSize: 12
                    text: qsTr("Check the boxes next to the output faders and LINK buttons to memorize their values and states to the preset.")
                }

                Item {
                    width: 220
                    height: 30
                    anchors.right: parent.right

                    CButton {

                        property bool isFirstTime: true

                        width: 120
                        height: 28

                        anchors.top: parent.top
                        anchors.left: parent.left
                        label.font.bold: true
                        label.font.pixelSize: 11
                        label.text: qsTr("Save Preset")
                        colorNormal: Material.accent
                        box.radius: 3

                        onClicked: {

                            popupOptions.close()

                            var model = dataModel.get(gridView.currentIndex)
                            model.assigned = true
                            app.updateLaunch(model.name, slotListView.getSlotLaunchOptions())
                            app.setLaunchOption(model.name, "title", model.title)
                            app.setLaunchOption(model.name, "color", model.color)

                            if (isFirstTime)
                                messageBox.show(qsTr("Took a snapshot for all checked items, which has been saved in a preset. Now, you can tap the button anytime to launch the preset."), qsTr("OK"))
                            isFirstTime = false
                            window.isModified = true
                        }
                    }

                    CButton {
                        width: 80
                        height: 28

                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        label.font.bold: true
                        label.font.pixelSize: 11
                        label.text: qsTr("Cancel")
                        box.radius: 3

                        onClicked: {
                            popupOptions.close()
                        }
                    }
                }

                Item {
                    height: 30
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Rectangle {
                        height: 1
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: "#333333"
                    }
                }

                Label {
                    id: labelRememberOutput
                    height: 22
                    anchors.left: parent.left
                    anchors.right: parent.right
                    leftPadding: 10
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                    color: "#999999"
                    text: qsTr("Output faders")

                    CButton {
                        id: buttonRememberOutputNone

                        width: 80
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        colorNormal: "black"
                        box.radius: 3
                        label.text: qsTr("Check None")
                        label.font.bold: false
                        label.font.pixelSize: 9
                        onClicked: slotListView.setSlotLaunchRememberAllOutputs(false)
                    }

                    CButton {
                        id: buttonRememberOutputAll

                        width: 80
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: buttonRememberOutputNone.left
                        anchors.rightMargin: 5
                        //colorNormal: Material.accent
                        box.radius: 3
                        label.text: qsTr("Check All")
                        label.font.bold: false
                        label.font.pixelSize: 9
                        onClicked: slotListView.setSlotLaunchRememberAllOutputs(true)
                    }
                }

                Label {
                    id: labelRememberLink
                    height: 22
                    anchors.left: parent.left
                    anchors.right: parent.right
                    leftPadding: 10
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                    color: "#999999"
                    text: qsTr("LINK buttons")

                    CButton {
                        id: buttonRememberLinkNone

                        width: 80
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        colorNormal: "black"
                        box.radius: 3
                        label.text: qsTr("Check None")
                        label.font.bold: false
                        label.font.pixelSize: 9
                        onClicked: slotListView.setSlotLaunchRememberAllLinks(false)
                    }

                    CButton {
                        id: buttonRememberLinkAll

                        width: 80
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: buttonRememberLinkNone.left
                        anchors.rightMargin: 5
                        //colorNormal: Material.accent
                        box.radius: 3
                        label.text: qsTr("Check All")
                        label.font.bold: false
                        label.font.pixelSize: 9
                        onClicked: slotListView.setSlotLaunchRememberAllLinks(true)
                    }
                }

            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOff
        }
    }

    Component.onCompleted: {

    }

    function open() { height = 300 }

    function close() { if (!popupOptions.visible) height = 0 }

    function refresh() {

        // prepare launches data model
        dataModel.clear()
        for (var n=0 ; n<50 ; n++) {

            // create default data
            var name = "launch" + (n+1)
            var color = defaultItemColor(n)
            var title = defaultItemTitle(n)
            var assigned = false

            // load user data
            var launch = app.getLaunchOptions(name)
            if (Object.keys(launch).length > 0) {

                if (launch["assigned"] !== undefined) assigned = launch["assigned"]
                if (launch["title"]    !== undefined) title    = launch["title"]
                if (launch["color"]    !== undefined) color    = launch["color"]
            }

            // make data model for the grid view
            dataModel.append({
                name: name,
                title: title,
                color: color,
                assigned: assigned
            });
        }

        editingLaunchName = ""
    }

    function defaultItemColor(index) {
        var row = Math.floor(index / 5)
        switch (row % 5) {
        case 0: return CColor.Lime
        case 1: return CColor.Lake
        case 2: return CColor.Ocean
        case 3: return CColor.Iris
        case 4: return CColor.HotPink
        }
    }

    function defaultItemTitle(index) {
        return qsTr("Preset") + " " + (index + 1)
    }

}
