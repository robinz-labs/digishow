import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import DigiShow 1.0

import "components"

Popup {
    id: popupMoreOptions

    /*
    property alias checkOptInitialize:      checkOptInitialize
    property alias spinOptInitialA:         spinOptInitialA
    property alias spinOptInitialB:         spinOptInitialB
    property alias spinOptRangeSteps:       spinOptRangeSteps
    property alias spinOptRangeFrequency:   spinOptRangeFrequency
    property alias spinOptFilterLevel:      spinOptFilterLevel
    property alias spinOptSamplingInterval: spinOptSamplingInterval
    property alias buttonOptModePuPd:       buttonOptModePuPd
    property alias menuOptModePuPd:         menuOptModePuPd
    */

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

            height: 28
            anchors.left: parent.left
            anchors.leftMargin: 110
            padding: 0
            checked: false
            visible: enabled

            onClicked: {
                if (!checked) {
                    spinOptInitialA.value = 0
                    spinOptInitialB.value = 0
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 100
            value: 0
            stepSize: 10
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 1
            value: 0
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 10000000
            value: 1000
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 60000
            value: 1000
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 255
            value: 0
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

            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 110
            from: 0
            to: 60000
            value: 0
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

        this.open()
        common.runLater(showOptions) // force to refresh
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
        spinOptRangeSteps      .enabled = (enables["optRangeSteps"      ] === true)
        spinOptRangeFrequency  .enabled = (enables["optRangeFrequency"  ] === true)
        spinOptFilterLevel     .enabled = (enables["optFilterLevel"     ] === true)
        spinOptSamplingInterval.enabled = (enables["optSamplingInterval"] === true)
        buttonOptModePuPd      .enabled = (enables["optModePuPd"        ] === true)

        checkOptInitialize     .enabled = (spinOptInitialA.enabled || spinOptInitialB.enabled)
    }

    function resetOptions() {

        checkOptInitialize     .checked = false
        spinOptInitialA        .value = 0
        spinOptInitialB        .value = 0
        spinOptRangeSteps      .value = 1000
        spinOptRangeFrequency  .value = 1000
        spinOptFilterLevel     .value = 0
        spinOptSamplingInterval.value = 0
        menuOptModePuPd        .selectedIndex = 0
    }

    function setOptions(options) {

        function getOptionValue(optionKey, defaultValue) {
            if (options[optionKey] !== undefined) return options[optionKey];
            return defaultValue;
        }

        if (checkOptInitialize.enabled) {
            var initial = getOptionValue("initial", -1)
            checkOptInitialize.checked = (initial >= 0)
            if (spinOptInitialA.enabled) spinOptInitialA.value = Math.round(initial * 100)
            else
            if (spinOptInitialB.enabled) spinOptInitialB.value = (initial > 0.5 ? 1 : 0)
        }

        if (spinOptRangeSteps.enabled) spinOptRangeSteps.value = getOptionValue("range", 1000)
        else
        if (spinOptRangeFrequency.enabled) spinOptRangeFrequency.value = getOptionValue("range", 1000)

        if (spinOptFilterLevel.enabled) spinOptFilterLevel.value = getOptionValue("optFilterLevel", 0)
        if (spinOptSamplingInterval.enabled) spinOptSamplingInterval.value = getOptionValue("optSamplingInterval", 0)
        if (buttonOptModePuPd.enabled) menuOptModePuPd.selectOption(getOptionValue("optMode", 0))
    }

    function getOptions() {

        var options = {}

        if (checkOptInitialize.enabled) {
            if (checkOptInitialize.checked === false) options["initial"] = -1
            else
            if (spinOptInitialA.enabled) options["initial"] = spinOptInitialA.value * 0.01
            else
            if (spinOptInitialB.enabled) options["initial"] = spinOptInitialB.value
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

