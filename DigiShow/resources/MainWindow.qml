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
    property alias metronome: digishow.metronome

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


    Utilities            { id: utilities          }
    CCommon              { id: common             }
    Digishow             { id: digishow           }
    DigishowData         { id: extractor          }
    MwUndoManager        { id: undoManager        }
    MwRecentFilesManager { id: recentFilesManager }

    Component.onCompleted: {

        // force refresh window size
        width = minimumWidth
        height = minimumHeight

        // get info of online ports
        listOnline = digishow.listOnline()
        utilities.saveJsonToFile(listOnline, digishow.appDataPath("online.json"))

        // init recent files manager
        recentFilesManager.recentFilesListUpdated.connect(function() {
            var n
            for (n=menuRecentFiles.count-2 ; n>=0 ; n--)
                menuRecentFiles.removeItem(menuRecentFiles.itemAt(n))

            var recentFiles = recentFilesManager.recentFilesList;
            for (n=0 ; n<recentFiles.length ; n++) {
                var filepath = recentFiles[n]
                if (utilities.fileExists(filepath)) {
                    var menuItem = Qt.createQmlObject("MwRecentFileMenuItem {}", menuRecentFiles)
                    menuItem.text = utilities.getFileName(filepath)
                    menuItem.filepath = filepath
                    menuRecentFiles.insertItem(menuRecentFiles.count-1, menuItem)
                }
            }
            if (menuRecentFiles.count>1) {
                var menuSeparator = Qt.createQmlObject("MwMenuSeparator {}", menuRecentFiles)
                menuRecentFiles.insertItem(menuRecentFiles.count-1, menuSeparator)
            }
        })
        recentFilesManager.load()

        // callback while file loaded
        app.filepathChanged.connect(function() {
            console.log("file loaded: " + app.filepath)
            //utilities.setMacWindowTitleWithFile(window, app.filepath)

            window.isModified = false
            undoManager.clear()
            undoManager.archive()

            if (app.filepath !== "") {
                recentFilesManager.add(app.filepath)
                recentFilesManager.save()
            }

            quickLaunchView.close()
            metronomeView.close()
        })

        // callback while interfaces data loaded
        app.interfaceListChanged.connect(function() {

            dialogInterfaces.refresh()
        })

        // callback while slots data loaded
        app.slotListChanged.connect(function() {

            slotListView.currentIndex = -1
            slotListView.highlightedIndex = -1
            slotListView.refresh()

            slotDetailView.slotIndex = -1
            slotDetailView.refresh()
        })

        // callback while launch list loaded
        app.launchListChanged.connect(function() {

            quickLaunchView.refresh()
        })

        app.newShow()

        // determine whether the screen is too small to show main window properly
        // show app options dialogbox in which user can set display scale or disable hi-dpi mode
        if (screen.width < minimumWidth || screen.height < minimumHeight) {

            window.showMaximized()
            dialogAppOptions.show()
        }
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

    Shortcut {
        sequence: StandardKey.Copy
        onActivated: slotListView.copySlots()
    }
    Shortcut {
        sequence: StandardKey.Paste
        onActivated: slotListView.pasteSlots()
    }
    Shortcut {
        sequence: StandardKey.SelectAll
        onActivated: slotListView.selectAll()
    }

    Shortcut {
        sequence: StandardKey.Undo
        onActivated: slotListView.undo()
    }
    Shortcut {
        sequence: StandardKey.Redo
        onActivated: slotListView.redo()
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
                        onTriggered: {
                            messageBox.show(qsTr("Starting a new DigiShow LINK instance ..."))
                            utilities.newAppInstance()
                            common.setTimeout(function(){
                                messageBox.close()
                            }, 3000)
                        }
                    }
                    MwMenuSeparator {}
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
                    Menu {
                        id: menuRecentFiles
                        title: qsTr("Open Recent")
                        MenuItem {
                            text: qsTr("Clear Menu")
                            enabled: menuRecentFiles.count > 1
                            onTriggered: {
                                for (var n=menuRecentFiles.count-2 ; n>=0 ; n--)
                                    menuRecentFiles.removeItem(menuRecentFiles.itemAt(n))
                                recentFilesManager.clear()
                                recentFilesManager.save()
                            }
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
                    MwMenuSeparator {}
                    MenuItem {
                        text: qsTr("About DigiShow")
                        onTriggered: dialogAbout.open()
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
                    MwMenuSeparator {}
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
                height: 28
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
                width: 34
                height: 28
                anchors.left: parent.right
                anchors.leftMargin: -305
                anchors.verticalCenter: parent.verticalCenter
                toolTipText: qsTr("Presets")
                toolTipVisible: !quickLaunchView.opened
                icon.width: 24
                icon.height: 24
                icon.sourceSize.width: 48
                icon.sourceSize.height: 48
                icon.source: "qrc:///images/icon_pad_white.png"
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
                id: buttonMetronome
                width: 34
                height: 28
                anchors.left: buttonQuickLaunch.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                toolTipText: qsTr("Metronome")
                toolTipVisible: !metronomeView.opened
                icon.width: 24
                icon.height: 24
                icon.sourceSize.width: 48
                icon.sourceSize.height: 48
                icon.source: "qrc:///images/icon_metro_white.png"
                box.radius: 3
                box.border.width: 1
                colorNormal: metronomeView.opened ? "#666666" : "transparent"
                visible: slotListView.listItemCount > 0
                onClicked: {

                    if (metronomeView.opened)
                        metronomeView.close()
                    else
                        metronomeView.open()
                }
            }

            CButton {
                id: buttonGotoBookmark
                width: 34
                height: 28
                anchors.left: buttonMetronome.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                toolTipText: qsTr("Bookmarks")
                toolTipVisible: !menuGotoBookmark.visible
                icon.width: 24
                icon.height: 24
                icon.sourceSize.width: 48
                icon.sourceSize.height: 48
                icon.source: "qrc:///images/icon_goto_white.png"
                box.radius: 3
                box.border.width: 1
                colorNormal: blinkGotoBookmark.state ? "darkRed" : "transparent"
                visible: slotListView.hasBookmarks
                onVisibleChanged: {
                    if (visible) blinkGotoBookmark.start()
                }
                onClicked: {
                    menuGotoBookmark.optionItems = slotListView.getBookmarks()
                    menuGotoBookmark.showOptions()
                }

                COptionMenu {
                    id: menuGotoBookmark
                    width: 160
                    onOptionClicked: {
                        slotListView.gotoBookmark(value)
                    }
                }

                Timer {
                    property bool state: false
                    property int step: 0

                    id: blinkGotoBookmark
                    interval: 200
                    repeat: true
                    running: false
                    onRunningChanged: {
                        state = false
                        step = 0
                    }
                    onTriggered: {
                        state = !state
                        step++
                        if (step === 8) running = false
                    }
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
                        text: qsTr("OSC ...")
                        onTriggered: dialogInterfaces.showTab(2)
                    }
                    MenuItem {
                        text: qsTr("ArtNet ...")
                        onTriggered: dialogInterfaces.showTab(3)
                    }
                    MenuItem {
                        text: qsTr("Modbus ...")
                        onTriggered: dialogInterfaces.showTab(4)
                    }
                    MenuItem {
                        text: qsTr("Arduino ...")
                        onTriggered: dialogInterfaces.showTab(5)
                    }
                    MenuItem {
                        text: qsTr("Hue ...")
                        onTriggered: dialogInterfaces.showTab(6)
                    }
                    MenuItem {
                        text: qsTr("Screen ...")
                        onTriggered: dialogInterfaces.showTab(7)
                    }
                    MenuItem {
                        text: qsTr("Virtual Pipe ...")
                        onTriggered: dialogInterfaces.showTab(8)
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

        MwMetronomeView {
            id: metronomeView
            height: 0
            anchors.top: rectTopLeftBar.bottom
            anchors.left: rectTopLeftBar.left
            anchors.right: rectTopLeftBar.right
        }

        MwQuickLaunchView {
            id: quickLaunchView
            height: 0
            anchors.top: metronomeView.bottom
            anchors.left: rectTopLeftBar.left
            anchors.right: rectTopLeftBar.right
        }

        MwSlotListView {
            id: slotListView
            anchors.top: quickLaunchView.bottom
            anchors.left: rectTopLeftBar.left
            anchors.right: rectTopLeftBar.right
            anchors.bottom: parent.bottom

            onHighlightedIndexChanged: {
                slotDetailView.slotIndex = slotListView.highlightedIndex
                slotDetailView.refresh()
                //console.log("onHighlightedIndexChanged", slotListView.highlightedIndex)
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

    FileDialog {
        id: dialogLoadFile
        title: qsTr("Open File")
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ qsTr("DigiShow files") + " (*.dgs)", qsTr("JSON files")  + " (*.json)", qsTr("All files") + " (*)" ]
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
        nameFilters: [ qsTr("DigiShow files") + " (*.dgs)", qsTr("All files") + " (*)" ]
        onAccepted: {
            console.log("save file: " + dialogSaveFile.fileUrl)
            app.saveFile(utilities.urlToPath(dialogSaveFile.fileUrl),
                         slotListView.getVisualItemsIndexList())
            if (callbackAfterSaved !== null) callbackAfterSaved()
        }
    }

    MwAboutDialog {
        id: dialogAbout
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
            var buttonIndex = messageBox.showAndWait(qsTr("Do you want to save all data to a file before create a new ?"),
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
            var buttonIndex = messageBox.showAndWait(qsTr("Do you want to save all data to a file before open another ?"),
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

        if (app.isRunning) app.stop()

        if (menu.visible) menu.close()

        if (!isModified) {
            app.loadFile(filepath)
        } else {
            var buttonIndex = messageBox.showAndWait(qsTr("Do you want to save current data before open file %1 ?")
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
            //if (messageBox.showAndWait(qsTr("Are you sure you want to quit DigiShow app ?"), qsTr("Quit"), qsTr("Cancel")) === 1) {
                Qt.quit()
            //}
        } else { 
            window.showNormal()
            var buttonIndex = messageBox.showAndWait(qsTr("Do you want to save all data to a file before close the app ?"),
                                                     qsTr("Save"), qsTr("Don't Save"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: saveAndDo(Qt.quit); break
            case 2: Qt.quit(); break
            }
        }
    }

}
