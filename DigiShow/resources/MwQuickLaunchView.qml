import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"
import "components/CColor.js" as CColor

Item {

    id: quickLaunchView

    property string editingLaunchName: "" // name of the launch item that is being edited
    property bool isEditing: editingLaunchName !== ""
    property bool isInUse: false
    property bool opened: height > 0
    property alias dataModel: dataModel
    property alias currentIndex: gridView.currentIndex

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
            playing: false
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
                property bool isPlaying: model.playing
                property real progress: 0

                onIsPlayingChanged: progress = 0

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
                opacity: isEditing || model.assigned ? 1.0 : 0.25
                supportLongPress: true

                Timer {
                    id: playingTimer
                    interval: 100
                    running: parent.isPlaying
                    repeat: true
                    onTriggered: {
                        var player = cueManager.cuePlayer(model.name)
                        parent.progress = player ? player.position() / player.duration() : 0
                    }
                }

                ProgressBar {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.bottomMargin: 6
                    height: 6
                    visible: parent.isPlaying
                    opacity: 0.8
                    value: parent.progress
                    Material.accent: "#ffffff"
                    Material.foreground: "#ffffff"
                    background: Rectangle {
                        implicitWidth: parent.width
                        implicitHeight: parent.height
                        color: "#333333"
                        opacity: 0.5
                        radius: height / 2
                    }
                    contentItem: Item {
                        implicitWidth: parent.width
                        implicitHeight: parent.height
                        Rectangle {
                            width: parent.width * parent.parent.visualPosition
                            height: parent.height
                            radius: height / 2
                            color: "#ffffff"
                        }
                    }
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
                        id: menuItemStop
                        text: qsTr("Stop")
                        onTriggered: {
                            menu.close()
                            cueManager.stopCue(model.name)
                        }
                    }
                    CMenuItem {
                        text: model.assigned ? qsTr("Edit Preset") : qsTr("Create Preset")
                        onTriggered: {
                            menu.close()
                            cueManager.stopAllCues()
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
                            if (messageBox.showAndWait(
                                qsTr("Do you want to delete the preset ?"),
                                qsTr("Delete"), qsTr("Cancel")) !== 1) return

                            cueManager.stopCue(model.name)
                            app.deleteCuePlayer(model.name)
                            app.deleteLaunch(model.name)

                            model.assigned = false
                            model.title = defaultItemTitle(model.index)
                            model.color = defaultItemColor(model.index)

                            window.isModified = true
                            undoManager.archive()
                        }
                    }
                    MenuSeparator {}
                    CMenuItem {
                        text: qsTr("Copy")
                        onTriggered: copyLaunch(model.name)
                    }
                    CMenuItem {
                        id: menuItemPaste
                        text: qsTr("Paste")
                        onTriggered: pasteLaunch(model.name)
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

                onLongPressed: {
                    showContextMenu(mouseX, mouseY)
                }

                onRightClicked: {
                    showContextMenu(mouseX, mouseY)
                }

                onClicked: {
                    gridView.currentIndex = model.index
                    app.startLaunch(model.name)
                }

                function showContextMenu(mouseX, mouseY) {
                    gridView.currentIndex = model.index
                    menuItemStop.enabled = cueManager.isCuePlaying(model.name)
                    menuItemPaste.enabled = utilities.clipboardExists("application/vnd.digishow.launch") && !model.assigned
                    menu.x = mouseX
                    menu.y = mouseY
                    menu.open()
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

                            window.isModified = true

                            var url = "http://" + utilities.hostIpAddress() + ":" + remoteWeb.port
                            var r = messageBox.showQrAndWait(
                                        url,
                                        qsTr("Please scan the QR code with your smartphone:"),
                                        qsTr("Preview in Web"), qsTr("Close"))

                            if (r===1) Qt.openUrlExternally(url)
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

            width: 450
            height: 240
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

                    var slotCuePlayerOptions = app.getSlotCuePlayerOptions(model.name)
                    slotListView.setSlotCuePlayerOptions(slotCuePlayerOptions)

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
                spacing: 5

                Item {
                    width: parent.width
                    height: 80
                    
                    Label {
                        id: labelFirstTitle
                        width: parent.width - buttonReset.width - 20
                        height: 20
                        anchors.left: parent.left
                        anchors.top: parent.top
                        topPadding: 5
                        leftPadding: 10
                        rightPadding: 10
                        bottomPadding: 0
                        font.bold: true
                        font.pixelSize: 13
                        color: Material.accent
                        text: qsTr("Preset Memory")
                    }

                    Label {
                        id: labelFirst
                        width: parent.width - buttonReset.width - 20
                        height: 35
                        anchors.left: parent.left
                        anchors.top: labelFirstTitle.bottom
                        topPadding: 5
                        leftPadding: 10
                        rightPadding: 10
                        bottomPadding: 0
                        lineHeight: 1.1
                        wrapMode: Text.WordWrap
                        font.bold: false
                        font.pixelSize: 12
                        color: "#cccccc"
                        text: qsTr("Please check the relevant LINK buttons and output signal faders in the signal link table, then save their current states and values into the preset.")
                    }

                    CButton {
                        id: buttonReset
                        width: 60
                        height: 28
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.top: labelFirst.top
                        anchors.topMargin: 5
                        label.font.bold: false
                        label.font.pixelSize: 11
                        label.text: qsTr("Reset")
                        box.border.width: 1
                        colorNormal: "transparent"
                        box.radius: 3

                        onClicked: {
                            slotListView.setSlotLaunchRememberAllLinks(false)
                            slotListView.setSlotLaunchRememberAllOutputs(false)
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 75

                    Label {
                        id: labelSecondTitle
                        width: parent.width - buttonClear.width - 20
                        height: 20
                        anchors.top: parent.top
                        topPadding: 5
                        leftPadding: 10
                        rightPadding: 10
                        bottomPadding: 0
                        font.bold: true
                        font.pixelSize: 13
                        color: Material.accent
                        text: qsTr("Attached Cue Player")
                    }

                    Label {
                        id: labelSecond
                        width: parent.width - buttonClear.width - 20
                        height: 35
                        anchors.top: labelSecondTitle.bottom
                        topPadding: 5
                        leftPadding: 10
                        rightPadding: 10
                        bottomPadding: 0
                        lineHeight: 1.1
                        wrapMode: Text.WordWrap
                        font.bold: false
                        font.pixelSize: 12
                        color: "#cccccc"
                        text: qsTr("You can also click the + CUE button below the output signal fader to design the playback curve of the output signal.")
                    }

                    CButton {
                        id: buttonClear
                        width: 60
                        height: 28
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.top: labelSecond.top
                        anchors.topMargin: 5
                        label.font.bold: false
                        label.font.pixelSize: 11
                        label.text: qsTr("Clear")
                        box.border.width: 1
                        colorNormal: "transparent"
                        box.radius: 3

                        onClicked: {
                            var name = quickLaunchView.editingLaunchName
                            if (app.cuePlayerExists(name)) {
                                cueManager.stopCue(name)
                                if (messageBox.showAndWait(
                                    qsTr("Do you want to delete all cue player data attached to the preset ?"),
                                    qsTr("Delete"), qsTr("Cancel")) === 1) {
                                    app.deleteCuePlayer(name)
                                    slotListView.setSlotCuePlayerOptions(app.getSlotCuePlayerOptions(name))
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                Item {
                    height: 30
                    anchors.left: parent.left
                    anchors.right: parent.right

                    CButton {
                        id: buttonSave

                        width: 130
                        height: 32
                        anchors.top: parent.top
                        anchors.right: buttonCancel.left
                        anchors.rightMargin: 18
                        label.font.bold: true
                        label.font.pixelSize: 12
                        label.text: qsTr("Save Preset")
                        colorNormal: Material.accent
                        box.radius: 3

                        CButton {
                            width: 18
                            height: 18
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            label.text: "?"
                            label.font.pixelSize: 11
                            label.font.bold: true
                            box.radius: 9

                            onClicked: {
                                messageBox.show(qsTr("The saved preset contains:\r\n• The status of the checked items in the signal link table\r\n• The playback curves of the output signal in the cue player\r\n\r\nTap on the preset button to activate them."), qsTr("OK"))
                            }
                        }

                        onClicked: save()
                    }

                    CButton {
                        id: buttonCancel

                        width: 80
                        height: 32
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        label.font.bold: true
                        label.font.pixelSize: 12
                        label.text: qsTr("Cancel")
                        box.radius: 3

                        onClicked: {
                            popupOptions.close()
                        }
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOff
        }
    }

    Component.onCompleted: {

        cueManager.statusUpdated.connect(function() {

            // update playing status for each launch button
            for (var i = 0; i < dataModel.count; i++) {
                var item = dataModel.get(i)
                item.playing = cueManager.isCuePlaying(item.name)
            }
        })
    }

    function refresh() {

        // prepare launches data model
        dataModel.clear()
        isInUse = false
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

                isInUse = true
            }

            // make data model for the grid view
            dataModel.append({
                name: name,
                title: title,
                color: color,
                assigned: assigned,
                playing: false
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

    function open() { height = 300 }

    function close() {
        if (popupOptions.visible) popupOptions.close()
        height = 0
    }

    function confirmEditing() {

        // return values:
        // true -  the preset editing was finished, and go ahead
        // false - the user doesn't want to finish the editing

        if (!quickLaunchView.isEditing) return true

        if (messageBox.showAndWait(
             qsTr("You need to save the preset first, do you want to continue ?"),
             qsTr("Continue"), qsTr("Cancel")) === 1) {

            save()
            return true
        }

        return false
    }

    function save() {

        popupOptions.close()

        var model = dataModel.get(gridView.currentIndex)
        model.assigned = true
        app.updateLaunch(model.name, slotListView.getSlotLaunchOptions())
        app.setLaunchOption(model.name, "title", model.title)
        app.setLaunchOption(model.name, "color", model.color)

        isInUse = true
        window.isModified = true
        undoManager.archive()
    }

    function copyLaunch(name) {
        var data = {
            launchOptions: app.getLaunchOptions(name),
            launchDetails: app.getSlotLaunchDetails(name),
            cuePlayerOptions: app.getCuePlayerOptions(name),
            cuePlayerDetails: app.getSlotCuePlayerDetails(name)
        }
        utilities.copyJson(data, "application/vnd.digishow.launch")
    }

    function pasteLaunch(name) {
        var data = utilities.pasteJson("application/vnd.digishow.launch")

        if (!data) return
        var launchOptions = data.launchOptions
        var launchDetails = data.launchDetails
        var cuePlayerOptions = data.cuePlayerOptions
        var cuePlayerDetails = data.cuePlayerDetails

        // update the launch options and details
        if (launchOptions) app.setLaunchOptions(name, launchOptions)
        if (launchDetails) app.setSlotLaunchDetails(name, launchDetails)
        if (cuePlayerOptions) app.setCuePlayerOptions(name, cuePlayerOptions)
        if (cuePlayerDetails) app.setSlotCuePlayerDetails(name, cuePlayerDetails)

        // look for the same name in the data model
        for (var i=0 ; i<dataModel.count ; i++) {
            var model = dataModel.get(i)
            if (model.name === name) {
                model.title = launchOptions.title
                model.color = launchOptions.color
                model.assigned = true
            }
        }
        
        isInUse = true
        window.isModified = true
        undoManager.archive()
    }

}
