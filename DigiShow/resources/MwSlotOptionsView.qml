import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: slotOptionsView

    property int inputFullRange: 0
    property int outputFullRange: 0

    property var slotInfoBackup: ({})

    signal slotOptionUpdated(string key, var value)

    Item {

        id: itemInformation

        anchors.fill: parent
        visible: false

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -24
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#dddddd"
            font.pixelSize: 16
            font.bold: true
            text: qsTr("Signal mapping transformation options")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 20
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#999999"
            font.pixelSize: 14
            font.bold: false
            text: qsTr("Please select signal endpoints of both source and destination")
        }
    }

    Item {
        id: itemInputExpression

        height: 30
        anchors.bottom: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        CButton {
            id: buttonInputExpression
            width: 110
            height: 18
            anchors.verticalCenter: textInputExpression.verticalCenter
            anchors.right: textInputExpression.right
            anchors.rightMargin: 12
            label.font.pixelSize: 10
            label.font.bold: false
            label.text: qsTr("Add JS Expression")
            box.border.width: 1
            box.radius: 9
            visible: !textInputExpression.visible

            colorNormal: mouseOver ? "#331122" : "transparent"
            opacity: mouseOver ? 1.0 : 0.4
            Behavior on opacity { NumberAnimation { duration: 200 } }

            onClicked: {
                textInputExpression.text = "value"
                textInputExpression.apply()
                textInputExpression.input.forceActiveFocus()
            }

            Image {
                width: 16
                height: 16
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:///images/icon_expression_white.png"
                opacity: 0.5
            }
        }

        CTextInputBox {

            id: textInputExpression

            property bool isEditing: false
            property bool hasError: false
            property alias isLargeEditor: rectInputExpression.visible

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.left: parent.left
            anchors.right: parent.right
            box.color: isEditing ? "#000000" : (hasError ? "#770000" : "#331122")
            box.radius: 14
            input.anchors.leftMargin: 50
            input.anchors.rightMargin: 80
            input.font.pixelSize: 12
            input.font.bold: true
            text: ""
            visible: text !== "" || isEditing

            onTextEdited: isEditing = true
            onReturnPressed: apply()

            onIsLargeEditorChanged: {
                if (isLargeEditor) {
                    var isAlreadyEditing = isEditing
                    editInputExpression.text = text
                    editInputExpression.forceActiveFocus()
                    isEditing = isAlreadyEditing;
                } else {
                    text = editInputExpression.text
                }
            }

            Image {
                width: 16
                height: 16
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:///images/icon_expression_white.png"
                opacity: 0.5
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 36
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                opacity: 0.5
                font.pixelSize: 16
                font.bold: true
                text: "="
            }

            Rectangle {
                id: rectInputExpression

                height: 142
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                color: parent.box.color
                radius: 8
                border.width: 1
                border.color: "#555555"
                visible: false

                MouseArea {
                    anchors.fill: parent
                }

                ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 4
                    anchors.leftMargin: 16
                    anchors.bottomMargin: 4
                    anchors.rightMargin: 110
                    clip: true

                    background: Rectangle {
                        color: "transparent"
                    }

                    TextArea {
                        id: editInputExpression

                        color: "#cccccc"
                        selectionColor: "#666666"
                        selectedTextColor: "#ffffff"
                        selectByMouse: true
                        font.pixelSize: 12
                        font.bold: true
                        wrapMode: TextEdit.WordWrap
                        tabStopDistance: 10

                        onTextChanged: textInputExpression.isEditing = true
                    }
                }
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                label.font.pixelSize: 11
                label.font.bold: true
                label.text: qsTr("?")
                box.radius: 9

                onClicked: expressionHelp(textInputExpression)
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 12
                icon.height: 12
                icon.source: parent.isLargeEditor ? "qrc:///images/icon_edit_one_white.png" : "qrc:///images/icon_edit_multi_white.png"
                box.radius: 9

                onClicked: parent.isLargeEditor = !parent.isLargeEditor
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 54
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 16
                icon.height: 16
                icon.source: "qrc:///images/icon_close_white.png"
                box.radius: 9
                visible: !parent.isLargeEditor

                onClicked: {
                    parent.text = ""
                    parent.isEditing = true
                }
            }

            CButton {
                width: 50
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 54
                anchors.verticalCenter: parent.verticalCenter
                label.font.pixelSize: 10
                label.font.bold: false
                label.text: qsTr("OK")
                box.radius: 9
                colorNormal: Material.accent
                visible: parent.isEditing

                onClicked: parent.apply()
            }

            function apply() {

                isEdited = true
                isModified = true

                isLargeEditor = false
                if (text !== "") {
                    setSlotOption("inputExpression", text)
                } else {
                    clearSlotOption("inputExpression")
                }
                isEditing = false
                parent.forceActiveFocus()
            }
        }
    }

    Item {
        id: itemOutputExpression

        height: 30
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        CButton {
            id: buttonOutputExpression
            width: 110
            height: 18
            anchors.verticalCenter: textOutputExpression.verticalCenter
            anchors.right: textOutputExpression.right
            anchors.rightMargin: 12
            label.font.pixelSize: 10
            label.font.bold: false
            label.text: qsTr("Add JS Expression")
            box.border.width: 1
            box.radius: 9
            visible: !textOutputExpression.visible

            colorNormal: mouseOver ? "#331122" : "transparent"
            opacity: mouseOver ? 1.0 : 0.4
            Behavior on opacity { NumberAnimation { duration: 200 } }

            onClicked: {
                textOutputExpression.text = "value"
                textOutputExpression.apply()
                textOutputExpression.input.forceActiveFocus()
            }

            Image {
                width: 16
                height: 16
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:///images/icon_expression_white.png"
                opacity: 0.5
            }
        }

        CTextInputBox {

            id: textOutputExpression

            property bool isEditing: false
            property bool hasError: false
            property alias isLargeEditor: rectOutputExpression.visible

            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.right: parent.right
            box.color: isEditing ? "#000000" : (hasError ? "#770000" : "#331122")
            box.radius: 14
            input.anchors.leftMargin: 50
            input.anchors.rightMargin: 80
            input.font.pixelSize: 12
            input.font.bold: true
            text: ""
            visible: text !== "" || isEditing

            onTextEdited: isEditing = true
            onReturnPressed: apply()

            onIsLargeEditorChanged: {
                if (isLargeEditor) {
                    var isAlreadyEditing = isEditing
                    editOutputExpression.text = text
                    editOutputExpression.forceActiveFocus()
                    isEditing = isAlreadyEditing;
                } else {
                    text = editOutputExpression.text
                }
            }

            Image {
                width: 16
                height: 16
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:///images/icon_expression_white.png"
                opacity: 0.5
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 36
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                opacity: 0.5
                font.pixelSize: 16
                font.bold: true
                text: "="
            }

            Rectangle {
                id: rectOutputExpression

                height: 142
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                color: parent.box.color
                radius: 8
                border.width: 1
                border.color: "#555555"
                visible: false

                MouseArea {
                    anchors.fill: parent
                }

                ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 4
                    anchors.leftMargin: 16
                    anchors.bottomMargin: 4
                    anchors.rightMargin: 110
                    clip: true

                    background: Rectangle {
                        color: "transparent"
                    }

                    TextArea {
                        id: editOutputExpression

                        color: "#cccccc"
                        selectionColor: "#666666"
                        selectedTextColor: "#ffffff"
                        selectByMouse: true
                        font.pixelSize: 12
                        font.bold: true
                        wrapMode: TextEdit.WordWrap
                        tabStopDistance: 10

                        onTextChanged: textOutputExpression.isEditing = true
                    }
                }
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                label.font.pixelSize: 11
                label.font.bold: true
                label.text: qsTr("?")
                box.radius: 9

                onClicked: expressionHelp(textOutputExpression)
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 12
                icon.height: 12
                icon.source: parent.isLargeEditor ? "qrc:///images/icon_edit_one_white.png" : "qrc:///images/icon_edit_multi_white.png"
                box.radius: 9

                onClicked: parent.isLargeEditor = !parent.isLargeEditor
            }

            CButton {
                width: 18
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 54
                anchors.verticalCenter: parent.verticalCenter
                icon.width: 16
                icon.height: 16
                icon.source: "qrc:///images/icon_close_white.png"
                box.radius: 9
                visible: !parent.isLargeEditor

                onClicked: {
                    parent.text = ""
                    parent.isEditing = true
                }
            }

            CButton {
                width: 50
                height: 18
                anchors.right: parent.right
                anchors.rightMargin: 54
                anchors.verticalCenter: parent.verticalCenter
                label.font.pixelSize: 10
                label.font.bold: false
                label.text: qsTr("OK")
                box.radius: 9
                colorNormal: Material.accent
                visible: parent.isEditing

                onClicked: parent.apply()
            }

            function apply() {

                isEdited = true
                isModified = true

                isLargeEditor = false
                if (text !== "") {
                    setSlotOption("outputExpression", text)
                } else {
                    clearSlotOption("outputExpression")
                }
                isEditing = false
                parent.forceActiveFocus()
            }
        }
    }

    CheckBox {
        id: checkInputInverted

        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 5
        font.pixelSize: 12
        text: qsTr("Invert Input Signal")

        onClicked: setSlotOption("inputInverted", checked)
    }

    CheckBox {
        id: checkOutputInverted

        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        font.pixelSize: 12
        text: qsTr("Invert Output Signal")

        onClicked: setSlotOption("outputInverted", checked)
    }

    Item {
        id: itemSlotOptions

        anchors.fill: parent
        visible: false

        CButton {
            id: buttonSlotOptionsMenu
            width: 28
            height: 28
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.top: parent.top
            anchors.topMargin: 16
            colorNormal: "transparent"
            icon.width: 14
            icon.height: 14
            icon.source: "qrc:///images/icon_arrow_down_white.png"
            icon.anchors.verticalCenterOffset: 2
            box.border.width: 1
            box.radius: 3

            onClicked: {
                if (!menuSlotOptionsMenu.visible) {
                    menuItemPaste.enabled = utilities.clipboardExists("application/vnd.digishow.options")
                    menuSlotOptionsMenu.open()
                }
                else menuSlotOptionsMenu.close()
            }

            CMenu {
                id: menuSlotOptionsMenu
                x: parent.width - this.width
                y: parent.height + 2
                width: 80
                transformOrigin: Menu.TopRight
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                CMenuItem {
                    text: qsTr("Revert")
                    onTriggered: {
                        importSlotInfo(slotInfoBackup)
                    }
                }
                CMenuItem {
                    text: qsTr("Copy")
                    onTriggered: {
                        utilities.copyJson(exportSlotInfo(), "application/vnd.digishow.options")
                    }
                }
                CMenuItem {
                    id: menuItemPaste
                    text: qsTr("Paste")
                    onTriggered: {
                        importSlotInfo(utilities.pasteJson("application/vnd.digishow.options"))
                    }
                }
            }
        }

        Rectangle {
            id: lineSpliter

            width: 1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 70
            anchors.bottomMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#333333"

            visible: itemMappingOptions.visible && itemEnvelopeOptions.visible
        }

        CheckBox {
            id: checkOutputLowAsZero

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 45
            font.pixelSize: 12
            text: qsTr("Zero Output While Reaching Lower")
            enabled:
                sliderMappingInputRange.visible && sliderMappingOutputRange.visible &&
                sliderMappingOutputRange.first.value > 0
            opacity: enabled ? 1 : 0

            // Behavior on opacity { NumberAnimation { duration: 200 } }

            onClicked: setSlotOption("outputLowAsZero", checked)
        }

        CSpinBox {
            id: spinOutputSmoothing

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: parent.width/2 + 120
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            from: 0
            to: 9000000
            stepSize: 10
            unit: "ms"

            onValueModified: setSlotOption("outputSmoothing", value)

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Output Smoothing")
            }
        }


        Row {
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: 50

            Item {
                id: itemMappingOptions

                width: parent.width/2 - 25
                height: parent.height - 100
                anchors.top: parent.top
                anchors.topMargin: 50

                Text {
                    id: labelMapping

                    color: "#cccccc"
                    font.pixelSize: 12
                    font.bold: true
                    text: qsTr("Input-output Mapping")
                }

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: labelMapping.bottom
                    anchors.topMargin: 65
                    spacing: 95

                    CRangeSlider {
                        id: sliderMappingInputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("inputLow", parseFloat(first.value.toFixed(10)))
                        second.onMoved: setSlotOption("inputHigh", parseFloat(second.value.toFixed(10)))

                        Text {
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 13
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Input Range")
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 13
                            color: Material.accent
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " - " +
                                  Math.round(parent.second.value*100) + " %"
                        }

                        CSpinBox {
                            id: spinInputRangeLower
                            width: Math.min(parent.width / 2 - 15, 140)
                            anchors.left: parent.left
                            anchors.leftMargin: -3
                            anchors.top: parent.bottom
                            anchors.topMargin: 14
                            from: 0
                            to: Math.round(parent.second.value*inputFullRange)
                            value: Math.round(parent.first.value*inputFullRange)
                            stepSize: 1

                            onValueModified: {
                                parent.first.value = value / inputFullRange
                                value = Qt.binding(function() { return Math.round(parent.first.value*inputFullRange) })
                                parent.first.moved()
                            }
                        }

                        CSpinBox {
                            id: spinInputRangeUpper
                            width: Math.min(parent.width / 2 - 15, 140)
                            anchors.right: parent.right
                            anchors.rightMargin: -3
                            anchors.top: parent.bottom
                            anchors.topMargin: 14
                            from: Math.round(parent.first.value*inputFullRange)
                            to: inputFullRange
                            value: Math.round(parent.second.value*inputFullRange)
                            stepSize: 1

                            onValueModified: {
                                parent.second.value = value / inputFullRange
                                value = Qt.binding(function() { return Math.round(parent.second.value*inputFullRange) })
                                parent.second.moved()
                            }
                        }
                    }

                    CRangeSlider {
                        id: sliderMappingOutputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("outputLow", parseFloat(first.value.toFixed(10)))
                        second.onMoved: setSlotOption("outputHigh", parseFloat(second.value.toFixed(10)))

                        Text {
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 13
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Output Range")
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 13
                            color: Material.accent
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " - " +
                                  Math.round(parent.second.value*100) + " %"
                        }

                        CSpinBox {
                            id: spinOutnputRangeLower
                            width: Math.min(parent.width / 2 - 15, 140)
                            anchors.left: parent.left
                            anchors.leftMargin: -3
                            anchors.top: parent.bottom
                            anchors.topMargin: 14
                            from: 0
                            to: Math.round(parent.second.value*outputFullRange)
                            value: Math.round(parent.first.value*outputFullRange)
                            stepSize: 1

                            onValueModified: {
                                parent.first.value = value / outputFullRange
                                value = Qt.binding(function() { return Math.round(parent.first.value*outputFullRange) })
                                parent.first.moved()
                            }
                        }

                        CSpinBox {
                            id: spinOutnputRangeUpper
                            width: Math.min(parent.width / 2 - 15, 140)
                            anchors.right: parent.right
                            anchors.rightMargin: -3
                            anchors.top: parent.bottom
                            anchors.topMargin: 14
                            from: Math.round(parent.first.value*outputFullRange)
                            to: outputFullRange
                            value: Math.round(parent.second.value*outputFullRange)
                            stepSize: 1

                            onValueModified: {
                                parent.second.value = value / outputFullRange
                                value = Qt.binding(function() { return Math.round(parent.second.value*outputFullRange) })
                                parent.second.moved()
                            }
                        }
                    }
                }
            }

            Item {
                id: itemEnvelopeOptions

                width: parent.width/2 - 25
                height: parent.height - 100
                anchors.top: parent.top
                anchors.topMargin: 50

                Text {
                    id: labelEnvelope

                    color: "#cccccc"
                    font.pixelSize: 12
                    font.bold: true
                    text: qsTr("Output Envelope")
                }

                CButton {
                    id: buttonEnvelopeHelp

                    width: 18
                    height: 18
                    anchors.verticalCenter: labelEnvelope.verticalCenter
                    anchors.left: labelEnvelope.right
                    anchors.leftMargin: 10
                    label.text: "?"
                    label.font.pixelSize: 11
                    label.font.bold: true
                    box.radius: 9

                    onClicked: popupEnvelopeHelp.open()
                }

                Popup {
                    id: popupEnvelopeHelp
                    width: 750
                    height: 450
                    anchors.centerIn: Overlay.overlay
                    modal: true
                    dim: true
                    clip: true

                    background: Rectangle {
                        anchors.fill: parent
                        color: "black"
                        radius: 12
                    }

                    Image {
                        anchors.fill: parent
                        source: "qrc:///images/AHDSR.png"
                    }
                }

                Column {
                    anchors.top: labelEnvelope.bottom
                    anchors.topMargin: 30
                    spacing: 3

                    CSpinBox {
                        id: spinEnvelopeInDelay

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeInDelay", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("On Delay")
                        }
                    }

                    //Item {
                    //    width: 120
                    //    height: 9
                    //}

                    CSpinBox {
                        id: spinEnvelopeAttack

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeAttack", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Attack")
                        }
                    }

                    CSpinBox {
                        id: spinEnvelopeHold

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeHold", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Hold")
                        }
                    }

                    CSpinBox {
                        id: spinEnvelopeDecay

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeDecay", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Decay")
                        }
                    }

                    CSpinBox {
                        id: spinEnvelopeSustain

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 100
                        value: 100
                        stepSize: 5
                        unit: "%"

                        onValueModified: setSlotOption("envelopeSustain", value * 0.01)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Sustain")
                        }
                    }

                    CSpinBox {
                        id: spinEnvelopeRelease

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeRelease", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Release")
                        }
                    }

                    //Item {
                    //    width: 120
                    //    height: 9
                    //}

                    CSpinBox {
                        id: spinEnvelopeOutDelay

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 95
                        from: 0
                        to: 9000000
                        stepSize: 10
                        unit: "ms"

                        onValueModified: setSlotOption("envelopeOutDelay", value)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Off Delay")
                        }
                    }

                }

            }

        }

    }

    function refresh() {

        slotInfoBackup = {}

        // show information and hide all options when slot is not ready
        itemInformation.visible = true
        itemSlotOptions.visible = false
        itemInputExpression.visible = false
        itemOutputExpression.visible = false
        checkInputInverted.visible = false
        checkOutputInverted.visible = false

        // confirm and get slot configuration
        if (slotIndex === -1) return

        var config = digishow.getSlotConfiguration(slotIndex)
        var slotInfo = config["slotInfo"]

        if (slotInfo === undefined) return
        slotInfoBackup = slotInfo

        // show signal inversion options when source/destination endpoint is ready
        var inputSignal  = (digishow.getSourceEndpointIndex     (slotIndex) === -1 ? 0 : slotInfo["inputSignal" ])
        var outputSignal = (digishow.getDestinationEndpointIndex(slotIndex) === -1 ? 0 : slotInfo["outputSignal"])

        if (inputSignal !== 0)
            itemInputExpression.visible = true
        if (outputSignal !== 0)
            itemOutputExpression.visible = true

        if (inputSignal === DigishowEnvironment.SignalAnalog ||
            inputSignal === DigishowEnvironment.SignalBinary)
            checkInputInverted.visible = true

        if (outputSignal === DigishowEnvironment.SignalAnalog ||
            outputSignal === DigishowEnvironment.SignalBinary)
            checkOutputInverted.visible = true

        // confirm both endpoints are ready
        // and show slot options
        if (inputSignal !== 0 && outputSignal !==0) {
            itemInformation.visible = false
            itemSlotOptions.visible = true
            inputFullRange = slotInfo["inputRange"]
            outputFullRange = slotInfo["outputRange"]
        }

        // show / hide detailed options according to input-output signals
        checkOutputLowAsZero.visible = false
        spinOutputSmoothing.visible = false
        itemMappingOptions.visible = false
        itemEnvelopeOptions.visible = false

        if (inputSignal  === DigishowEnvironment.SignalAnalog &&
            outputSignal === DigishowEnvironment.SignalAnalog) {

            checkOutputLowAsZero.visible = true
            spinOutputSmoothing.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = false

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            spinOutputSmoothing.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = true

            refreshEnvelopeForAnalog()

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            refreshEnvelopeForBinary()

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = true

            refreshEnvelopeForBinary()

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            checkOutputLowAsZero.visible = true
            spinOutputSmoothing.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

            refreshEnvelopeForAnalog()


        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            refreshEnvelopeForBinary()

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

            refreshEnvelopeForBinary()
        }

        // refresh option values
        // or clear unused options
        if (checkInputInverted.visible) {
            checkInputInverted.checked = slotInfo["inputInverted"]
        } else {
            clearSlotOption("inputInverted")
        }

        if (checkOutputInverted.visible) {
            checkOutputInverted.checked = slotInfo["outputInverted"]
        } else {
            clearSlotOption("outputInverted")
        }

        if (checkOutputLowAsZero.visible) {
            checkOutputLowAsZero.checked = slotInfo["outputLowAsZero"]
        } else {
            clearSlotOption("outputLowAsZero")
        }

        if (spinOutputSmoothing.visible) {
            spinOutputSmoothing.value = slotInfo["outputSmoothing"]
        } else {
            clearSlotOption("outputSmoothing")
        }

        if (sliderMappingInputRange.visible) {
            sliderMappingInputRange.first.value = 0
            sliderMappingInputRange.second.value = 1
            sliderMappingInputRange.first.value = slotInfo["inputLow"]
            sliderMappingInputRange.second.value = slotInfo["inputHigh"]
        } else {
            clearSlotOption("inputLow")
            clearSlotOption("inputHigh")
        }

        if (sliderMappingOutputRange.visible) {
            sliderMappingOutputRange.first.value = 0
            sliderMappingOutputRange.second.value = 1
            sliderMappingOutputRange.first.value = slotInfo["outputLow"]
            sliderMappingOutputRange.second.value = slotInfo["outputHigh"]
        } else {
            clearSlotOption("outputLow")
            clearSlotOption("outputHigh")
        }

        if (spinEnvelopeAttack.visible) {
            spinEnvelopeAttack.value = slotInfo["envelopeAttack"]
        } else {
            clearSlotOption("envelopeAttack")
        }

        if (spinEnvelopeHold.visible) {
            spinEnvelopeHold.value = slotInfo["envelopeHold"]
        } else {
            clearSlotOption("envelopeHold")
        }

        if (spinEnvelopeDecay.visible) {
            spinEnvelopeDecay.value = slotInfo["envelopeDecay"]
        } else {
            clearSlotOption("envelopeDecay")
        }

        if (spinEnvelopeSustain.visible) {
            spinEnvelopeSustain.value = Math.round(slotInfo["envelopeSustain"]*100)
        } else {
            clearSlotOption("envelopeSustain")
        }

        if (spinEnvelopeRelease.visible) {
            spinEnvelopeRelease.value = slotInfo["envelopeRelease"]
        } else {
            clearSlotOption("envelopeRelease")
        }

        if (spinEnvelopeInDelay.visible) {
            spinEnvelopeInDelay.value = slotInfo["envelopeInDelay"]
        } else {
            clearSlotOption("envelopeInDelay")
        }

        if (spinEnvelopeOutDelay.visible) {
            spinEnvelopeOutDelay.value = slotInfo["envelopeOutDelay"]
        } else {
            clearSlotOption("envelopeOutDelay")
        }

        textInputExpression.isLargeEditor = false
        textInputExpression.isEditing = false
        textInputExpression.text = slotInfo["inputExpression"]
        textOutputExpression.isLargeEditor = false
        textOutputExpression.isEditing = false
        textOutputExpression.text = slotInfo["outputExpression"]
    }

    function refreshEnvelopeForAnalog() {

        itemEnvelopeOptions.visible = true
        buttonEnvelopeHelp.visible = true
        spinEnvelopeAttack.visible = true
        spinEnvelopeHold.visible = true
        spinEnvelopeDecay.visible = true
        spinEnvelopeSustain.visible = true
        spinEnvelopeRelease.visible = true
        spinEnvelopeInDelay.visible = true
        spinEnvelopeOutDelay.visible = true
    }

    function refreshEnvelopeForBinary() {

        itemEnvelopeOptions.visible = true
        buttonEnvelopeHelp.visible = false
        spinEnvelopeAttack.visible = false
        spinEnvelopeHold.visible = true
        spinEnvelopeDecay.visible = false
        spinEnvelopeSustain.visible = false
        spinEnvelopeRelease.visible = false
        spinEnvelopeInDelay.visible = true
        spinEnvelopeOutDelay.visible = true
    }

    function importSlotInfo(slotInfo) {

        function importCheckItem(item, key) {
            var checked = slotInfo[key]
            if (item.visible && checked !== undefined) {
                item.checked = checked
                setSlotOption(key, checked)
            }
        }

        function importValueItem(item, key, scale) {
            var value = slotInfo[key]
            if (item.visible && value !== undefined) {
                item.value = (scale===undefined ? value : Math.round(value*scale))
                setSlotOption(key, value)
            }
        }

        function importRangeItem(item, key1, key2) {
            var value1 = slotInfo[key1]
            var value2 = slotInfo[key2]
            if (item.visible && (value1 !== undefined || value2 !== undefined)) {
                if (value1 === undefined) value1 = 0
                if (value2 === undefined) value2 = 1
                item.first.value = 0
                item.second.value = 1
                item.first.value = value1
                item.second.value = value2
                setSlotOption(key1, parseFloat(value1.toFixed(10)))
                setSlotOption(key2, parseFloat(value2.toFixed(10)))
            }
        }

        importCheckItem(checkInputInverted,       "inputInverted")
        importCheckItem(checkOutputInverted,      "outputInverted")
        importCheckItem(checkOutputLowAsZero,     "outputLowAsZero")
        importValueItem(spinOutputSmoothing,      "outputSmoothing")
        importRangeItem(sliderMappingInputRange,  "inputLow", "inputHigh")
        importRangeItem(sliderMappingOutputRange, "outputLow", "outputHigh")
        importValueItem(spinEnvelopeAttack,       "envelopeAttack")
        importValueItem(spinEnvelopeHold,         "envelopeHold")
        importValueItem(spinEnvelopeDecay,        "envelopeDecay")
        importValueItem(spinEnvelopeSustain,      "envelopeSustain", 100)
        importValueItem(spinEnvelopeRelease,      "envelopeRelease")
        importValueItem(spinEnvelopeInDelay,      "envelopeInDelay")
        importValueItem(spinEnvelopeOutDelay,     "envelopeOutDelay")
    }

    function exportSlotInfo() {

        var slotInfo = {}

        function exportCheckItem(item, key) {
            if (item.visible) slotInfo[key] = item.checked
        }

        function exportValueItem(item, key, scale) {
            if (item.visible) slotInfo[key] = (scale===undefined ? item.value : item.value            /scale)
        }

        function exportRangeItem(item, key1, key2) {
            if (item.visible) {
                slotInfo[key1] = item.first.value
                slotInfo[key2] = item.second.value
            }
        }

        exportCheckItem(checkInputInverted,       "inputInverted")
        exportCheckItem(checkOutputInverted,      "outputInverted")
        exportCheckItem(checkOutputLowAsZero,     "outputLowAsZero")
        exportValueItem(spinOutputSmoothing,      "outputSmoothing")
        exportRangeItem(sliderMappingInputRange,  "inputLow", "inputHigh")
        exportRangeItem(sliderMappingOutputRange, "outputLow", "outputHigh")
        exportValueItem(spinEnvelopeAttack,       "envelopeAttack")
        exportValueItem(spinEnvelopeHold,         "envelopeHold")
        exportValueItem(spinEnvelopeDecay,        "envelopeDecay")
        exportValueItem(spinEnvelopeSustain,      "envelopeSustain", 100)
        exportValueItem(spinEnvelopeRelease,      "envelopeRelease")
        exportValueItem(spinEnvelopeInDelay,      "envelopeInDelay")
        exportValueItem(spinEnvelopeOutDelay,     "envelopeOutDelay")

        return slotInfo
    }

    function setSlotOption(key, value) {
        if (slotIndex !== -1) {
            app.slotAt(slotIndex).setSlotOption(key, value)
            slotOptionUpdated(key, value) // emit signal
        }
    }

    function clearSlotOption(key) {
        if (slotIndex !== -1) {
            app.slotAt(slotIndex).clearSlotOption(key)
            //slotOptionUpdated(key, null) // emit signal
        }
    }

    function expressionHelp(textExpression) {

        function expressionReference() {
            Qt.openUrlExternally("https://github.com/robinz-labs/digishow/blob/master/guides/" + qsTr("expression.md"))
        }

        var exampleCode = "value*0.5 + 100"
        var btn = 0
        btn = messageBox.showAndWait(
            qsTr("Write a JavaScript expression to dynamically change the signal value, for example:") + "\r\n\r\n= " + exampleCode,
            qsTr("Try It"), qsTr("Reference ..."), qsTr("Next"))
        if (btn == 1) { textExpression.isLargeEditor = false; textExpression.text = exampleCode; textExpression.isEditing = true; }
        if (btn == 2) expressionReference()
        if (btn <= 2) return

        exampleCode = "value*0.5 + outputValueOf('Another Signal Link')*0.5"
        btn = messageBox.showAndWait(
            qsTr("The expression can reference the input or output value of any other signal link, for example:") + "\r\n\r\n= " + exampleCode,
            qsTr("Try It"), qsTr("Reference ..."), qsTr("Next"))
        if (btn == 1) { textExpression.isLargeEditor = false; textExpression.text = exampleCode; textExpression.isEditing = true; }
        if (btn == 2) expressionReference()
        if (btn <= 2) return

        exampleCode = "value/range > 0.5 ? value*(Math.random()*0.2 + 0.8) : null"
        btn = messageBox.showAndWait(
            qsTr("The expression can contain common JavaScript statements and functions, for example:") + "\r\n\r\n= " + exampleCode,
            qsTr("Try It"), qsTr("Reference ..."), qsTr("Next"))
        if (btn == 1) { textExpression.isLargeEditor = false; textExpression.text = exampleCode; textExpression.isEditing = true; }
        if (btn == 2) expressionReference()
        if (btn <= 2) return

        if (app.scriptableFileExists()) {
            btn = messageBox.showAndWait(
                qsTr("The expression can also call user-defined functions, now you can create your functions in the attached script file."),
                qsTr("Show Script"), qsTr("Reference ..."), qsTr("Done"))
            if (btn == 1) showScriptableFile()
            if (btn == 2) expressionReference()

        } else {
            btn = messageBox.showAndWait(
                qsTr("The expression can also call user-defined functions, now you can create an attached script file contains your functions."),
                qsTr("Create Script"), qsTr("Reference ..."), qsTr("Done"))
            if (btn == 1) createScriptableFile()
            if (btn == 2) expressionReference()
        }

    }

}
