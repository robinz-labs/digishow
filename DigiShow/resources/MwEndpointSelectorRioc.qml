import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemRioc

    COptionButton {
        id: buttonUnit
        width: 80
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Unit") + " " + spinUnit.value
        onClicked: spinUnit.visible = true

        COptionSpinBox {
            id: spinUnit
        }
    }

    COptionButton {
        id: buttonType
        width: 120
        height: 28
        anchors.left: buttonUnit.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: {
                refreshMenuChannel()
                refreshMoreOptions()
            }
        }
    }

    COptionButton {
        id: buttonChannel
        width: 80
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuChannel.selectedItemText
        onClicked: menuChannel.showOptions()

        COptionMenu {
            id: menuChannel
        }
    }

    function refresh() {
        var items
        var n

        // init rioc unit option spinbox
        spinUnit.from = 1
        spinUnit.to = 255
        spinUnit.visible = false

        // init rioc type option menu
        if (menuType.count === 0) {
            items = []
            if (forInput ) items.push({ text: qsTr("Digital In"),     value: DigishowEnvironment.EndpointRiocDigitalIn,  tag: "digital_in"  })
            if (forOutput) items.push({ text: qsTr("Digital Out"),    value: DigishowEnvironment.EndpointRiocDigitalOut, tag: "digital_out" })
            if (forInput ) items.push({ text: qsTr("Analog In"),      value: DigishowEnvironment.EndpointRiocAnalogIn,   tag: "analog_in"   })
          //if (forOutput) items.push({ text: qsTr("Analog Out"),     value: DigishowEnvironment.EndpointRiocAnalogOut,  tag: "analog_out"  })
            if (forOutput) items.push({ text: qsTr("PWM Out"),        value: DigishowEnvironment.EndpointRiocPwmOut,     tag: "pwm_out"     })
            if (forOutput) items.push({ text: qsTr("Frequency Out"),  value: DigishowEnvironment.EndpointRiocPfmOut,     tag: "pfm_out"     })
            if (forOutput) items.push({ text: qsTr("Servo"),          value: DigishowEnvironment.EndpointRiocRudderOut,  tag: "rudder_out"  })
            if (forOutput) items.push({ text: qsTr("Stepper"),        value: DigishowEnvironment.EndpointRiocStepperOut, tag: "stepper_out" })
            if (forInput ) items.push({ text: qsTr("Encoder"),        value: DigishowEnvironment.EndpointRiocEncoderIn,  tag: "encoder_in"  })

            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init rioc channel option menu
        if (menuChannel.count === 0) {
            refreshMenuChannel()
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMenuChannel() {

        var items = []
        var n
        var name

        var interfaceMode = menuInterface.getSelectedInterfaceConfiguration()["interfaceInfo"]["mode"]
        var endpointType = menuType.selectedItemValue
        if (interfaceMode === undefined || interfaceMode === DigishowEnvironment.InterfaceRiocGeneral) {
            for (n=0 ; n<128 ; n++)
                items.push({ text: qsTr("Pin") + " " + n, value: n })
        } else if (interfaceMode === DigishowEnvironment.InterfaceRiocArduinoUno) {
            for (n=2 ; n<22 ; n++) {
                name = digishow.getRiocPinName(interfaceMode, n)
                if ((name.startsWith("A") && endpointType===DigishowEnvironment.EndpointRiocAnalogIn) ||
                    (name.startsWith("D") && endpointType!==DigishowEnvironment.EndpointRiocAnalogIn))
                    items.push({ text: name, value: n })
            }
        } else if (interfaceMode === DigishowEnvironment.InterfaceRiocArduinoMega) {
            for (n=2 ; n<70 ; n++) if (n<14 || n>19) {
                name = digishow.getRiocPinName(interfaceMode, n)
                if ((name.startsWith("A") && endpointType===DigishowEnvironment.EndpointRiocAnalogIn) ||
                    (name.startsWith("D") && endpointType!==DigishowEnvironment.EndpointRiocAnalogIn))
                    items.push({ text: name, value: n })
            }
        } else if (interfaceMode === DigishowEnvironment.InterfaceRiocAladdin ||
                   interfaceMode === DigishowEnvironment.InterfaceRiocPlc1 ||
                   interfaceMode === DigishowEnvironment.InterfaceRiocPlc2 ) {

            var pins = digishow.getRiocPinList(interfaceMode)
            for (var i=0 ; i<pins.length ; i++) {
                n = pins[i]
                name = digishow.getRiocPinName(interfaceMode, n)
                if (((name.startsWith("DI") || name.startsWith("X")) && (
                     endpointType===DigishowEnvironment.EndpointRiocDigitalIn ||
                     endpointType===DigishowEnvironment.EndpointRiocEncoderIn
                    )) ||
                    ((name.startsWith("DO") || name.startsWith("Y")) && (
                     endpointType===DigishowEnvironment.EndpointRiocDigitalOut ||
                     endpointType===DigishowEnvironment.EndpointRiocPwmOut ||
                     endpointType===DigishowEnvironment.EndpointRiocPfmOut ||
                     endpointType===DigishowEnvironment.EndpointRiocRudderOut ||
                     endpointType===DigishowEnvironment.EndpointRiocStepperOut
                    )) ||
                    (name.startsWith("ADC") &&
                     endpointType===DigishowEnvironment.EndpointRiocAnalogIn
                    ) ||
                    (name.startsWith("LED") &&
                     endpointType===DigishowEnvironment.EndpointRiocDigitalOut
                    ) ||
                    (name.startsWith("SW") &&
                     endpointType===DigishowEnvironment.EndpointRiocDigitalIn
                    )
                ) items.push({ text: name, value: n })
            }
        }
        menuChannel.optionItems = items
        menuChannel.selectedIndex = (items.length > 0 ? 0 : -1)

    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var enables = {}

        if (endpointType !== DigishowEnvironment.EndpointRiocDigitalIn &&
            endpointType !== DigishowEnvironment.EndpointRiocAnalogIn) {

            if (endpointType === DigishowEnvironment.EndpointRiocDigitalOut) {
                enables["optInitialB"] = true
            } else {
                enables["optInitialA"] = true
            }
        }

        if (endpointType === DigishowEnvironment.EndpointRiocPfmOut) {
            enables["optRangeFrequency"] = true
        }
        if (endpointType === DigishowEnvironment.EndpointRiocEncoderIn) {
            enables["optRangeSteps"] = true
        }

        if (endpointType === DigishowEnvironment.EndpointRiocDigitalIn ||
            endpointType === DigishowEnvironment.EndpointRiocEncoderIn) {
            enables["optModePuPd"] = true
        }

        if (endpointType === DigishowEnvironment.EndpointRiocDigitalIn ||
            endpointType === DigishowEnvironment.EndpointRiocAnalogIn) {
            enables["optFilterLevel"] = true
        }

        if (endpointType === DigishowEnvironment.EndpointRiocDigitalIn ||
            endpointType === DigishowEnvironment.EndpointRiocAnalogIn ||
            endpointType === DigishowEnvironment.EndpointRiocEncoderIn) {
            enables["optSamplingInterval"] = true
        }

        if (endpointType === DigishowEnvironment.EndpointRiocStepperOut) {
            enables["optRangeSteps"] = true
            enables["optSpeed"] = true
            enables["optPosition"] = true
            enables["optModeStepper"] = true
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        spinUnit.value = endpointInfo["unit"]
        menuType.selectOption(endpointInfo["type"])
        menuChannel.selectOption(endpointInfo["channel"])
    }

    function getEndpointOptions() {

        var options = {}
        options["unit"] = spinUnit.value
        options["type"] = menuType.selectedItemTag
        options["channel"] = menuChannel.selectedItemValue

        return options
    }

}
