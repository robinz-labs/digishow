import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import DigiShow 1.0

import "components"

Popup {
    id: popupMoreOptions

    property bool isDefault: false

    width: 280
    height: columnMoreOptions.height + 40
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

        id: columnMoreOptions

        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 16
        spacing: 10

        CheckBox {
            id: checkOptInitialize

            property bool defaultValue: false

            height: 28
            anchors.left: parent.left
            anchors.leftMargin: 110
            padding: 0
            visible: enabled


            onClicked: {
                if (!checked) {
                    spinOptInitialA.value = 0
                    spinOptInitialB.value = 0
                    spinOptInitialMidi.value = 0
                }
                isModified = true
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Need to Initialize")
            }
        }

        CSpinBox {
            id: spinOptInitialA

            property int defaultValue: 0

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 100
            stepSize: 5
            unit: "%"
            visible: enabled


            onValueModified: {
                checkOptInitialize.checked = true
                isModified = true
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Initial Value")
            }
        }

        CSpinBox {
            id: spinOptInitialB

            property int defaultValue: 0

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 1
            stepSize: 1
            unit: ""
            visible: enabled


            onValueModified: {
                checkOptInitialize.checked = true
                isModified = true
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Initial Value")
            }
        }

        CSpinBox {
            id: spinOptInitialMidi

            property int defaultValue: 0

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 127
            stepSize: 1
            unit: ""
            visible: enabled


            onValueModified: {
                checkOptInitialize.checked = true
                isModified = true
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Initial Value")
            }
        }

        CSpinBox {
            id: spinOptRangeSteps

            property int defaultValue: 1000

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 10000000
            stepSize: 100
            unit: ""
            visible: enabled


            onValueModified: isModified = true

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Value Range (steps)")
            }
        }

        CSpinBox {
            id: spinOptRangeFrequency

            property int defaultValue: 1000

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 60000
            stepSize: 100
            unit: "Hz"
            visible: enabled


            onValueModified: isModified = true

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Value Range (Freq.)")
            }
        }

        CSpinBox {
            id: spinOptFilterLevel

            property int defaultValue: 0

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 255
            stepSize: 1
            unit: ""
            visible: enabled


            onValueModified: isModified = true

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Smoothing Level")
            }
        }

        CSpinBox {
            id: spinOptSamplingInterval

            property int defaultValue: 0

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 60000
            stepSize: 100
            unit: "µs"
            visible: enabled


            onValueModified: isModified = true

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("Sampling Interval")
            }
        }

        COptionButton {
            id: buttonOptModePuPd

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            visible: enabled


            text: menuOptModePuPd.selectedItemText
            onClicked: menuOptModePuPd.showOptions()

            COptionMenu {
                id: menuOptModePuPd

                property int defaultValue: 0

                optionItems: [
                                { text: qsTr("Default"), value: 0 },
                                { text: qsTr("Internal Pull-Up"), value: 1 },
                                { text: qsTr("External PU/PD"), value: 2 }
                             ]
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                color: "#cccccc"
                font.pixelSize: 12
                text: qsTr("PU/PD Resistor")
            }
        }
    }

    function show() {

        // refresh options form height
        var height = 0
        for (var n=0; n<columnMoreOptions.children.length; n++) {
            var child = columnMoreOptions.children[n]
            if (child.enabled) height += child.height + columnMoreOptions.spacing
        }
        if (height > 0) height -= columnMoreOptions.spacing
        columnMoreOptions.height = height

        showOptions()
    }

    function showOptions() {

        var popup = popupMoreOptions
        var button = popup.parent
        var window = button.Window
        var bottomInWindow = window.height - parent.mapToItem(window.contentItem, 0, button.height).y
        if (bottomInWindow > popup.height + 10) {
            popup.transformOrigin = Popup.TopRight
            popup.x = -popup.width + button.width
            popup.y = button.height + 10
        } else {
            popup.transformOrigin = Popup.BottomRight
            popup.x = -popup.width + button.width
            popup.y = -popup.height - 10
        }

        popup.open()
    }

    function enableOptions(enables) {

        spinOptInitialA        .enabled = (enables["optInitialA"        ] === true)
        spinOptInitialB        .enabled = (enables["optInitialB"        ] === true)
        spinOptInitialMidi     .enabled = (enables["optInitialMidi"     ] === true)
        spinOptRangeSteps      .enabled = (enables["optRangeSteps"      ] === true)
        spinOptRangeFrequency  .enabled = (enables["optRangeFrequency"  ] === true)
        spinOptFilterLevel     .enabled = (enables["optFilterLevel"     ] === true)
        spinOptSamplingInterval.enabled = (enables["optSamplingInterval"] === true)
        buttonOptModePuPd      .enabled = (enables["optModePuPd"        ] === true)

        checkOptInitialize     .enabled = (spinOptInitialA.enabled || spinOptInitialB.enabled || spinOptInitialMidi.enabled)
    }

    function resetOptions() {

        checkOptInitialize     .checked = checkOptInitialize     .defaultValue
        spinOptInitialA        .value   = spinOptInitialA        .defaultValue
        spinOptInitialB        .value   = spinOptInitialB        .defaultValue
        spinOptInitialMidi     .value   = spinOptInitialMidi     .defaultValue
        spinOptRangeSteps      .value   = spinOptRangeSteps      .defaultValue
        spinOptRangeFrequency  .value   = spinOptRangeFrequency  .defaultValue
        spinOptFilterLevel     .value   = spinOptFilterLevel     .defaultValue
        spinOptSamplingInterval.value   = spinOptSamplingInterval.defaultValue

        menuOptModePuPd.selectOption(menuOptModePuPd.defaultValue)

        isDefault = true
    }

    function setOptions(options) {

        function getOptionValue(optionKey, defaultValue) {
            if (options[optionKey] !== undefined) return options[optionKey];
            return defaultValue;
        }

        resetOptions()

        if (checkOptInitialize.enabled) {
            var initial = getOptionValue("initial", -1)
            checkOptInitialize.checked = (initial >= 0)
            if (spinOptInitialA.enabled) spinOptInitialA.value = Math.round(initial * 100)
            else
            if (spinOptInitialB.enabled) spinOptInitialB.value = (initial > 0.5 ? 1 : 0)
            else
            if (spinOptInitialMidi.enabled) spinOptInitialMidi.value = Math.round(initial * 127)

            if (checkOptInitialize.checked !== checkOptInitialize.defaultValue) isDefault = false
        }

        if (spinOptRangeSteps.enabled) {
            spinOptRangeSteps.value = getOptionValue("range", spinOptRangeSteps.defaultValue)
            if (spinOptRangeSteps.value !== spinOptRangeSteps.defaultValue) isDefault = false
        }

        if (spinOptRangeFrequency.enabled) {
            spinOptRangeFrequency.value = getOptionValue("range", spinOptRangeFrequency.defaultValue)
            if (spinOptRangeFrequency.value !== spinOptRangeFrequency.defaultValue) isDefault = false
        }

        if (spinOptFilterLevel.enabled) {
            spinOptFilterLevel.value = getOptionValue("optFilterLevel", spinOptFilterLevel.defaultValue)
            if (spinOptFilterLevel.value !== spinOptFilterLevel.defaultValue) isDefault = false
        }

        if (spinOptSamplingInterval.enabled) {
            spinOptSamplingInterval.value = getOptionValue("optSamplingInterval", spinOptSamplingInterval.defaultValue)
            if (spinOptSamplingInterval.value !== spinOptSamplingInterval.defaultValue) isDefault = false
        }

        if (buttonOptModePuPd.enabled) {
            menuOptModePuPd.selectOption(getOptionValue("optMode", menuOptModePuPd.defaultValue))
            if (menuOptModePuPd.selectedItemValue !== menuOptModePuPd.defaultValue) isDefault = false
        }
    }

    function getOptions() {

        var options = {}

        if (checkOptInitialize.enabled) {
            if (checkOptInitialize.checked === false) options["initial"] = -1
            else
            if (spinOptInitialA.enabled) options["initial"] = spinOptInitialA.value * 0.01
            else
            if (spinOptInitialB.enabled) options["initial"] = spinOptInitialB.value
            else
            if (spinOptInitialMidi.enabled) options["initial"] = parseFloat((spinOptInitialMidi.value / 127).toFixed(5))
        }

        if (spinOptRangeSteps.enabled) options["range"] = spinOptRangeSteps.value
        else
        if (spinOptRangeFrequency.enabled) options["range"] = spinOptRangeFrequency.value

        if (spinOptFilterLevel.enabled) options["optFilterLevel"] = spinOptFilterLevel.value
        if (spinOptSamplingInterval.enabled) options["optSamplingInterval"] = spinOptSamplingInterval.value
        if (buttonOptModePuPd.enabled) options["optMode"] = menuOptModePuPd.selectedItemValue

        return options
    }

}

