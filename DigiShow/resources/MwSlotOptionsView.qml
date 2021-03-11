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
    property var slotInfoClipboard: ({})

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
            text: qsTr("Signal mapping and transform options")
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
                if (!menuSlotOptionsMenu.visible) menuSlotOptionsMenu.open()
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
                        slotInfoClipboard = exportSlotInfo()
                    }
                }
                CMenuItem {
                    text: qsTr("Paste")
                    onTriggered: {
                        importSlotInfo(slotInfoClipboard)
                    }
                }
            }
        }


        Rectangle {
            id: lineSpliter

            width: 1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#333333"
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


        CheckBox {
            id: checkOutputLowAsZero

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.bottom: checkOutputInverted.top
            anchors.bottomMargin: -10
            font.pixelSize: 12
            text: qsTr("Zero Output While Reach Lower")
            enabled:
                sliderMappingInputRange.visible && sliderMappingOutputRange.visible &&
                sliderMappingOutputRange.first.value > 0
            opacity: enabled ? 1 : 0

            // Behavior on opacity { NumberAnimation { duration: 200 } }

            onClicked: setSlotOption("outputLowAsZero", checked)
        }

        CSpinBox {
            id: spinOutputSmoothing

            width: 110
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: checkOutputInverted.verticalCenter
            from: 0
            to: 60000
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
                anchors.topMargin: 70

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
                    anchors.topMargin: 66
                    spacing: 84

                    CRangeSlider {
                        id: sliderMappingInputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("inputLow", parseFloat(first.value.toFixed(5)))
                        second.onMoved: setSlotOption("inputHigh", parseFloat(second.value.toFixed(5)))

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.bottom
                            anchors.topMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " % - " +
                                  Math.round(parent.second.value*100) + " %"
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.bottom
                            anchors.topMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*inputFullRange) + " - " +
                                  Math.round(parent.second.value*inputFullRange)
                        }

                        Text {
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Input Range")
                        }
                    }

                    CRangeSlider {
                        id: sliderMappingOutputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("outputLow", parseFloat(first.value.toFixed(5)))
                        second.onMoved: setSlotOption("outputHigh", parseFloat(second.value.toFixed(5)))

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.bottom
                            anchors.topMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " % - " +
                                  Math.round(parent.second.value*100) + " %"
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.bottom
                            anchors.topMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*outputFullRange) + " - " +
                                  Math.round(parent.second.value*outputFullRange)
                        }

                        Text {
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 15
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Output Range")
                        }

                    }
                }
            }

            Item {
                id: itemEnvelopeOptions

                width: parent.width/2 - 25
                height: parent.height - 100
                anchors.top: parent.top
                anchors.topMargin: 70

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
                    spacing: 20

                    CSpinBox {
                        id: spinEnvelopeAttack

                        width: 120
                        anchors.left: parent.left
                        anchors.leftMargin: 80
                        from: 0
                        to: 60000
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
                        anchors.leftMargin: 80
                        from: 0
                        to: 60000
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
                        anchors.leftMargin: 80
                        from: 0
                        to: 60000
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
                        anchors.leftMargin: 80
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
                        anchors.leftMargin: 80
                        from: 0
                        to: 60000
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

                }

            }

        }

    }

    function refresh() {

        slotInfoBackup = {}

        // show information when slot or endpoint is not available
        itemInformation.visible = true
        itemSlotOptions.visible = false

        // confirm both endpoints are already selected
        if (slotIndex === -1 ||
            digishow.getSourceEndpointIndex(slotIndex) === -1 ||
            digishow.getDestinationEndpointIndex(slotIndex) === -1) return

        // confirm slot configuration is avalible
        var config = digishow.getSlotConfiguration(slotIndex)
        var slotInfo = config["slotInfo"]
        if (slotInfo === undefined) return

        slotInfoBackup = slotInfo

        // show slot options
        itemInformation.visible = false
        itemSlotOptions.visible = true

        // show / hide options according to input-output signals
        var inputSignal = slotInfo["inputSignal"]
        var outputSignal = slotInfo["outputSignal"]

        inputFullRange = slotInfo["inputRange"]
        outputFullRange = slotInfo["outputRange"]

        checkInputInverted.visible = false
        checkOutputInverted.visible = false
        checkOutputLowAsZero.visible = false

        spinOutputSmoothing.visible = false

        itemMappingOptions.visible = false
        itemEnvelopeOptions.visible = false

        if (inputSignal  === DigishowEnvironment.SignalAnalog &&
            outputSignal === DigishowEnvironment.SignalAnalog) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true
            checkOutputLowAsZero.visible = true

            spinOutputSmoothing.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = false

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            checkInputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true

            spinOutputSmoothing.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            checkInputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            checkOutputInverted.visible = true
            checkOutputLowAsZero.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

            itemEnvelopeOptions.visible = true
            buttonEnvelopeHelp.visible = true
            spinEnvelopeAttack.visible = true
            spinEnvelopeHold.visible = true
            spinEnvelopeDecay.visible = true
            spinEnvelopeSustain.visible = true
            spinEnvelopeRelease.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            checkOutputInverted.visible = true

            itemEnvelopeOptions.visible = true
            buttonEnvelopeHelp.visible = false
            spinEnvelopeAttack.visible = true
            spinEnvelopeHold.visible = true
            spinEnvelopeDecay.visible = false
            spinEnvelopeSustain.visible = false
            spinEnvelopeRelease.visible = true
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
                setSlotOption(key1, parseFloat(value1.toFixed(5)))
                setSlotOption(key2, parseFloat(value2.toFixed(5)))
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
        }
    }
}
