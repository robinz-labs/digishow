import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtWebEngine 1.8
import DigiShow 1.0

import "components"

Dialog {

    id: dialog

    property string launchName: ""
    property int launchIndex: -1
    property int slotIndex: -1
    property alias preferredColor: timelineView.lineColor
    property alias preferredRange: timelineView.actualValueRangeUpper

    width: parent.width - 100
    height: 440
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    modal: true
    focus: true
    padding: 10
    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        anchors.fill: parent
        color: "#333333"
        radius: 5
    }

    // @disable-check M16
    Overlay.modal: Rectangle {
        color: "#cc000000"
    }

    CButton {
        width: 28
        height: 28
        anchors.top: parent.top
        anchors.topMargin: -30
        anchors.right: parent.right
        anchors.rightMargin: -20
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_close_white.png"
        box.radius: 14
        box.border.width: 2
        box.border.color: "#cccccc"
        colorNormal: "#000000"

        onClicked: {

            if (!timelineView.isModified) {
                close()
                return
            }

            var buttonIndex = messageBox.showAndWait(qsTr("Do you want to save changes to the cue player before closing ?"),
                                                     qsTr("Save Changes"), qsTr("Discard Changes"), qsTr("Cancel"))
            switch (buttonIndex) {
            case 1: save(); close(); break
            case 2: close(); break
            }
        }
    }

    CButton {
        id: buttonPlay
        height: 35
        width: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 0
        anchors.leftMargin: 10
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_play_white.png"
        colorNormal: timelineView.isPlaying ? preferredColor : "transparent"

        onClicked: {
            if (timelineView.isPlaying)
                timelineView.stop()
            else
                timelineView.play()
        }
    }

    CButton {
        id: buttonStop
        height: 35
        width: 40
        anchors.top: parent.top
        anchors.left: buttonPlay.right
        anchors.topMargin: 0
        anchors.leftMargin: 10
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_stop_white.png"
        colorNormal: "transparent"

        onClicked: {
            if (timelineView.isPlaying)
                timelineView.stop()
            else {
                timelineView.viewportPosition = 0
                timelineView.seek(0)
            }
        }
    }

    CButton {
        id: buttonMenu
        height: 35
        width: 40
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 0
        anchors.rightMargin: 10
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_menu_white.png"
        onClicked: {
            if (!menu.visible) {
                menuItemPaste.enabled = utilities.clipboardExists("application/vnd.digishow.cue.timeline")
                menu.open()
            }
            else menu.close()
        }

        Menu {
            id: menu
            x: parent.width - this.width
            y: parent.height + 5
            width: 120
            transformOrigin: Menu.TopRight
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

            MenuItem {
                text: qsTr("Reset")
                onTriggered: reset()
            }
            MwMenuSeparator {}
            MenuItem {
                id: menuItemUndo
                text: qsTr("Undo")
                onTriggered: undo()
            }
            MenuItem {
                id: menuItemCopy
                text: qsTr("Copy")
                onTriggered: copy()
            }
            MenuItem {
                id: menuItemPaste
                text: qsTr("Paste")
                onTriggered: paste()
            }
            MwMenuSeparator {}
            MenuItem {
                text: qsTr("Delete ...")
                onTriggered: {
                    menu.close()
                    if (messageBox.showAndWait(
                         qsTr("Do you want to delete this playback curve from the cue player ?"),
                         qsTr("Delete"), qsTr("Cancel")) === 1) remove()
                }
            }
        }
        Shortcut {
            sequences: [ StandardKey.Undo ]
            enabled: dialog.visible
            onActivated: menuItemUndo.triggered()
        }
        Shortcut {
            sequences: [ StandardKey.Copy ]
            enabled: dialog.visible
            onActivated: menuItemCopy.triggered()
        }
        Shortcut {
            sequences: [ StandardKey.Paste ]
            enabled: dialog.visible
            onActivated: menuItemPaste.triggered()
        }
        Shortcut {
            sequence: "Space"
            enabled: dialog.visible
            onActivated: buttonPlay.clicked()
        }

    }

    CButton {
        id: buttonSave
        height: 35
        width: 80
        anchors.top: parent.top
        anchors.right: buttonMenu.left
        anchors.topMargin: 0
        anchors.rightMargin: 10
        label.text: qsTr("Save")
        label.font.pixelSize: 14
        onClicked: {
            save()
            timelineView.blinking() // indicating that the data has been saved
        }
    }

    Rectangle {

        id: rectCueOptions

        height: 48
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#2d2d2d"
        radius: 6

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5

            Text {
                color: "#666666"
                font.bold: false
                font.pixelSize: 14
                text: qsTr("Cue player attached to: ")
            }

            Text {
                color: "#ffffff"
                font.bold: false
                font.pixelSize: 14
                text: {
                    if (launchIndex === -1) return ""
                    return quickLaunchView.dataModel.get(launchIndex).title
                }
            }

            Item {
                width: 50
                height: 1
            }

            Text {
                color: "#666666"
                font.bold: false
                font.pixelSize: 14
                text: qsTr("Current track output to: ")
            }

            Text {
                color: "#ffffff"
                font.bold: false
                font.pixelSize: 14
                text: {
                    if (slotIndex === -1) return ""
                    var slotTitle = slotListView.dataModel.get(slotIndex).slotTitle
                    if (slotTitle === "") slotTitle = qsTr("Signal Link") + " " + (slotIndex+1)
                    return slotTitle
                }
            }
        }

        CButton {
            id: buttonCueOptions

            width: 130
            height: 28
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            label.font.bold: false
            label.font.pixelSize: 11
            label.text: qsTr("Cue Options ...")
            box.radius: 3

            onClicked: popupCueOptions.open()
        }

        Popup {
            id: popupCueOptions

            width: 280
            height: 250
            x: parent.width - this.width  - 10
            y: parent.height - this.height - 48
            transformOrigin: Popup.BottomRight
            modal: true
            focus: true

            background: Rectangle {
                anchors.fill: parent
                color: "#333333"
                radius: 5
                border.width: 1
                border.color: "#999999"
            }

            // @disable-check M16
            Overlay.modal: Item {}

            Column {
                anchors.fill: parent
                anchors.leftMargin: 16
                spacing: 10

                Text {
                    height: 25
                    verticalAlignment: Text.AlignBottom
                    color: "#cccccc"
                    font.pixelSize: 12
                    font.bold: true
                    text: qsTr("Cue Playback Options")
                }

                Item {
                    width: 5
                    height: 5
                }

                CheckBox {
                    id: checkCueAlone

                    height: 28
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    padding: 0
                    checked: true

                    onClicked: {
                        app.setCuePlayerOption(launchName, "cueAlone", checked)
                        window.isModified = true
                    }

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("Play Alone")
                    }
                }

                CSpinBox {
                    id: spinCueDuration

                    width: 120
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    from: 0
                    to: 99999999
                    value: 0
                    stepSize: 1000
                    unit: qsTr("ms")

                    onValueModified: {
                        app.setCuePlayerOption(launchName, "cueDuration", value)
                        window.isModified = true
                    }

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("Duration")
                    }
                }

                CheckBox {
                    id: checkCueRepeat

                    height: 28
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    padding: 0
                    checked: false

                    onClicked: {
                        app.setCuePlayerOption(launchName, "cueRepeat", checked)
                        window.isModified = true
                    }

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("Repeat")
                    }
                }

                Item {
                    width: 5
                    height: 5
                }

                CButton {
                    width: 100
                    height: 28
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    label.font.bold: false
                    label.font.pixelSize: 11
                    label.text: qsTr("Defaults")
                    box.radius: 3

                    onClicked: {
                        checkCueAlone.checked = true
                        spinCueDuration.value = 0
                        checkCueRepeat.checked = false
                        saveCuePlayerOptions()
                        window.isModified = true
                    }
                }

            }
        }
    }

    TimelineEditor {

        id: timelineView

        property real playheadValue: calculateValueAt(playheadTime)
        property real playheadActualValue: calculateActualValue(playheadValue)
        property bool isModified: false

        height: 300
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: rectCueOptions.top
        anchors.bottomMargin: 10
        backgroundColor: "#2d2d2d"

        onVisibleChanged: {
            if (!visible) timelineView.stop()
        }

        onPlayheadTimeChanged: {
            if (timelineView.isPlaying) {
                let slot = app.slotAt(slotIndex)
                slot.setEndpointOutValue(playheadActualValue)
            }
        }

        onHistoryChanged: {
            if (historyStack.length > 0) timelineView.isModified = true
        }

        function blinking() {
            let t = 0
            for (let n=0 ; n<3 ; n++) {
                common.setTimeout(function(){ timelineView.backgroundColor = "#000000"; timelineView.paint() }, 200*t); t++
                common.setTimeout(function(){ timelineView.backgroundColor = "#2d2d2d"; timelineView.paint() }, 200*t); t++
            }
        }
    }

    Component.onCompleted: {

    }

    function show(launchIndex, slotIndex) {

        dialog.launchIndex = launchIndex
        dialog.slotIndex = slotIndex
        dialog.launchName = quickLaunchView.dataModel.get(launchIndex).name

        // reset timeline view
        timelineView.reset()
        timelineView.historyStack = []

        // load cue player options
        loadCuePlayerOptions()

        dialog.open()

        // load the data into the timeline view
        var cuePlayerDetails = app.slotAt(slotIndex).getSlotOptions()["cuePlayerDetails"]
        if (cuePlayerDetails === undefined || cuePlayerDetails === null) return

        var cuePlayerData = cuePlayerDetails[launchName]
        if (cuePlayerData !== undefined) {
            timelineView.importData(cuePlayerData)
            timelineView.historyStack = []
        }

        timelineView.isModified = false
    }

    function save() {
        var data = timelineView.exportData()

        // save the timeline data
        var cuePlayerDetails = app.slotAt(slotIndex).getSlotOptions()["cuePlayerDetails"]
        if (cuePlayerDetails === undefined || cuePlayerDetails === null) cuePlayerDetails = ({})

        cuePlayerDetails[launchName] = data
        app.slotAt(slotIndex).setSlotOption("cuePlayerDetails", cuePlayerDetails, true)

        // save cue player options
        saveCuePlayerOptions()

        // update the slot list item display
        updateCuePlayerAttachedToSlot(true)
        window.isModified = true
        timelineView.isModified = false
    }

    function reset() {
        timelineView.reset()
    }

    function undo() {
        timelineView.undo()
    }

    function copy() {
        var data = timelineView.exportData()
        utilities.copyJson(data, "application/vnd.digishow.cue.timeline")
    }

    function paste() {
        var data = utilities.pasteJson("application/vnd.digishow.cue.timeline")
        timelineView.importData(data)
    }

    function remove() {
        app.slotAt(slotIndex).clearSlotOption("cuePlayerDetails")

        updateCuePlayerAttachedToSlot(false)
        window.isModified = true

        dialog.close()
    }

    function loadCuePlayerOptions() {
        var options = app.getCuePlayerOptions(launchName)
        var v
        v = options["cueAlone"];    checkCueAlone.checked  = (v === undefined ? true  : v )
        v = options["cueDuration"]; spinCueDuration.value  = (v === undefined ? 0     : v )
        v = options["cueRepeat"];   checkCueRepeat.checked = (v === undefined ? false : v )
    }

    function saveCuePlayerOptions() {
        app.setCuePlayerOption(launchName, "cueAlone"   , checkCueAlone.checked)
        app.setCuePlayerOption(launchName, "cueDuration", spinCueDuration.value)
        app.setCuePlayerOption(launchName, "cueRepeat"  , checkCueRepeat.checked)
    }

    function updateCuePlayerAttachedToSlot(attached) {
        var listOptions = slotListView.getSlotCuePlayerOptions()
        listOptions[slotIndex].attached = attached
        slotListView.setSlotCuePlayerOptions(listOptions)

        if (attached) {
            listOptions = slotListView.getSlotLaunchOptions()
            listOptions[slotIndex].rememberOutput = false
            slotListView.setSlotLaunchOptions(listOptions)
        }
    }
}
