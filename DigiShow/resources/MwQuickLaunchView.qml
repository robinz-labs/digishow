import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: quickLaunchView

    property string editingLaunchName: "" // name of the launch item that is being edited
    property bool opened: height > 0

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

                anchors.fill: parent
                anchors.margins: 3
                colorNormal: model.color
                colorActivated: "#000000"
                box.border.color: "#ffffff"
                label.font.pixelSize: 12
                label.font.bold: model.assigned
                label.text: model.title
                label.visible: !textLaunchTitle.visible
                opacity: model.assigned || model.name === editingLaunchName ? 1.0 : 0.25

                onLongPressed: {

                    gridView.currentIndex = model.index
                    popupOptions.open()
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
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    CMenuItem {
                        text: model.assigned ? qsTr("Edit") : qsTr("Create")
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
                        text: qsTr("Delete")
                        onTriggered: {
                            menu.close()
                            model.assigned = false
                            app.deleteLaunch(model.name)
                            window.isModified = true
                        }
                    }
                }

            }
        }

        Popup {
            id: popupOptions

            width: 240
            height: 180
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

                    if (slotLaunchOptions.length > 0) {
                        var rememberLink = false
                        var rememberOutput = false
                        for (var n=0 ; n<slotLaunchOptions.length ; n++) {
                            var options = slotLaunchOptions[n]
                            if (options["rememberLink"]) rememberLink = true
                            if (options["rememberOutput"]) rememberOutput = true
                        }
                        checkRememberLink.checked = rememberLink
                        checkRememberOutput.checked = rememberOutput
                    }


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

            /*
            Image {
                width: 16
                height: 16
                anchors.right: parent.left
                anchors.rightMargin: 7
                anchors.top: parent.top
                anchors.topMargin: 30
                opacity: 0.6
                source: "qrc:///images/icon_arrow_left_white.png"
                visible: popupOptions.visible
            }
            */

            Column {
                anchors.fill: parent
                spacing: 10

                Label {
                    topPadding: 5
                    leftPadding: 10
                    font.bold: true
                    text: qsTr("Create A Launch Item Here to :")
                }

                CheckBox {
                    id: checkRememberLink
                    height: 28
                    text: qsTr("Remember Slot Link States")
                    checked: true
                    onClicked: slotListView.setSlotLaunchRememberAllLinks(checked)
                }

                CheckBox {
                    id: checkRememberOutput
                    height: 28
                    text: qsTr("Remember Output Signals")
                    checked: true
                    onClicked: slotListView.setSlotLaunchRememberAllOutputs(checked)
                }

                Item {
                    width: 200
                    height: 40
                    anchors.horizontalCenter: parent.horizontalCenter

                    CButton {
                        width: 95
                        height: 28

                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        label.font.bold: false
                        label.font.pixelSize: 11
                        label.text: qsTr("Save")
                        colorNormal: Material.accent
                        box.radius: 3

                        onClicked: {

                            popupOptions.close()

                            var model = dataModel.get(gridView.currentIndex)
                            model.assigned = true
                            app.updateLaunch(model.name, slotListView.getSlotLaunchOptions())

                            messageBox.show(qsTr("The launch item has been saved, all you just checked will be replayed as soon as you tap the launch button again."), qsTr("OK"))
                            window.isModified = true
                        }
                    }

                    CButton {
                        width: 95
                        height: 28

                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        label.font.bold: false
                        label.font.pixelSize: 11
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

    }

    function open() { height = 300 }

    function close() { if (!popupOptions.visible) height = 0 }

    function refresh() {

        var colorHotPink  = "#ed1b74"
        var colorRosePink = "#f386ad"
        var colorCherry   = "#ee364e"
        var colorSalmon   = "#f7a08d"
        var colorMarigold = "#eae613"
        var colorOrange   = "#ff9900"
        var colorMint     = "#a0d28b"
        var colorLime     = "#00a652"
        var colorRobinEgg = "#18b8b6"
        var colorAqua     = "#44baec"
        var colorOcean    = "#1177dd"
        var colorRoyal    = "#2b459c"
        var colorIris     = "#575aa9"

        // prepare launches data model
        dataModel.clear()
        for (var n=0 ; n<50 ; n++) {

            // create default data
            var name = "launch" + (n+1)
            var row = Math.floor(n / 5)
            var color
            switch (row % 5) {
            case 0: color = colorLime; break
            case 1: color = colorRobinEgg; break
            case 2: color = colorOcean; break
            case 3: color = colorIris; break
            case 4: color = colorHotPink; break
            }
            var title = qsTr("Launch") + " " + (n+1);
            var assigned = false

            // load user data
            var launch = app.getLaunchOptions(name);
            if (Object.keys(launch).length > 0) {

                if (launch["assigned"] !== undefined) assigned = launch["assigned"];
                if (launch["title"]    !== undefined) title    = launch["title"];
                if (launch["color"]    !== undefined) color    = launch["color"];
            }

            // make data model for the grid view
            dataModel.append({
                name: name,
                title: title,
                color: color,
                assigned: assigned
            });
        }

        editingLaunchName = "";
    }
}
