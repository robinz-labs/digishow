import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: slotOptionsView

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
            font.pixelSize: 12
            text: qsTr("Zero Output While Reach Lower")
            enabled:
                sliderMappingInputRange.visible && sliderMappingOutputRange.visible &&
                sliderMappingOutputRange.first.value > 0
            opacity: enabled ? 1 : 0

            // Behavior on opacity { NumberAnimation { duration: 200 } }

            onClicked: setSlotOption("outputLowAsZero", checked)
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
                    anchors.topMargin: 40
                    spacing: 80

                    CSlider {
                        id: sliderMappingInputThreshold

                        anchors.left: parent.left
                        anchors.right: parent.right
                        value: 0.5
                        color: Material.accent

                        onMoved: setSlotOption("inputThreshold", parseFloat(value.toFixed(2)))

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Input Threshold")
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.value*100) + " %"
                        }
                    }

                    CRangeSlider {
                        id: sliderMappingInputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("inputLow", parseFloat(first.value.toFixed(2)))
                        second.onMoved: setSlotOption("inputHigh", parseFloat(second.value.toFixed(2)))

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Input Range")
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " % - " +
                                  Math.round(parent.second.value*100) + " %"
                        }
                    }

                    CRangeSlider {
                        id: sliderMappingOutputRange

                        anchors.left: parent.left
                        anchors.right: parent.right
                        first.value: 0
                        second.value: 1.0
                        color: Material.accent

                        first.onMoved: setSlotOption("outputLow", parseFloat(first.value.toFixed(2)))
                        second.onMoved: setSlotOption("outputHigh", parseFloat(second.value.toFixed(2)))

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: qsTr("Output Range")
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.bottom
                            anchors.topMargin: 20
                            color: "#cccccc"
                            font.pixelSize: 12
                            text: Math.round(parent.first.value*100) + " % - " +
                                  Math.round(parent.second.value*100) + " %"
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

        // show slot options
        itemInformation.visible = false
        itemSlotOptions.visible = true

        // show / hide options according to input-output signals
        var inputSignal = slotInfo["inputSignal"]
        var outputSignal = slotInfo["outputSignal"]

        checkInputInverted.visible = false
        checkOutputInverted.visible = false
        checkOutputLowAsZero.visible = false

        itemMappingOptions.visible = false
        itemEnvelopeOptions.visible = false

        if (inputSignal  === DigishowEnvironment.SignalAnalog &&
            outputSignal === DigishowEnvironment.SignalAnalog) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true
            checkOutputLowAsZero.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputThreshold.visible = false
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputThreshold.visible = true
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = false

        } else if (inputSignal  === DigishowEnvironment.SignalAnalog &&
                   outputSignal === DigishowEnvironment.SignalNote) {

            checkInputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputThreshold.visible = false
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalBinary &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            checkInputInverted.visible = true
            checkOutputInverted.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputThreshold.visible = false
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
            sliderMappingInputThreshold.visible = false
            sliderMappingInputRange.visible = false
            sliderMappingOutputRange.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalAnalog) {

            checkOutputInverted.visible = true
            checkOutputLowAsZero.visible = true

            itemMappingOptions.visible = true
            sliderMappingInputThreshold.visible = false
            sliderMappingInputRange.visible = true
            sliderMappingOutputRange.visible = true

            itemEnvelopeOptions.visible = true
            spinEnvelopeAttack.visible = true
            spinEnvelopeHold.visible = true
            spinEnvelopeDecay.visible = true
            spinEnvelopeSustain.visible = true
            spinEnvelopeRelease.visible = true

        } else if (inputSignal  === DigishowEnvironment.SignalNote &&
                   outputSignal === DigishowEnvironment.SignalBinary) {

            checkOutputInverted.visible = true

            itemEnvelopeOptions.visible = true
            spinEnvelopeAttack.visible = true
            spinEnvelopeHold.visible = true
            spinEnvelopeDecay.visible = false
            spinEnvelopeSustain.visible = false
            spinEnvelopeRelease.visible = true
        }

        // reset unused options
        if (!checkInputInverted.visible) setSlotOption("inputInverted", false)
        if (!checkOutputInverted.visible) setSlotOption("outputInverted", false)
        if (!checkOutputLowAsZero.visible) setSlotOption("outputLowAsZero", false)

        // refresh option values
        checkInputInverted.checked = slotInfo["inputInverted"]
        checkOutputInverted.checked = slotInfo["outputInverted"]
        checkOutputLowAsZero.checked = slotInfo["outputLowAsZero"]

        sliderMappingInputThreshold.value = slotInfo["inputThreshold"]
        sliderMappingInputRange.first.value = slotInfo["inputLow"]
        sliderMappingInputRange.second.value = slotInfo["inputHigh"]
        sliderMappingOutputRange.first.value = slotInfo["outputLow"]
        sliderMappingOutputRange.second.value = slotInfo["outputHigh"]

        spinEnvelopeAttack.value = slotInfo["envelopeAttack"]
        spinEnvelopeHold.value = slotInfo["envelopeHold"]
        spinEnvelopeDecay.value = slotInfo["envelopeDecay"]
        spinEnvelopeSustain.value = Math.round(slotInfo["envelopeSustain"]*100)
        spinEnvelopeRelease.value = slotInfo["envelopeRelease"]
    }

    function setSlotOption(key, value) {
        if (slotIndex !== -1) {
            app.slotAt(slotIndex).setSlotOption(key, value)
            slotOptionUpdated(key, value) // emit signal
        }
    }
}
