import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemLFO

    COptionButton {
        id: buttonChannel
        width: 100
        height: 28
        anchors.left: parent.right
        anchors.top: parent.top
        text: qsTr("LFO") + " " + spinChannel.value
        onClicked: spinChannel.visible = true

        COptionSpinBox {
            id: spinChannel
        }
    }

    COptionButton {
        id: buttonType
        width: forOutput ? 200 : 100
        height: 28
        anchors.left: buttonChannel.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: refreshMoreOptions()
        }
    }

    Text {
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.verticalCenter: buttonType.verticalCenter
        font.pixelSize: 12
        font.bold: false
        text: {
            switch (menuType.selectedItemValue) {
            case DigishowEnvironment.EndpointLfoTime:
            case DigishowEnvironment.EndpointLfoPeriod:
            case DigishowEnvironment.EndpointLfoSample:
                return qsTr("ms")
            case DigishowEnvironment.EndpointLfoPhase:
            case DigishowEnvironment.EndpointLfoAmplitude:
                return qsTr("%")
            }
            return ""
        }
        color: "#666"
    }

    COptionButton {
        id: buttonFunction
        width: 100
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuFunction.selectedItemText
        onClicked: menuFunction.showOptions()

        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointLfoValue

        COptionMenu {
            id: menuFunction
        }
    }

    function refresh() {

        var items
        var n

        // init LFO type menu
        if (menuType.count === 0) {
            items = []

            if  (forInput) {

                items.push({ text: qsTr("Value"                 ), value: DigishowEnvironment.EndpointLfoValue,     tag: "value" })
                items.push({ text: qsTr("Time"                  ), value: DigishowEnvironment.EndpointLfoTime,      tag: "time" })

            } else if (forOutput) {

                items.push({ text: qsTr("Reset"                 ), value: DigishowEnvironment.EndpointLfoReset,     tag: "reset" })
                items.push({ text: qsTr("Run ON"                ), value: DigishowEnvironment.EndpointLfoRun,       tag: "run" })
                items.push({ text: qsTr("Period Change"         ), value: DigishowEnvironment.EndpointLfoPeriod,    tag: "period" })
                items.push({ text: qsTr("Phase Change"          ), value: DigishowEnvironment.EndpointLfoPhase,     tag: "phase" })
                items.push({ text: qsTr("Amplitude Change"      ), value: DigishowEnvironment.EndpointLfoAmplitude, tag: "amplitude" })
                items.push({ text: qsTr("Sample Interval Change"), value: DigishowEnvironment.EndpointLfoSample,    tag: "sample" })
            }

            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init LFO function menu
        if (menuFunction.count === 0) {
            items = []
            items.push({ text: qsTr("None"    ), value: DigishowLFO.LfoNone,     tag: "" })
            items.push({ text: qsTr("Sine"    ), value: DigishowLFO.LfoSine,     tag: "sine" })
            items.push({ text: qsTr("Square"  ), value: DigishowLFO.LfoSquare,   tag: "square" })
            items.push({ text: qsTr("Sawtooth"), value: DigishowLFO.LfoSawtooth, tag: "sawtooth" })
            items.push({ text: qsTr("Triangle"), value: DigishowLFO.LfoTriangle, tag: "triangle" })
            items.push({ text: qsTr("Random"  ), value: DigishowLFO.LfoRandom,   tag: "random" })

            menuFunction.optionItems = items
            menuFunction.selectedIndex = 1
        }

        // init LFO channel option spinbox
        spinChannel.from = 1
        spinChannel.to = 9999
        spinChannel.visible = false

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointLfoValue) {
            enables["optRange"] = true
            enables["optRun"] = true
            enables["optPeriod"] = true
            enables["optPhase"] = true
            enables["optAmplitude"] = true
            enables["optSample"] = true
        } else if (endpointType === DigishowEnvironment.EndpointLfoTime) {
            enables["optRangeMSec"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        var type = endpointInfo["type"]
        menuType.selectOption(type)
        spinChannel.value = endpointInfo["channel"]

        if (type === DigishowEnvironment.EndpointLfoValue) {
            menuFunction.selectOptionWithTag(endpointOptions["optFunction"])
        }
    }

    function getEndpointOptions() {

        var options = {}

        var type = menuType.selectedItemValue
        options["type"] = menuType.selectedItemTag
        options["channel"] = spinChannel.value

        if (type === DigishowEnvironment.EndpointLfoValue) {
            options["optFunction"] = menuFunction.selectedItemTag
        }

        return options
    }
}

