import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.0
import DigiShow 1.0

import "components"

ApplicationWindow {

    id: window

    property bool isModified: false
    property bool isBusy: false

    property var listOnline: ({})

    property alias app: digishow.app


    visible: true
    width: 1280
    height: 800
    minimumWidth: 1280
    minimumHeight: 800
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    title: qsTr("DigiShow LINK - ") +
           (app.filepath==="" ? qsTr("Untitled") : utilities.getFileName(app.filepath)) +
           (isModified ? "*" : "") +
           (app.isRunning ? "" : qsTr(" ( stopped )") )


    Utilities { id: utilities }
    CCommon { id: common }
    Digishow { id: digishow }

    Component.onCompleted: {

        // get info of online ports
        listOnline = digishow.listOnline()
        utilities.saveJsonToFile(listOnline, digishow.appDataPath("online.json"))

        // callback while file loaded
        app.filepathChanged.connect(function() {
            console.log("file loaded: " + app.filepath)

            utilities.setMacWindowTitleWithFile(window, app.filepath)
            isModified = false

            quickLaunchView.refresh()
            quickLaunchView.close()
        })

        // callback while slots data loaded
        app.slotListChanged.connect(function() {

            slotListView.currentIndex = -1
            slotListView.selectedIndex = -1
            slotListView.refresh()

            slotDetailView.slotIndex = -1
            slotDetailView.refresh()
        })

        // callback while interfaces data loaded
        app.interfaceListChanged.connect(function() {

            dialogInterfaces.refresh()
        })

        app.newShow()
    }

    onIsModifiedChanged: {
        utilities.setMacWindowIsModified(window, isModified)
    }

    onClosing: {

        app.stop()
        close.accepted = false
        common.runLater(appClose)
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["text/uri-list"]
        onEntered: drag.accepted = drag.hasUrls
        onDropped: {
            if (drop.hasUrls) {
                var fileUrl = drop.urls[0]
                if (fileUrl.startsWith("file://") && fileUrl.endsWith(".dgs")) {
                    drop.acceptProposedAction()
                    common.runLater(function() {
                        fileOpen(utilities.urlToPath(fileUrl))
                    })
                }
            }
        }
    }


    Shortcut {
        sequence: StandardKey.New
        onActivated: createNew()
    }
    Shortcut {
        sequence: StandardKey.Open
        onActivated: open()
    }
    Shortcut {
        sequence: StandardKey.Save
        onActivated: save()
    }
    Shortcut {
        sequence: StandardKey.SaveAs
        onActivated: saveAs()
    }
    Shortcut {
        sequence: StandardKey.Close
        onActivated: close()
    }

    Rectangle {
        id: rectRoot
        color: "#111111"
        anchors.fill: parent

        Rectangle {
            id: rectTopLeftBar
            height: 80
            width: Math.max(rectRoot.width * 0.5, 710)
            anchors.top: parent.top
            anchors.left: parent.left
            color: "#222222"

            CButton {
                id: buttonMenu
                height: 40
                width: 40
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 20
                anchors.leftMargin: 20
                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icon_menu_white.png"
                onClicked: {
                    if (!menu.visible) menu.open()
                    else menu.close()
                }

                Menu {
                    id: menu
                    x: 0
                    y: parent.height + 5
                    transformOrigin: Menu.TopLeft
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                    MenuItem {
                        text: qsTr("New Instance")
                        onTriggered: utilities.newAppInstance()
                    }
                    MenuSeparator {
                        padding: 0
                        contentItem: Rectangle { implicitHeight: 1; color: "#333333" }
                    }
                    MenuItem {
                        text: qsTr("New")
                        onTriggered: {
                            menu.close()
                            createNew()
                        }
                    }
                    MenuItem {
                        text: qsTr("Open ...")
                        onTriggered: {
                            menu.close()
                            open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Save")
                        onTriggered: save()
                    }
                    MenuItem {
                        text: qsTr("Save As ...")
                        onTriggered: saveAs()
                    }
                    MenuItem {
                        enabled: app.filepath!==""
                        text: qsTr("Show File")
                        onTriggered: utilities.showFileInShell(app.filepath)
                    }
                    MenuSeparator {
                        padding: 0
                        contentItem: Rectangle { implicitHeight: 1; color: "#333333" }
                    }
                    MenuItem {
                        text: qsTr("About")
                        onTriggered: popupAbout.open()
                    }
                    MenuItem {
                        text: qsTr("Options ...")
                        onTriggered: dialogAppOptions.show()
                    }
                    MenuItem {
                        text: window.visibility==Window.FullScreen ? qsTr("Exit Full Screen") : qsTr("Enter Full Screen")
                        onTriggered: {
                            if (window.visibility==Window.FullScreen)
                                window.showNormal()
                            else
                                window.showFullScreen()
                        }
                    }
                    MenuSeparator {
                        padding: 0
                        contentItem: Rectangle { implicitHeight: 1; color: "#333333" }
                    }
                    MenuItem {
                        text: qsTr("Close")
                        onTriggered: {
                            menu.close()
                            window.close()
                        }
                    }
                }
            }

            CButton {
                id: buttonNewSlot
                height: 40
                width: 40
                anchors.top: parent.top
                anchors.left: buttonMenu.right
                anchors.topMargin: 20
                anchors.leftMargin: 10
                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icon_add_white.png"

                onClicked: slotListView.newSlot()
            }

            CButton {
                id: buttonLinkAll
                width: 34
                height: 27
                anchors.left: parent.left
                anchors.leftMargin: 226
                anchors.verticalCenter: parent.verticalCenter
                label.text: "LINK"
                label.font.bold: false
                label.font.pixelSize: 9
                box.radius: 3
                box.border.width: mouseOver || app.isPaused ? 1 : 0
                colorNormal: !app.isPaused ? "#666666" : "transparent"
                visible: slotListView.listItemCount > 0
                onClicked: app.pause(!app.isPaused)
            }

            CButton {
                id: buttonQuickLaunch
                width: 90
                height: 27
                anchors.left: parent.right
                anchors.leftMargin: -305
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icon_pad_white.png"
                icon.anchors.horizontalCenterOffset: -28
                label.text: qsTr("PRESETS")
                label.font.bold: false
                label.font.pixelSize: 9
                label.anchors.horizontalCenterOffset: 13
                box.radius: 3
                box.border.width: 1
                colorNormal: quickLaunchView.opened ? "#666666" : "transparent"
                visible: slotListView.listItemCount > 0
                onClicked: {
                    if (quickLaunchView.opened)
                        quickLaunchView.close()
                    else
                        quickLaunchView.open()
                }
            }

            CButton {
                id: buttonStop
                height: 40
                width: 40
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 20
                anchors.rightMargin: 20
                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icon_stop_white.png"
                onClicked: app.stop()
            }

            CButton {

                id: buttonPlay
                height: 40
                width: 40
                anchors.top: parent.top
                anchors.right: buttonStop.left
                anchors.topMargin: 20
                anchors.rightMargin: 10
                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icon_play_white.png"
                colorNormal: {

                    if (app.isRunning)
                        return Material.accent
                    else {
                        if (slotListView.listItemCount === 0)
                            return "#484848"
                        else
                            return (blink.state ? Material.accent : "#484848")
                    }
                }

                onClicked: {

                    window.isBusy = true
                    common.runLater(function(){

                        var done = app.start()
                        window.isBusy = false

                        if (!done) {
                            for (var n=0 ; n<app.interfaceCount() ; n++) {
                               if (app.interfaceAt(n).isInterfaceOpened() === false) {
                                   messageBox.show(qsTr("Error occurred when open interface %1 .").arg(app.interfaceAt(n).getInterfaceInfo()["label"]) , qsTr("OK"))
                                   break;
                               }
                            }
                        }
                    })
                }

                Behavior on colorNormal { ColorAnimation { duration: 200 } }

                Timer {
                    property bool state: false
                    property int step: 0

                    id: blink
                    interval: 200
                    repeat: true
                    running: !app.isRunning
                    onRunningChanged: {
                        state = false
                        step = 0
                    }
                    onTriggered: {
                        step++
                        if (step===10) {
                            state = true
                            step = 0
                        } else {
                            state = false
                        }
                    }
                }
            }
        }

        Rectangle {
            id: rectTopRightBar
            height: 80
            anchors.top: parent.top
            anchors.left: rectTopLeftBar.right
            anchors.right: parent.right
            color: rectTopLeftBar.color

            CButton {
                id: buttonInterfaceSettings
                height: 48
                width: 120
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 16
                anchors.rightMargin: 20
                icon.width: 24
                icon.height: 24
                icon.anchors.horizontalCenterOffset: -35
                icon.source: "qrc:///images/icon_port_white.png"
                label.text: qsTr("Interface Manager")
                label.wrapMode: Text.WordWrap
                label.font.pixelSize: 11
                label.lineHeight: 1.15
                label.horizontalAlignment: Text.AlignLeft
                label.leftPadding: 50

                onClicked: {
                    if (!menuInterfaceSettings.visible) menuInterfaceSettings.open()
                    else menuInterfaceSettings.close()
                }

                Menu {
                    id: menuInterfaceSettings
                    x: parent.width - this.width
                    y: parent.height + 5
                    width: 150
                    transformOrigin: Menu.TopRight
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                    MenuItem {
                        text: qsTr("MIDI ...")
                        onTriggered: dialogInterfaces.showTab(0)
                    }
                    MenuItem {
                        text: qsTr("DMX ...")
                        onTriggered: dialogInterfaces.showTab(1)
                    }
                    MenuItem {
                        text: qsTr("ArtNet ...")
                        onTriggered: dialogInterfaces.showTab(2)
                    }
                    MenuItem {
                        text: qsTr("Modbus ...")
                        onTriggered: dialogInterfaces.showTab(3)
                    }
                    MenuItem {
                        text: qsTr("Arduino ...")
                        onTriggered: dialogInterfaces.showTab(4)
                    }
                    MenuItem {
                        text: qsTr("Hue ...")
                        onTriggered: dialogInterfaces.showTab(5)
                    }
                    MenuItem {
                        text: qsTr("Screen ...")
                        onTriggered: dialogInterfaces.showTab(6)
                    }
                    MenuItem {
                        text: qsTr("Virtual Pipe ...")
                        onTriggered: dialogInterfaces.showTab(7)
                    }

                }
            }
        }

        Image {
            id: imageLogo
            width: 170
            height: 25
            anchors.verticalCenter: rectTopLeftBar.verticalCenter
            anchors.horizontalCenter: rectTopLeftBar.horizontalCenter
            source: "qrc:///images/logo.png"
            anchors.horizontalCenterOffset: slotListView.listItemCount === 0 ? 0 : rectTopLeftBar.width / 2 + 175
        }

        MwQuickLaunchView {
            id: quickLaunchView
            height: 0
            anchors.top: rectTopLeftBar.bottom
            anchors.left: rectTopLeftBar.left
            anchors.right: rectTopLeftBar.right
        }

        MwSlotListView {
            id: slotListView
            anchors.top: quickLaunchView.bottom
            anchors.left: rectTopLeftBar.left
            anchors.right: rectTopLeftBar.right
            anchors.bottom: parent.bottom

            onSelectedIndexChanged: {
                slotDetailView.slotIndex = slotListView.selectedIndex
                slotDetailView.refresh()
                //console.log("onSelectedIndexChanged", slotListView.selectedIndex)
            }
        }

        MwSlotDetailView {
            id: slotDetailView
            anchors.top: rectTopRightBar.bottom
            anchors.left: rectTopRightBar.left
            anchors.right: rectTopRightBar.right
            anchors.bottom: parent.bottom
            slotIndex: -1

            onSlotDetailUpdated: {
                slotListView.refreshSlot(slotDetailView.slotIndex)
                isModified = true
            }
        }

        Rectangle {
            id: lineSpliter
            width: 1
            anchors.right: rectTopRightBar.left
            anchors.top: rectTopRightBar.bottom
            anchors.bottom: parent.bottom
            color: rectTopRightBar.color
        }
    }

    Rectangle {
        id: busyOverlay
        anchors.fill: parent
        color: "#000000"
        opacity: window.isBusy ? 0.5 : 0
        visible: window.isBusy

        Behavior on opacity { NumberAnimation { duration: 500 } }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.BusyCursor
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: false //window.isBusy
    }

    Popup {
        id: popupAbout

        width: 640
        height: 480
        anchors.centerIn: parent
        modal: false
        dim: true
        focus: true

        onVisibleChanged: {
            textSlogan.visible = false
            textSlogan.reset()
            timerSlogan.stop()
        }

        background: Image {
            anchors.fill: parent
            source: "qrc:///images/background_about.png"
        }

        // @disable-check M16
        Overlay.modeless: Rectangle {
            color: "#55000000"

            MouseArea {
                anchors.fill: parent
                onClicked: popupAbout.close()
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                textSlogan.visible = true
                timerSlogan.start()
            }
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: 60
            anchors.left: parent.left
            anchors.leftMargin: 60
            color: Material.accent
            font.bold: false
            font.pixelSize: 18
            text: digishow.appName() + (digishow.appExperimental() ? " +" : "" )
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: 110
            anchors.left: parent.left
            anchors.leftMargin: 60
            color: "#999999"
            lineHeight: 1.5
            font.pixelSize: 14
            text: qsTr("app version: ") + digishow.appVersion() + "\r\n" +
                  qsTr("app build date: ") + digishow.appBuildDate() + "\r\n\r\n" +
                  qsTr("qt version: ") + digishow.appQtVersion() + "\r\n" +
                  qsTr("rtmidi version: ") + digishow.appRtMidiVersion()
        }

        Text {
            id: textSlogan
            anchors.top: parent.top
            anchors.topMargin: 300
            anchors.left: parent.left
            anchors.leftMargin: 60
            color: "#666666"
            lineHeight: 1.5
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            font.pixelSize: 14
            scale: 1
            rotation: 0
            text: qsTr("Jam with All Things Digital")
            visible: false

            Behavior on color { ColorAnimation { duration: 600 } }
            Behavior on scale { NumberAnimation { duration: 1500; easing.type: Easing.OutCubic } }
            Behavior on rotation { NumberAnimation { duration: 1500; easing.type: Easing.OutCubic } }

            Timer {
                id: timerSlogan

                property int tick: 0

                interval: 1000
                repeat: true
                running: false

                onRunningChanged: tick = 0

                onTriggered: {

                    if (tick < 8) {
                        textSlogan.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.8)
                        textSlogan.scale = Math.random()*5 + 1.0
                        textSlogan.rotation = Math.random()*90 - 45
                        tick++
                    } else {
                        textSlogan.reset()
                        stop()
                    }
                }
            }

            function reset() {
                scale = 1
                rotation = 0
                color = "#666666"
            }
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: 60
            color: "#999999"
            font.pixelSize: 12
            text: qsTr("© 2020 Robin Zhang & Labs")
        }

    }

    FileDialog {
        id: dialogLoadFile
        title: qsTr("Load File")
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ qsTr("DigiShow files (*.dgs)"), qsTr("JSON files (*.json)"), qsTr("All files (*)") ]
        onAccepted: {
            console.log("load file: " + dialogLoadFile.fileUrl)

            if (app.isRunning) app.stop();
            app.loadFile(utilities.urlToPath(dialogLoadFile.fileUrl))
        }
    }

    FileDialog {

        property var callbackAfterSaved: null

        id: dialogSaveFile
        title: qsTr("Save File")
        folder: shortcuts.home
        selectExisting: false
        nameFilters: [ qsTr("DigiShow files (*.dgs)"), qsTr("All files (*)") ]
        onAccepted: {
            console.log("save file: " + dialogSaveFile.fileUrl)
            app.saveFile(utilities.urlToPath(dialogSaveFile.fileUrl),
                         slotListView.getVisualItemsIndexList())
            if (callbackAfterSaved !== null) callbackAfterSaved()
        }
    }

    MwColorPickerDialog {
        property var callbackAfterPicked: null

        id: dialogColorPicker

        onAccepted: {
            if (callbackAfterPicked !== null) callbackAfterPicked()
        }
    }

    MwAppOptionsDialog {
        id: dialogAppOptions
    }

    MwInterfacesDialog {
        id: dialogInterfaces
    }

    CMessageBox {
        id: messageBox
    }

    function createNew() {
        if (!isModified) {
            app.newShow()
        } else {
            var buttonIndex = messageBox.showAndWait(qsTr("Would you like to save all data to a file before create a new ?"),
                                                     qsTr("Save"), qsTr("Don't Save"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: saveAndDo(app.newShow); break
            case 2: app.newShow(); break
            }
        }
    }

    function open() {
        if (!isModified) {
            dialogLoadFile.open()
        } else {
            var buttonIndex = messageBox.showAndWait(qsTr("Would you like to save all data to a file before open another ?"),
                                                     qsTr("Save"), qsTr("Don't Save"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: saveAndDo(dialogLoadFile.open); break
            case 2: dialogLoadFile.open(); break
            }
        }
    }

    function save() {
        saveAndDo(null);
    }

    function saveAs() {
        dialogSaveFile.callbackAfterSaved = null
        dialogSaveFile.open()
    }

    function saveAndDo(callback) {

        if (app.filepath !== "") {
            // save data to the current file
            app.saveFile("", slotListView.getVisualItemsIndexList())
            if (callback !== null) callback()
        } else {
            // save data to a new file
            dialogSaveFile.callbackAfterSaved = callback
            dialogSaveFile.open()
        }
    }

    // the function is prepared for calling by c++
    // when the app main program received a fileopen event
    function fileOpen(filepath) {

        if (messageBox.visible) return

        if (app.isRunning) app.stop();

        if (!isModified) {
            app.loadFile(filepath)
        } else {
            var buttonIndex = messageBox.showAndWait(qsTr("Would you like to save current data before open file %1 ?")
                                                        .arg(utilities.getFileName(filepath)),
                                                     qsTr("Save"), qsTr("Don't Save"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: saveAndDo(function(){ app.loadFile(filepath) }); break
            case 2: app.loadFile(filepath); break
            }
        }
    }

    // the function is prepared for calling by c++
    // when the app main program received an app close event
    function appClose() {

        if (messageBox.visible) return

        if (!isModified) {
            if (messageBox.showAndWait(qsTr("Would you like to quit DigiShow app ?"), qsTr("Quit"), qsTr("Cancel")) === 1) {
                Qt.quit()
            }
        } else {
            var buttonIndex = messageBox.showAndWait(qsTr("Would you like to save all data to a file before close the app ?"),
                                                     qsTr("Save"), qsTr("Don't Save"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: saveAndDo(Qt.quit); break
            case 2: Qt.quit(); break
            }
        }
    }

}
