import QtQml.Models 2.1
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: slotListView

    property int   highlightedIndex: -1 // index to data modal
    property int   currentIndex: -1 // index to data modal
    property alias currentIndexVisual: listView.currentIndex // index to visual modal
    property alias listItemCount: dataModel.count

    property bool  showSlotSelection: false
    property bool  hasBookmarks: false
    property bool  shiftKeyHeld: false

    onHighlightedIndexChanged: currentIndex = highlightedIndex
    onCurrentIndexChanged: currentIndexVisual = getVisualItemIndex(currentIndex)
    onCurrentIndexVisualChanged: currentIndex = getDataItemIndex(currentIndexVisual)

    onShowSlotSelectionChanged: if (!showSlotSelection) selectNone()

    Rectangle {

        anchors.fill: parent
        color: "#181818"
        clip: true

        Component {
            id: dragDelegate

            MouseArea {
                id: dragArea

                property bool held: false

                height: 70
                anchors.leftMargin: showSlotSelection ? 32 : 20
                anchors.rightMargin: 20

                drag.target: held ? content : undefined
                drag.axis: Drag.YAxis
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                Component.onCompleted: {
                    anchors.left = parent.left
                    anchors.right = parent.right
                }

                onPressed: held = true
                onReleased: held = false
                onClicked: {

                    if (shiftKeyHeld && highlightedIndex !== -1 && index !== highlightedIndex) {

                        // select multiple
                        selectMultiple(highlightedIndex, index)
                        //highlightedIndex = index

                    } else {

                        // hightlight one
                        highlightedIndex = index
                        listView.forceActiveFocus()

                        if (mouse.button === Qt.RightButton) {
                            menuSlot.x = mouse.x
                            menuSlot.y = mouse.y
                            menuSlot.open()
                        }
                    }
                }
                onPressAndHold: {
                    highlightedIndex = index
                    listView.forceActiveFocus()

                    menuSlot.x = mouse.x
                    menuSlot.y = mouse.y
                    menuSlot.open()
                }

                CMenu {
                    id: menuSlot
                    width: 150
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    CMenuItem {
                        text: qsTr("Rename")
                        onTriggered: {
                            menuSlot.close()
                            labelSlotTitle.showRename()
                            labelSlotTitle.showRename() //?
                        }
                    }
                    CMenuItem {
                        text: !model.slotBookmarked ? qsTr("Bookmark") : qsTr("Remove Bookmark")
                        onTriggered: {
                            bookmarkSlot(currentIndex, !model.slotBookmarked)
                        }
                    }
                    CMenuItem {
                        text: qsTr("Duplicate")
                        onTriggered: {
                            menuSlot.close()
                            duplicateSlots()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Delete")
                        onTriggered: {
                            menuSlot.close()
                            deleteSlots()
                        }
                    }

                    MenuSeparator {}
                    CMenuItem {
                        text: showSlotSelection ? qsTr("Deselect") : qsTr("Select ...")
                        onTriggered: {
                            showSlotSelection = !showSlotSelection
                        }
                    }

                    MenuSeparator {}
                    CMenuItem {
                        text: qsTr("Copy")
                        onTriggered: {
                            menuSlot.close()
                            copySlots()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Paste")
                        onTriggered: {
                            menuSlot.close()
                            pasteSlots()
                        }
                    }

                    MenuSeparator {}
                    CMenuItem {
                        text: qsTr("Undo")
                        enabled: undoManager.canUndo || slotDetailView.isEdited
                        onTriggered: {
                            menuSlot.close()
                            undo()
                        }
                    }
                    CMenuItem {
                        text: qsTr("Redo")
                        enabled: undoManager.canRedo && !slotDetailView.isEdited
                        onTriggered: {
                            menuSlot.close()
                            redo()
                        }
                    }
                }

                Rectangle {
                    id: content
                    anchors.fill: dragArea
                    border.width: 2
                    border.color: highlightedIndex===index ? "#cccccc" : currentIndex===index ? "#666666" : color
                    color: "#222222"
                    radius: 4

                    Drag.active: dragArea.held
                    Drag.source: dragArea
                    Drag.hotSpot.x: width / 2
                    Drag.hotSpot.y: height / 2

                    states: State {
                        when: dragArea.held
                        ParentChange { target: content; parent: slotListView }
                    }

                    Rectangle {
                        id: bookmark
                        width: 8
                        height: 8
                        anchors.right: parent.left
                        anchors.rightMargin: showSlotSelection ? 12 : 5
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: showSlotSelection ? -24 : 0
                        color: "red"
                        radius: 4
                        visible: model.slotBookmarked
                    }

                    Text {
                        id: labelSlotTitle

                        anchors.left: parent.left
                        //anchors.leftMargin: 260
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        anchors.topMargin: 8
                        horizontalAlignment: Text.AlignHCenter

                        color: highlightedIndex===index ? "#cccccc" : "#666666"
                        text: model.slotTitle === undefined || model.slotTitle === "" ?
                                  qsTr("Untitled Link") + " " + (index+1) :
                                  model.slotTitle
                        font.pixelSize: 11
                        font.bold: false
                        visible: !textSlotTitle.visible

                        Behavior on color { ColorAnimation { duration: 500 } }

                        MouseArea {
                            anchors.fill: parent
                            //onDoubleClicked: { labelSlotTitle.showRename() }
                            //onPressAndHold: { labelSlotTitle.showRename() }
                            onClicked: { labelSlotTitle.showRename() }
                        }

                        function showRename() {
                            textSlotTitle.text = model.slotTitle
                            textSlotTitle.visible = true
                            textSlotTitle.selectAll()
                            textSlotTitle.forceActiveFocus()
                        }

                        function doRename() {
                            var slotTitle = textSlotTitle.text.trim()
                            model.slotTitle = slotTitle
                            app.slotAt(index).setSlotOption("title", slotTitle)
                            textSlotTitle.visible = false
                            window.isModified = true
                            undoManager.archive()
                        }
                    }

                    TextInput {
                        id: textSlotTitle
                        anchors.left: labelSlotTitle.left
                        anchors.right: labelSlotTitle.right
                        anchors.verticalCenter: labelSlotTitle.verticalCenter
                        horizontalAlignment: Text.AlignHCenter

                        font.pixelSize: 11
                        font.bold: true
                        color: "#eeeeee"
                        selectionColor: "#666666"
                        selectedTextColor: "#ffffff"
                        selectByMouse: true
                        focus: true
                        visible: false

                        onEditingFinished: {
                            if (textSlotTitle.visible) labelSlotTitle.doRename()
                        }
                        onFocusChanged: { textSlotTitle.visible = false }
                        Keys.priority: Keys.AfterItem
                        Keys.onPressed: { if (event.key === Qt.Key_Escape) textSlotTitle.visible = false; event.accepted = true }
                        Keys.onReleased: { event.accepted = true }
                    }

                    Rectangle {
                        id: rectEndpointIn

                        width: 60
                        height: 60
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 5
                        radius: 4
                        color: model.epInColor
                        opacity: 0.6

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 5
                            color: "#ffffff"
                            text: model.epInLabelEPT
                            font.pixelSize: 9
                            font.bold: false
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 20
                            color: "#ffffff"
                            text: model.epInLabelEPI
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Image {
                            width: 48
                            height: 24
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 3
                            source: model.epInIcon
                        }
                    }

                    Rectangle {
                        id: meterEndpointIn

                        visible: model.epInSignal !==0

                        width: 106
                        height: 12
                        anchors.left: rectEndpointIn.right
                        anchors.leftMargin: 20
                        anchors.verticalCenter: rectEndpointIn.verticalCenter
                        radius: 6
                        color: "#333333"

                        Rectangle {
                            property real value: model.epInValue / model.epInRange

                            anchors.fill: parent
                            anchors.leftMargin: 3 + (model.slotInInverted ? Math.round(100 * value) : 0)
                            anchors.rightMargin: 3 + (model.slotInInverted ? 0 : Math.round(100 * (1.0-value)))
                            anchors.topMargin: 3
                            anchors.bottomMargin: 3
                            radius: 3
                            color: model.epInColor

                            Behavior on value { NumberAnimation { duration: 200; easing.type: Easing.OutExpo } }
                        }

                        Text {
                            anchors.top: parent.bottom
                            anchors.topMargin: 6
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            color: "#dddddd"
                            text: "IN"
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Image {
                            width: 16
                            height: 16
                            anchors.top: parent.bottom
                            anchors.topMargin: 4
                            anchors.right: parent.right
                            anchors.rightMargin: 2
                            opacity: 0.5
                            source: "qrc:///images/icon_invert_white.png"
                            visible: model.slotInInverted
                        }

                        Text {
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            color: "#dddddd"
                            font.pixelSize: 9
                            font.bold: false

                            text: model.epInSignal === DigishowEnvironment.SignalBinary ?
                                    (model.epInValue ? "on" : "off") :
                                    Math.round(100 * model.epInValue / model.epInRange) + "%"
                        }

                        Text {
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            anchors.right: parent.right
                            anchors.rightMargin: 4
                            color: "#dddddd"
                            text: model.epInAvailable ? model.epInValue.toString() : "- -"
                            font.pixelSize: 9
                            font.bold: false
                        }
                    }

                    CButton {
                        id: buttonLink
                        width: 34
                        height: 23
                        anchors.left: parent.left
                        anchors.leftMargin: 206
                        anchors.verticalCenter: parent.verticalCenter
                        label.text: "LINK"
                        label.font.bold: false
                        label.font.pixelSize: 9
                        box.radius: 3
                        box.border.width: mouseOver || !model.slotLinked ? 1 : 0
                        colorNormal: model.slotLinked ? "#666666" : "transparent"
                        onClicked: {

                            model.slotLinked = !model.slotLinked
                            app.slotAt(index).setLinked(model.slotLinked)

                            if (isBlankSlot(index)) // the blank slot is regarded as a group header
                                setGroupLinked(index, model.slotLinked)
                        }
                    }

                    Image {
                        id: indicatorLink
                        width: 16
                        height: 16
                        anchors.left: buttonLink.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:///images/icon_arrow_right_white.png"
                        opacity: model.epInBusy ? 1.0 : 0.1
                        visible: model.slotLinked && !model.errTraffic
                    }

                    Image {
                        id: indicatorTraffic
                        width: 16
                        height: 16
                        anchors.left: buttonLink.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:///images/icon_arrow_right_red.png"
                        visible: model.errTraffic && model.epInBusy
                    }

                    Image {
                        id: indicatorInputExpression
                        width: 16
                        height: 16
                        anchors.horizontalCenter: meterEndpointIn.horizontalCenter
                        anchors.top: meterEndpointIn.bottom
                        anchors.topMargin: 6
                        opacity: 0.9
                        source: "qrc:///images/icon_expression_white.png"
                        visible: model.slotInExpression !== "" && !model.errInExp
                    }

                    Image {
                        id: indicatorInputExpressionError
                        width: 16
                        height: 16
                        anchors.horizontalCenter: meterEndpointIn.horizontalCenter
                        anchors.top: meterEndpointIn.bottom
                        anchors.topMargin: 4
                        source: "qrc:///images/icon_attention.png"
                        visible: model.errInExp
                    }

                    Rectangle {
                        id: rectEndpointOut

                        width: 60
                        height: 60
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 5
                        radius: 4
                        color: model.epOutColor
                        opacity: 0.6

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 5
                            color: "#ffffff"
                            text: model.epOutLabelEPT
                            font.pixelSize: 9
                            font.bold: false
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 20
                            color: "#ffffff"
                            text: model.epOutLabelEPI
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Image {
                            width: 48
                            height: 24
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 3
                            source: model.epOutIcon
                        }
                    }

                    Rectangle {
                        id: meterEndpointOut

                        visible: model.epOutSignal !==0

                        width: 106
                        height: 12
                        anchors.right: rectEndpointOut.left
                        anchors.rightMargin: 20
                        anchors.verticalCenter: rectEndpointOut.verticalCenter
                        radius: 6
                        color: "#333333"

                        Rectangle {

                            property real value: model.epOutValue / model.epOutRange

                            width: Math.round(100 * value)
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.margins: 3
                            radius: 3
                            color: model.epOutColor

                            Behavior on value { NumberAnimation { duration: 200; easing.type: Easing.OutExpo } }
                        }

                        Text {
                            anchors.top: parent.bottom
                            anchors.topMargin: 6
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            color: "#dddddd"
                            text: "OUT"
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Text {
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            color: "#dddddd"
                            font.pixelSize: 9
                            font.bold: false

                            text: model.epOutSignal === DigishowEnvironment.SignalBinary ?
                                      (model.epOutValue ? "on" : "off") :
                                      Math.round(100 * model.epOutValue / model.epOutRange) + "%"
                        }

                        Text {
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            anchors.right: parent.right
                            anchors.rightMargin: 4
                            color: "#dddddd"
                            text: model.epOutAvailable ? model.epOutValue.toString() : "- -"
                            font.pixelSize: 9
                            font.bold: false
                        }

                        CButton {
                            id: buttonTap
                            width: 34
                            height: 23
                            anchors.right: parent.left
                            anchors.rightMargin: 16
                            anchors.verticalCenter: parent.verticalCenter
                            label.text: "TAP"
                            label.font.bold: false
                            label.font.pixelSize: 9
                            box.radius: 3
                            box.border.width: 1
                            colorNormal: "transparent"
                            mouseDown: model.epOutTap
                            onPressed: {
                                model.epOutFaderHold = false

                                if (model.slotOutInverted === false && model.epOutFaderValue === 0)
                                    model.epOutFaderValue = model.epOutRange
                                if (model.slotOutInverted === true && model.epOutFaderValue === model.epOutRange)
                                    model.epOutFaderValue = 0

                                model.epOutTap = true
                                app.slotAt(index).setEndpointOutValue(faderOutput.value)
                            }
                            onReleased: {
                                model.epOutTap = false
                                app.slotAt(index).setEndpointOutValue(model.slotOutInverted ? model.epOutRange : 0)
                            }
                        }

                        CButton {
                            id: buttonHold
                            width: 34
                            height: 23
                            anchors.right: buttonTap.left
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            label.text: "HOLD"
                            label.font.bold: false
                            label.font.pixelSize: 9
                            box.radius: 3
                            box.border.width: mouseOver || !model.epOutFaderHold ? 1 : 0
                            colorNormal: model.epOutFaderHold ? "#666666" : "transparent"
                            onClicked: {
                                if (!model.epOutFaderHold) {
                                    model.epOutPreValue = faderOutput.value
                                    app.slotAt(index).setEndpointOutValue(faderOutput.value)
                                }
                                model.epOutFaderHold = !model.epOutFaderHold
                            }
                        }

                        CSlider {
                            id: faderOutput

                            width: rectTopLeftBar.width - 710 + 90
                            height:30
                            anchors.right: buttonHold.left
                            anchors.rightMargin: 18
                            anchors.verticalCenter: parent.verticalCenter
                            value: model.slotLinked && model.epOutFaderHold ? model.epOutPreValue : model.epOutFaderValue
                            to: model.epOutRange
                            stepSize: 1
                            color: model.epOutColor
                            inverted: model.slotOutInverted

                            Behavior on value { NumberAnimation { duration: (model.slotLinked && model.epOutFaderHold ? 300 : 0); easing.type: Easing.OutCubic } }

                            onMoved: {
                                if (model.epOutFaderHold) app.slotAt(index).setEndpointOutValue(faderOutput.value)
                            }

                            onValueChanged: {
                                model.epOutFaderValue = faderOutput.value
                            }

                            Image {
                                width: 16
                                height: 16
                                anchors.top: parent.bottom
                                anchors.topMargin: -5
                                anchors.right: parent.right
                                anchors.rightMargin: 0
                                opacity: 0.5
                                source: "qrc:///images/icon_invert_white.png"
                                visible: model.slotOutInverted
                            }
                        }
                    }

                    Image {
                        id: indicatorOutputExpression
                        width: 16
                        height: 16
                        anchors.horizontalCenter: meterEndpointOut.horizontalCenter
                        anchors.top: meterEndpointOut.bottom
                        anchors.topMargin: 6
                        opacity: 0.9
                        source: "qrc:///images/icon_expression_white.png"
                        visible: model.slotOutExpression !== "" && !model.errOutExp
                    }

                    Image {
                        id: indicatorOutputExpressionError
                        width: 16
                        height: 16
                        anchors.horizontalCenter: meterEndpointOut.horizontalCenter
                        anchors.top: meterEndpointOut.bottom
                        anchors.topMargin: 4
                        source: "qrc:///images/icon_attention.png"
                        visible: model.errOutExp
                    }

                    CheckBox {
                        id: checkLaunchRememberLink
                        anchors.horizontalCenter: buttonLink.horizontalCenter
                        anchors.verticalCenter: labelSlotTitle.verticalCenter
                        visible: quickLaunchView.visible && quickLaunchView.editingLaunchName !== ""
                        checked: model.launchRememberLink
                        onClicked: model.launchRememberLink = checked
                    }

                    CheckBox {
                        id: checkLaunchRememberOutput
                        anchors.horizontalCenter: rectEndpointOut.horizontalCenter
                        anchors.verticalCenter: labelSlotTitle.verticalCenter
                        visible: quickLaunchView.visible && quickLaunchView.editingLaunchName !== ""
                        checked: model.launchRememberOutput
                        onClicked: model.launchRememberOutput = checked
                    }

                    CheckBox {
                        id: checkSlotSelected
                        anchors.right: rectEndpointIn.left
                        anchors.rightMargin: 0
                        anchors.verticalCenter: rectEndpointIn.verticalCenter
                        visible: showSlotSelection
                        checked: model.slotSelected
                        onClicked: {
                            if (shiftKeyHeld && highlightedIndex !== -1 && index !== highlightedIndex) {
                                // select multiple
                                selectMultiple(highlightedIndex, index, checked)
                            } else {
                                // select one
                                model.slotSelected = checked
                                app.slotAt(index).setSelected(checked)
                            }
                        }
                    }
                }

                DropArea {
                    anchors.fill: parent
                    onEntered: {

                        if (drag.formats.length>0) {
                            drag.accepted = false
                            return
                        }

                        visualModel.items.move(
                                drag.source.DelegateModel.itemsIndex,
                                dragArea.DelegateModel.itemsIndex)
                        currentIndex = index
                    }
                }
            }
        }

        DelegateModel {
            id: visualModel
            model: dataModel
            delegate: dragDelegate
        }

        ListModel {
            id: dataModel
        }

        ListView {
            id: listView
            model: visualModel
            anchors.fill: parent
            anchors.topMargin: 20
            anchors.bottomMargin: 30
            spacing: 30
            cacheBuffer: 50

            snapMode: ListView.SnapToItem
            focus: true

            highlightFollowsCurrentItem: true
            highlightMoveVelocity: -1
            highlightMoveDuration: 200

            ScrollBar.vertical: ScrollBar {
                width: 6
                topPadding: -20
                bottomPadding: -30
                rightPadding: 0
            }

            Keys.onReleased: {

                if (event.key === Qt.Key_Shift) shiftKeyHeld = false

                // slot actions
                if (highlightedIndex !== -1) {

                    var slot = app.slotAt(highlightedIndex)
                    var model = dataModel.get(highlightedIndex)

                    if (event.key === Qt.Key_T) {

                        // tap
                        dataModel.setProperty(highlightedIndex, "epOutTap", false)
                        slot.setEndpointOutValue(model.slotOutInverted ? model.epOutRange : 0)

                        event.accepted = true
                        return
                    }
                }
            }

            Keys.onPressed: {

                if (event.key === Qt.Key_Shift) shiftKeyHeld = true

                // slot actions
                if (highlightedIndex !== -1) {

                    var slot = app.slotAt(highlightedIndex)
                    var model = dataModel.get(highlightedIndex)

                    if (event.key === Qt.Key_L) {

                        // link
                        slot.setLinked(!model.slotLinked)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_H) {

                        // hold
                        if (!model.epOutFaderHold) {
                            dataModel.setProperty(highlightedIndex, "epOutPreValue", model.epOutFaderValue)
                            slot.setEndpointOutValue(model.epOutFaderValue)
                        }
                        dataModel.setProperty(highlightedIndex, "epOutFaderHold", !model.epOutFaderHold)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_T) {

                        // tap
                        dataModel.setProperty(highlightedIndex, "epOutFaderHold", false)

                        if (model.slotOutInverted === false && model.epOutFaderValue === 0)
                            dataModel.setProperty(highlightedIndex, "epOutFaderValue", model.epOutRange)
                        if (model.slotOutInverted === true && model.epOutFaderValue === model.epOutRange)
                            dataModel.setProperty(highlightedIndex, "epOutFaderValue", 0)

                        dataModel.setProperty(highlightedIndex, "epOutTap", true)
                        slot.setEndpointOutValue(model.epOutFaderValue)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_Left || event.key === Qt.Key_Right) {

                        // fader - or fader +
                        var sign = (event.key === Qt.Key_Left ? -1 : 1)
                        var epOutFaderValue = (model.epOutSignal === DigishowEnvironment.SignalBinary
                                               ? model.epOutFaderValue + sign * 1
                                               : model.epOutFaderValue + sign * model.epOutRange * 0.1)
                        epOutFaderValue = Math.min(epOutFaderValue, model.epOutRange)
                        epOutFaderValue = Math.max(epOutFaderValue, 0)
                        epOutFaderValue = Math.round(epOutFaderValue)
                        dataModel.setProperty(highlightedIndex, "epOutFaderValue", epOutFaderValue)
                        if (model.epOutFaderHold) slot.setEndpointOutValue(epOutFaderValue)

                        event.accepted = true
                        return
                    }
                }

                // change current slot item
                var indexVisual = currentIndexVisual

                if (indexVisual !== -1) {
                    if (event.key === Qt.Key_Up || event.key === Qt.Key_PageUp) {

                        // move up
                        indexVisual -=  (event.key === Qt.Key_PageUp ? Math.floor(listView.height / 100) : 1)
                        currentIndexVisual = Math.max(indexVisual, 0)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_Down || event.key === Qt.Key_PageDown) {

                        // move down
                        indexVisual += (event.key === Qt.Key_PageDown ? Math.floor(listView.height / 100) : 1)
                        currentIndexVisual = Math.min(indexVisual, visualModel.items.count-1)

                        event.accepted = true
                        return
                    }
                }


                // change highlighted slot item
                if (event.key === Qt.Key_Space || event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {

                    highlightedIndex = slotListView.currentIndex

                    event.accepted = true
                    return
                }

                // delete the slot
                if (event.key === Qt.Key_Delete) {

                    deleteSlots()

                    event.accepted = true
                    return
                }
            }
        }

        Item {
            id: slotSelectionView
            height: 46
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: showSlotSelection ? 0 : -height
            clip: true

            Behavior on anchors.bottomMargin { NumberAnimation { duration: 120 } }

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.5
            }

            CButton {
                id: buttonSelectAll
                width: 80
                height: 26
                anchors.left: parent.left
                anchors.leftMargin: 36
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Select All")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    selectAll()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                id: buttonSelectNone
                width: 80
                height: 26
                anchors.left: buttonSelectAll.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Select None")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    selectNone()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                id: buttonCopySelection
                width: 80
                height: 26
                anchors.left: buttonSelectNone.right
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Copy")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    copySlots()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                id: buttonDuplicateSelection
                width: 80
                height: 26
                anchors.left: buttonCopySelection.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Duplicate")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    duplicateSelection()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                id: buttonDeleteSelection
                width: 80
                height: 26
                anchors.left: buttonDuplicateSelection.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Delete")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    deleteSelection()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                id: buttonMoveSelection
                width: 80
                height: 26
                anchors.left: buttonDeleteSelection.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                label.text: qsTr("Move")
                label.font.bold: false
                label.font.pixelSize: 11
                box.radius: 3
                box.border.width: 1
                colorNormal: "black"

                onClicked: {
                    moveSelection()
                    listView.forceActiveFocus()
                }
            }

            CButton {
                width: 20
                height: 20
                anchors.right: parent.right
                anchors.rightMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 20
                icon.height: 20
                icon.source: "qrc:///images/icon_close_black.png"
                box.radius: 10
                colorNormal: "white"

                onClicked: {
                    showSlotSelection = false
                    listView.forceActiveFocus()
                }
            }

        }
    }

    Rectangle {

        anchors.fill: parent
        color: "#111111"
        visible: dataModel.count===0

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#555555"
            text: qsTr("Please tap button + to add a new \r\n signal link between your digital things")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            font.bold: true
            lineHeight: 2.0

            Rectangle {
                width: 90
                height: 30
                anchors.top: parent.bottom
                anchors.topMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 15
                color: "#333333"
                opacity: 0.5

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        parent.color = Material.accent
                        buttonNewSlot.colorNormal = Material.accent
                    }
                    onReleased: {
                        parent.color = "#333333"
                        buttonNewSlot.colorNormal = "#484848"
                    }
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "#999999"
                    text: qsTr("STEP 2")
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }
    }

    Timer {
        id: dataUpdater

        interval: 20
        repeat: true
        running: true //app.isRunning
        onTriggered: {

            // update signal values periodically

            var slotCount = app.slotCount()
            for (var n=0 ; n<slotCount ; n++) {
                var data = digishow.getSlotRuntimeData(n)

                if (dataModel.get(n).epInAvailable  !== data["epInAvailable" ]) dataModel.setProperty(n, "epInAvailable",  data["epInAvailable" ])
                if (dataModel.get(n).epOutAvailable !== data["epOutAvailable"]) dataModel.setProperty(n, "epOutAvailable", data["epOutAvailable"])
                if (dataModel.get(n).epInValue      !== data["epInValue"     ]) dataModel.setProperty(n, "epInValue",      data["epInValue"     ])
                if (dataModel.get(n).epOutValue     !== data["epOutValue"    ]) dataModel.setProperty(n, "epOutValue",     data["epOutValue"    ])
                if (dataModel.get(n).epOutPreValue  !== data["epOutPreValue" ]) dataModel.setProperty(n, "epOutPreValue",  data["epOutPreValue" ])
                if (dataModel.get(n).epInBusy       !== data["epInBusy"      ]) dataModel.setProperty(n, "epInBusy",       data["epInBusy"      ])
                if (dataModel.get(n).epOutBusy      !== data["epOutBusy"     ]) dataModel.setProperty(n, "epOutBusy",      data["epOutBusy"     ])
                if (dataModel.get(n).slotEnabled    !== data["slotEnabled"   ]) dataModel.setProperty(n, "slotEnabled",    data["slotEnabled"   ])
                if (dataModel.get(n).slotLinked     !== data["slotLinked"    ]) dataModel.setProperty(n, "slotLinked",     data["slotLinked"    ])
                if (dataModel.get(n).errTraffic     !== data["errTraffic"    ]) dataModel.setProperty(n, "errTraffic",     data["errTraffic"    ])
                if (dataModel.get(n).errInExp       !== data["errInExp"      ]) dataModel.setProperty(n, "errInExp",       data["errInExp"      ])
                if (dataModel.get(n).errOutExp      !== data["errOutExp"     ]) dataModel.setProperty(n, "errOutExp",      data["errOutExp"     ])
            }
        }
    }

    function refresh() {

        dataModel.clear()
        hasBookmarks = false

        var slotCount = app.slotCount();
        for (var n=0 ; n<slotCount ; n++) refreshSlot(n)

        currentIndex = -1
        highlightedIndex = -1
    }

    function refreshSlot(slotIndex) {

        var n = slotIndex
        var config = digishow.getSlotConfiguration(n)

        // obtain slot data
        var slotOptions = config["slotOptions"]
        var slotTitle = slotOptions["title"]
        if (slotTitle === undefined) slotTitle = ""

        var slotBookmarked = slotOptions["bookmarked"]
        if (slotBookmarked === undefined) slotBookmarked = false
        if (slotBookmarked) hasBookmarks = true

        var slotInfo = config["slotInfo"]
        var slotInExpression = slotInfo["inputExpression"]
        var slotOutExpression = slotInfo["outputExpression"]
        var slotInInverted = slotInfo["inputInverted"]
        var slotOutInverted = slotInfo["outputInverted"]

        // obtain endpoint data
        var epInSignal = 0
        var epInType = 0
        var epInColor = "#666666"
        var epInIcon = "qrc:///images/icon_ep_in_white.png"
        var epInLabelEPT = qsTr("Source")
        var epInLabelEPI = qsTr("no input")
        var epInRange = 1

        var epOutSignal = 0
        var epOutType = 0
        var epOutColor = "#666666"
        var epOutIcon = "qrc:///images/icon_ep_out_white.png"
        var epOutLabelEPT = qsTr("Destination")
        var epOutLabelEPI = qsTr("no output")
        var epOutRange = 1

        var epInInfo = config["epInInfo"]
        if (epInInfo!== undefined) {
            epInSignal = epInInfo["signal"]
            epInType = epInInfo["type"]
            epInColor = digishow.getEndpointColor(epInType, epInSignal)
            epInIcon = digishow.getSignalIcon(epInSignal)
            epInLabelEPT = epInInfo["labelEPT"]
            epInLabelEPI = epInInfo["labelEPI"]

            if (epInInfo["range"]>0) epInRange = epInInfo["range"]
        }

        var epOutInfo = config["epOutInfo"]
        if (epOutInfo !== undefined) {
            epOutSignal = epOutInfo["signal"]
            epOutType = epOutInfo["type"]
            epOutColor = digishow.getEndpointColor(epOutType, epOutSignal)
            epOutIcon = digishow.getSignalIcon(epOutSignal)
            epOutLabelEPT = epOutInfo["labelEPT"]
            epOutLabelEPI = epOutInfo["labelEPI"]

            if (epOutInfo["range"]>0) epOutRange = epOutInfo["range"]
        }

        // obtain runtime date
        var data = digishow.getSlotRuntimeData(n)

        var epInAvailable  = data["epInAvailable"]
        var epOutAvailable = data["epOutAvailable"]
        var epInValue      = data["epInValue"]
        var epOutValue     = data["epOutValue"]
        var epOutPreValue  = data["epOutPreValue"]
        var epInBusy       = data["epInBusy"]
        var epOutBusy      = data["epOutBusy"]
        var slotEnabled    = data["slotEnabled"]
        var slotLinked     = data["slotLinked"]
        var errTraffic     = data["errTraffic"]
        var errInExp       = data["errInExp"]
        var errOutExp      = data["errOutExp"]


        // add to data model
        var item = {

            //itemIndex: n,

            slotTitle: slotTitle,

            slotBookmarked: slotBookmarked,
            slotEnabled: slotEnabled,
            slotLinked: slotLinked,
            slotSelected: false,

            slotInExpression: slotInExpression,
            slotOutExpression: slotOutExpression,
            slotInInverted: slotInInverted,
            slotOutInverted: slotOutInverted,

            epInSignal: epInSignal,
            epInColor: epInColor,
            epInIcon: epInIcon,
            epInLabelEPT: epInLabelEPT,
            epInLabelEPI: epInLabelEPI,

            epInRange: epInRange,
            epInValue: epInValue,
            epInBusy: epInBusy,
            epInAvailable: epInAvailable,

            epOutSignal: epOutSignal,
            epOutColor: epOutColor,
            epOutIcon: epOutIcon,
            epOutLabelEPT: epOutLabelEPT,
            epOutLabelEPI: epOutLabelEPI,

            epOutRange: epOutRange,
            epOutValue: epOutValue,
            epOutPreValue: epOutPreValue,
            epOutBusy: epOutBusy,
            epOutAvailable: epOutAvailable,

            epOutFaderValue: 0,
            epOutFaderHold: true,
            epOutTap: false,

            errTraffic: errTraffic,
            errInExp: errInExp,
            errOutExp: errOutExp,

            launchRememberLink: false,
            launchRememberOutput: false
        }

        dataModel.set(n, item)
    }

    function bookmarkSlot(slotIndex, bookmarked) {

        dataModel.setProperty(slotIndex, "slotBookmarked", bookmarked)
        app.slotAt(slotIndex).setSlotOption("bookmarked", bookmarked)

        hasBookmarks = false
        for (var n=0 ; n<dataModel.count ; n++) {
            if (dataModel.get(n)["slotBookmarked"]) {
                hasBookmarks = true
                break
            }
        }

        window.isModified = true
        //undoManager.archive()
    }

    function duplicateSlots() {

        if (slotListView.showSlotSelection)
            duplicateSelection()
        else
            duplicateSlot(slotListView.currentIndex)
    }

    function duplicateSlot(slotIndex) {

        var newSlotIndex = app.duplicateSlot(slotIndex)
        refreshSlot(newSlotIndex)
        visualModel.items.move(
                    getVisualItemIndex(newSlotIndex),
                    currentIndexVisual+1)
        currentIndex = newSlotIndex
        highlightedIndex = newSlotIndex

        window.isModified = true
        undoManager.archive()
    }

    function duplicateSelection() {

        if (messageBox.showAndWait(qsTr("Do you want to duplicate all selected links ?"), qsTr("Duplicate"), qsTr("Cancel")) !== 1) return

        var numAll = visualModel.items.count
        var n, i
        for (n=0 ; n<numAll ; n++) {
            i = getDataItemIndex(n)
            if (dataModel.get(i).slotSelected === true) currentIndex = i
        }
        for (n=0 ; n<numAll ; n++) {
            i = getDataItemIndex(n)
            if (dataModel.get(i).slotSelected === true) duplicateSlot(i)
        }
    }

    function deleteSlots() {

        if (slotListView.showSlotSelection)
            deleteSelection()
        else
            deleteSlot(slotListView.currentIndex)
    }

    function deleteSlot(slotIndex, showMessageToConfirm) {

        if (showMessageToConfirm === undefined) showMessageToConfirm = true

        var deletedIndex = slotIndex
        if (deletedIndex !== -1) {

            highlightedIndex = deletedIndex

            if (!showMessageToConfirm ||
                messageBox.showAndWait(qsTr("Do you want to delete the link ?"), qsTr("Delete"), qsTr("Cancel")) === 1) {

                highlightedIndex = -1
                currentIndex = -1

                var indexVisaul = getVisualItemIndex(deletedIndex)

                app.deleteSlot(deletedIndex)
                dataModel.remove(deletedIndex) //refresh()

                if (indexVisaul >= 0 &&
                    indexVisaul < visualModel.items.count) {

                    currentIndexVisual = indexVisaul
                }

                window.isModified = true
                undoManager.archive()
            }

            listView.forceActiveFocus()
        }
    }

    function deleteSelection() {

        if (messageBox.showAndWait(qsTr("Do you want to delete all selected links ?"), qsTr("Delete"), qsTr("Cancel")) !== 1) return

        for (var n=dataModel.count-1 ; n>=0 ; n--)
            if (dataModel.get(n).slotSelected === true) deleteSlot(n, false)
    }

    function moveSelection() {

        if (currentIndexVisual === -1) return
        if (messageBox.showAndWait(qsTr("Do you want to move all selected links to the current cursor position ?"), qsTr("Move"), qsTr("Cancel")) !== 1) return

        var numAll = visualModel.items.count
        var numMoved = 0
        for (var n=numAll-1 ; n>=0 ; n--) {
            if (visualModel.items.get(n).model.slotSelected === true) {
                numMoved++
                visualModel.items.move(n, numAll-numMoved)
                if (n<=currentIndexVisual) currentIndexVisual--
            }
        }
        visualModel.items.move(numAll-numMoved, currentIndexVisual+1, numMoved)
    }

    function selectAll() {

        if (dataModel.count === 0) return

        for (var n=0 ; n<dataModel.count ; n++) {
            dataModel.setProperty(n, "slotSelected",  true)
            app.slotAt(n).setSelected(true)
        }

        showSlotSelection = true
    }

    function selectNone() {
        for (var n=0 ; n<dataModel.count ; n++) {
            dataModel.setProperty(n, "slotSelected",  false)
            app.slotAt(n).setSelected(false)
        }
    }

    function selectMultiple(slotIndex1, slotIndex2, slotSelected) {

        if (slotSelected === undefined) slotSelected = true

        var iv1 = getVisualItemIndex(slotIndex1)
        var iv2 = getVisualItemIndex(slotIndex2)
        if (iv1 > iv2) { var iv0 = iv1; iv1 = iv2; iv2 = iv0 }
        for (var iv = iv1 ; iv <= iv2 ; iv++) {
            var i = getDataItemIndex(iv)
            dataModel.setProperty(i, "slotSelected", slotSelected)
            app.slotAt(i).setSelected(slotSelected)
        }

        showSlotSelection = true
    }

    function copySlots() {

        // no slot selection, instead select the current slot to copy
        var needCopyCurrentSlot = (!slotListView.showSlotSelection && currentIndex !== -1)
        if (needCopyCurrentSlot) {
            selectNone()
            showSlotSelection = true
            dataModel.setProperty(currentIndex, "slotSelected", true)
            app.slotAt(currentIndex).setSelected(true)
        }

        var data = app.exportData(getVisualItemsIndexList(), true)
        utilities.copyJson(data, "application/vnd.digishow.data")

        if (needCopyCurrentSlot) showSlotSelection = false
    }

    function pasteSlots() {
        var data = utilities.pasteJson("application/vnd.digishow.data")
        importData(data)
    }

    function importData(data) {

        // must stop app running first
        app.stop()

        // extracte imported data
        // to make slots with dependent interfaces and endpoints
        extractor.setData(data)
        var newSlotCount = extractor.slotCount();
        for (var n=0 ; n<newSlotCount ; n++) {

            var newSlotOptions = extractor.getSlotOptions(n)

            var newSourceInterfaceOptions = {}
            var newSourceEndpointOptions = {}
            var newDestinationInterfaceOptions = {}
            var newDestinationEndpointOptions = {}
            var newMediaOptions = {}

            if (newSlotOptions["source"] !== undefined) {
                var newSource = newSlotOptions["source"].split("/")
                newSourceInterfaceOptions = extractor.getInterfaceOptions(newSource[0])
                newSourceEndpointOptions = extractor.getEndpointOptions(newSource[0], newSource[1])
            }

            if (newSlotOptions["destination"] !== undefined) {
                var newDestination = newSlotOptions["destination"].split("/")
                newDestinationInterfaceOptions = extractor.getInterfaceOptions(newDestination[0])
                newDestinationEndpointOptions = extractor.getEndpointOptions(newDestination[0], newDestination[1])

                var newMedia = newDestinationEndpointOptions["media"]
                newMediaOptions = extractor.getMediaOptions(newDestination[0], newMedia)
            }

            var newSlotIndex = digishow.makeSlot(
                        newSlotOptions,
                        newSourceInterfaceOptions, newSourceEndpointOptions,
                        newDestinationInterfaceOptions, newDestinationEndpointOptions,
                        newMediaOptions["url"], newMediaOptions["type"])
            refreshSlot(newSlotIndex)
            visualModel.items.move(
                        getVisualItemIndex(newSlotIndex),
                        currentIndexVisual+1)
            currentIndex = newSlotIndex
            highlightedIndex = newSlotIndex
        }

        window.isModified = true
        undoManager.archive()

        // refresh interface manager
        dialogInterfaces.refresh()
    }

    function undo() {

        if (slotDetailView.slotIndex !== -1 && slotDetailView.isEdited) undoManager.archive()
        undoManager.undo()
    }

    function redo() {

        if (slotDetailView.slotIndex !== -1 && slotDetailView.isEdited) return
        undoManager.redo()
    }

    function setSlotLaunchRememberAllLinks(value) {
        for (var n=0 ; n<dataModel.count ; n++)
            dataModel.get(n).launchRememberLink = value
    }

    function setSlotLaunchRememberAllOutputs(value) {
        for (var n=0 ; n<dataModel.count ; n++)
            dataModel.get(n).launchRememberOutput = value
    }

    function getSlotLaunchOptions() {
        var listOptions = []
        for (var n=0 ; n<dataModel.count ; n++) {
            var options = {
                rememberLink: dataModel.get(n).launchRememberLink,
                rememberOutput: dataModel.get(n).launchRememberOutput
            }
            listOptions[n] = options
        }
        return listOptions;
    }

    function setSlotLaunchOptions(listOptions) {
        if (listOptions.length !== dataModel.count) return;
        for (var n=0 ; n<dataModel.count ; n++) {
            dataModel.setProperty(n, "launchRememberLink", listOptions[n].rememberLink)
            dataModel.setProperty(n, "launchRememberOutput", listOptions[n].rememberOutput)
        }
    }

    function getVisualItemIndex(dataItemIndex) {

        if (dataItemIndex === -1) return -1

        var visualItemIndex = -1
        for (var n=0 ; n<visualModel.items.count ; n++) {
            if (dataItemIndex === visualModel.items.get(n).model.index) {
                visualItemIndex = n
                break
            }
        }
        return visualItemIndex
    }

    function getDataItemIndex(visualItemIndex) {

        if (visualItemIndex === -1) return -1

        return visualModel.items.get(visualItemIndex).model.index
    }

    function getVisualItemsIndexList() {
        var list = []
        for (var n=0 ; n<visualModel.items.count ; n++) {
            list[n] = visualModel.items.get(n).model.index
        }
        return list
    }

    function getBookmarks() {
        var bookmarks = []
        for (var n=0 ; n<visualModel.items.count ; n++) {
            var model = visualModel.items.get(n).model
            if (model.slotBookmarked) {
                bookmarks.push({
                    value: n,
                    text:  model.slotTitle === undefined || model.slotTitle === "" ?
                               qsTr("Untitled Link") + " " + (model.index+1) :
                               model.slotTitle
                })
            }
        }
        return bookmarks
    }

    function gotoBookmark(indexVisual) {

        listView.highlightMoveDuration = 0

        // goto the bottom of the page where places the bookmarked item
        var indexVisual1 = indexVisual + Math.floor(listView.height / 100)
        currentIndexVisual = Math.min(indexVisual1, visualModel.items.count-1)

        // and goto the bookmarked item after a while
        currentIndexVisual = indexVisual
        highlightedIndex = currentIndex

        listView.highlightMoveDuration = 200
    }

    function newSlot() {
        var newSlotIndex = app.newSlot()
        slotListView.refreshSlot(newSlotIndex)
        if (currentIndex !== -1) {
            visualModel.items.move(
                        getVisualItemIndex(newSlotIndex),
                        currentIndexVisual+1)
        }
        slotListView.currentIndex = newSlotIndex
        slotListView.highlightedIndex = newSlotIndex
        window.isModified = true
        undoManager.archive()
    }

    function isBlankSlot(slotIndex) {
        var model = dataModel.get(slotIndex)
        return model.epInSignal === 0 && model.epOutSignal === 0
    }

    function setGroupLinked(slotIndex, slotLinked) {
        for (var n=getVisualItemIndex(slotIndex)+1 ; n<visualModel.items.count ; n++) {
            var index = getDataItemIndex(n)
            if (isBlankSlot(index))
                break
            else
                app.slotAt(index).setLinked(slotLinked)
        }
    }

}
