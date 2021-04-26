import QtQml.Models 2.1
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: slotListView

    property int   selectedIndex: -1 // index to data modal
    property int   currentIndex:  -1 // index to data modal
    property alias currentIndexVisual: listView.currentIndex // index to visual modal
    property alias listItemCount: dataModel.count

    onSelectedIndexChanged: currentIndex = selectedIndex
    onCurrentIndexChanged: currentIndexVisual = getVisualItemIndex(currentIndex)
    onCurrentIndexVisualChanged: currentIndex = getDataItemIndex(currentIndexVisual)

    Rectangle {

        anchors.fill: parent
        color: "#181818"
        clip: true

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#555555"
            text: qsTr("Please tap button + to add a new slot \r\n that enables signal link between your digital things")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            font.bold: true
            lineHeight: 2.0
            visible: dataModel.count===0

            Rectangle {
                width: 90
                height: 30
                anchors.top: parent.bottom
                anchors.topMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 15
                color: "#333333"
                opacity: 0.3

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

        Component {
            id: dragDelegate

            MouseArea {
                id: dragArea

                property bool held: false

                height: 70
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20

                drag.target: held ? content : undefined
                drag.axis: Drag.YAxis
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                //onPressAndHold: held = true
                onPressed: held = true
                onReleased: held = false
                onClicked: {
                    selectedIndex = index
                    listView.forceActiveFocus()

                    if (mouse.button === Qt.RightButton) {
                        menuSlot.x = mouse.x
                        menuSlot.y = mouse.y
                        menuSlot.open()
                    }
                }

                CMenu {
                    id: menuSlot
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    CMenuItem {
                        text: qsTr("Rename Slot")
                        onTriggered: {
                            menuSlot.close()
                            labelSlotTitle.showRename()
                            labelSlotTitle.showRename() //?
                        }
                    }
                    CMenuItem {
                        text: qsTr("Duplicate Slot")
                        onTriggered: {
                            menuSlot.close()
                            var newSlotIndex = app.duplicateSlot(currentIndex)
                            slotListView.refreshSlot(newSlotIndex)
                            visualModel.items.move(
                                        getVisualItemIndex(newSlotIndex),
                                        currentIndexVisual+1)
                            slotListView.currentIndex = newSlotIndex
                            slotListView.selectedIndex = newSlotIndex

                            isModified = true
                        }
                    }
                    CMenuItem {
                        text: qsTr("Delete Slot")
                        onTriggered: {
                            menuSlot.close()
                            deleteSlot(currentIndex)
                        }
                    }
                }

                Rectangle {
                    id: content
                    anchors.fill: dragArea
                    border.width: 2
                    border.color: selectedIndex===index ? "#cccccc" : currentIndex===index ? "#666666" : color
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

                    Text {
                        id: labelSlotTitle

                        anchors.left: parent.left
                        //anchors.leftMargin: 260
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        anchors.topMargin: 8
                        horizontalAlignment: Text.AlignHCenter

                        color: selectedIndex===index ? "#cccccc" : "#666666"
                        text: model.slotTitle === undefined || model.slotTitle === "" ?
                                  qsTr("Untitled Slot") + " " + (index+1) :
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

                            width: Math.round(100 * value)
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.margins: 3
                            radius: 3
                            color: model.epInColor

                            Behavior on value { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }
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
                            anchors.rightMargin: 4
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
                            text: model.epInAvailable ? model.epInValue : "- -"
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
                            model.slotLinked = !model.slotLinked;
                            app.slotAt(index).setLinked(model.slotLinked)
                        }
                    }

                    Image {
                        width: 16
                        height: 16
                        anchors.left: buttonLink.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:///images/icon_arrow_right_white.png"
                        opacity: model.epInBusy ? 1.0 : 0.1
                        visible: model.slotLinked
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

                            Behavior on value { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }
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

                        Image {
                            width: 16
                            height: 16
                            anchors.top: parent.bottom
                            anchors.topMargin: 4
                            anchors.right: parent.right
                            anchors.rightMargin: 4
                            source: "qrc:///images/icon_invert_white.png"
                            visible: model.slotOutInverted
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
                            text: model.epOutAvailable ? model.epOutValue : "- -"
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
                                model.epOutValue = Math.round(faderOutput.value)
                                app.slotAt(index).setEndpointOutValue(model.epOutValue)
                            }
                            onReleased: {
                                model.epOutTap = false
                                model.epOutValue = (model.slotOutInverted ? model.epOutRange : 0)
                                app.slotAt(index).setEndpointOutValue(model.epOutValue)
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
                                    model.epOutValue = Math.round(faderOutput.value)
                                    app.slotAt(index).setEndpointOutValue(model.epOutValue)
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
                            value: model.slotLinked && model.epOutFaderHold ? model.epOutValue : model.epOutFaderValue
                            to: model.epOutRange
                            stepSize: 1
                            color: model.epOutColor
                            inverted: model.slotOutInverted

                            Behavior on value { NumberAnimation { duration: (model.slotLinked && model.epOutFaderHold ? 300 : 0); easing.type: Easing.OutCubic } }

                            onMoved: {
                                if (model.epOutFaderHold) {
                                    model.epOutValue = Math.round(faderOutput.value)
                                    app.slotAt(index).setEndpointOutValue(model.epOutValue)
                                }
                                //model.epOutFaderValue = faderOutput.value
                            }

                            onValueChanged: {
                                model.epOutFaderValue = faderOutput.value
                            }
                        }
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
            highlightMoveDuration: 300

            ScrollBar.vertical: ScrollBar {
                width: 6
                topPadding: -20
                bottomPadding: -30
                rightPadding: 0
            }

            Keys.onReleased: {
                // slot actions
                if (selectedIndex !== -1) {

                    var slot = app.slotAt(selectedIndex)
                    var model = dataModel.get(selectedIndex)

                    if (event.key === Qt.Key_T) {

                        // tap
                        dataModel.setProperty(selectedIndex, "epOutTap", false)
                        slot.setEndpointOutValue(model.slotOutInverted ? model.epOutRange : 0)

                        event.accepted = true
                        return
                    }
                }
            }

            Keys.onPressed: {

                // slot actions
                if (selectedIndex !== -1) {

                    var slot = app.slotAt(selectedIndex)
                    var model = dataModel.get(selectedIndex)

                    if (event.key === Qt.Key_L) {

                        // link
                        slot.setLinked(!model.slotLinked)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_H) {

                        // hold
                        if (model.epOutFaderHold) {
                            slot.setEndpointOutValue(model.slotOutInverted ? model.epOutRange : 0)
                        } else {
                            dataModel.setProperty(selectedIndex, "epOutValue", model.epOutFaderValue)
                            slot.setEndpointOutValue(model.epOutFaderValue)
                        }
                        dataModel.setProperty(selectedIndex, "epOutFaderHold", !model.epOutFaderHold)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_T) {

                        // tap
                        dataModel.setProperty(selectedIndex, "epOutFaderHold", false)

                        if (model.slotOutInverted === false && model.epOutFaderValue === 0)
                            dataModel.setProperty(selectedIndex, "epOutFaderValue", model.epOutRange)
                        if (model.slotOutInverted === true && model.epOutFaderValue === model.epOutRange)
                            dataModel.setProperty(selectedIndex, "epOutFaderValue", 0)

                        dataModel.setProperty(selectedIndex, "epOutTap", true)
                        slot.setEndpointOutValue(model.epOutFaderValue)

                        event.accepted = true
                        return

                    } else if (event.key === Qt.Key_Left || event.key === Qt.Key_Right) {

                        // fader - or fader +
                        var signal = (event.key === Qt.Key_Left ? -1 : 1)
                        var epOutFaderValue = (model.epOutSignal === DigishowEnvironment.SignalBinary
                                               ? model.epOutFaderValue + signal * 1
                                               : model.epOutFaderValue + signal * Math.round(model.epOutRange * 0.1) )
                        epOutFaderValue = Math.min(epOutFaderValue, model.epOutRange)
                        epOutFaderValue = Math.max(epOutFaderValue, 0)
                        dataModel.setProperty(selectedIndex, "epOutFaderValue", epOutFaderValue)
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


                // change selected slot item
                if (event.key === Qt.Key_Space || event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {

                    selectedIndex = slotListView.currentIndex

                    event.accepted = true
                    return
                }

                // delete the selected slot
                if (event.key === Qt.Key_Delete) {

                    deleteSlot(slotListView.currentIndex)

                    event.accepted = true
                    return
                }
            }
        }
    }

    Timer {
        id: dataUpdater

        interval: 50
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
                if (dataModel.get(n).epInBusy       !== data["epInBusy"      ]) dataModel.setProperty(n, "epInBusy",       data["epInBusy"      ])
                if (dataModel.get(n).epOutBusy      !== data["epOutBusy"     ]) dataModel.setProperty(n, "epOutBusy",      data["epOutBusy"     ])
                if (dataModel.get(n).slotEnabled    !== data["slotEnabled"   ]) dataModel.setProperty(n, "slotEnabled",    data["slotEnabled"   ])
                if (dataModel.get(n).slotLinked     !== data["slotLinked"    ]) dataModel.setProperty(n, "slotLinked",     data["slotLinked"    ])
            }
        }
    }

    function refresh() {

        dataModel.clear()

        var slotCount = app.slotCount();
        for (var n=0 ; n<slotCount ; n++) refreshSlot(n)

        currentIndex = -1
        selectedIndex = -1
    }

    function refreshSlot(slotIndex) {

        var n = slotIndex
        var config = digishow.getSlotConfiguration(n)

        // obtain slot data
        var slotOptions = config["slotOptions"]
        var slotTitle = slotOptions["title"]
        if (slotTitle === undefined) slotTitle = ""

        var slotInfo = config["slotInfo"]
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
        var epInValue   = data["epInValue"]
        var epOutValue  = data["epOutValue"]
        var epInBusy    = data["epInBusy"]
        var epOutBusy   = data["epOutBusy"]
        var slotEnabled = data["slotEnabled"]
        var slotLinked  = data["slotLinked"]

        // add to data model
        var item = {

            //itemIndex: n,

            slotTitle: slotTitle,

            slotEnabled: slotEnabled,
            slotLinked: slotLinked,

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
            epOutBusy: epOutBusy,
            epOutAvailable: epOutAvailable,

            epOutFaderValue: 0,
            epOutFaderHold: true,
            epOutTap: false,

            launchRememberLink: true,
            launchRememberOutput: true
        }

        dataModel.set(n, item)
    }

    function deleteSlot(slotIndex) {

        var deletedIndex = slotIndex
        if (deletedIndex !== -1) {

            selectedIndex = deletedIndex

            if (messageBox.showAndWait(qsTr("Would you like to delete the selected slot ?"), qsTr("Delete"), qsTr("Cancel")) === 1) {

                selectedIndex = -1
                currentIndex = -1

                var indexVisaul = getVisualItemIndex(deletedIndex)

                app.deleteSlot(deletedIndex)
                dataModel.remove(deletedIndex) //refresh()

                if (indexVisaul >= 0 &&
                    indexVisaul < visualModel.items.count) {

                    currentIndexVisual = indexVisaul
                }

                isModified = true
            }

            listView.forceActiveFocus()
        }
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

    function newSlot() {
        var newSlotIndex = app.newSlot()
        slotListView.refreshSlot(newSlotIndex)
        if (currentIndex !== -1) {
            visualModel.items.move(
                        getVisualItemIndex(newSlotIndex),
                        currentIndexVisual+1)
        }
        slotListView.currentIndex = newSlotIndex
        slotListView.selectedIndex = newSlotIndex
        isModified = true
    }
}
