import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemRioc

    property alias spinUnit:    spinRiocUnit
    property alias menuType:    menuRiocType
    property alias menuChannel: menuRiocChannel

    COptionButton {
        id: buttonRiocUnit
        width: 80
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: qsTr("Unit") + " " + spinRiocUnit.value
        onClicked: spinRiocUnit.visible = true

        COptionSpinBox {
            id: spinRiocUnit
        }
    }

    COptionButton {
        id: buttonRiocType
        width: 120
        height: 28
        anchors.left: buttonRiocUnit.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuRiocType.selectedItemText
        onClicked: menuRiocType.showOptions()

        COptionMenu {
            id: menuRiocType

            onOptionSelected: {
                refreshMenuChannel()
                refreshMoreOptions()
            }
        }
    }

    COptionButton {
        id: buttonRiocChannel
        width: 80
        height: 28
        anchors.left: buttonRiocType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuRiocChannel.selectedItemText
        onClicked: menuRiocChannel.showOptions()

        COptionMenu {
            id: menuRiocChannel
        }
    }

    function refresh() {
        var items
        var n

        // init rioc unit option spinbox
        spinRiocUnit.from = 1
        spinRiocUnit.to = 255
        spinRiocUnit.visible = false

        // init rioc type option menu
        if (menuRiocType.count === 0) {
            items = []
            if (forInput ) items.push({ text: qsTr("Digital In"),     value: DigishowEnvironment.EndpointRiocDigitalIn,  tag: "digital_in"  })
            if (forOutput) items.push({ text: qsTr("Digital Out"),    value: DigishowEnvironment.EndpointRiocDigitalOut, tag: "digital_out" })
            if (forInput ) items.push({ text: qsTr("Analog In"),      value: DigishowEnvironment.EndpointRiocAnalogIn,   tag: "analog_in"   })
          //if (forOutput) items.push({ text: qsTr("Analog Out"),     value: DigishowEnvironment.EndpointRiocAnalogOut,  tag: "analog_out"  })
            if (forOutput) items.push({ text: qsTr("PWM Out"),        value: DigishowEnvironment.EndpointRiocPwmOut,     tag: "pwm_out"     })
            if (forOutput) items.push({ text: qsTr("PFM Out / Tone"), value: DigishowEnvironment.EndpointRiocPfmOut,     tag: "pfm_out"     })
            if (forOutput) items.push({ text: qsTr("Servo"),          value: DigishowEnvironment.EndpointRiocRudderOut,  tag: "rudder_out"  })

            if (digishow.appExperimental()) {
            if (forOutput) items.push({ text: qsTr("Stepper"),        value: DigishowEnvironment.EndpointRiocStepperOut, tag: "stepper_out" })
            if (forInput ) items.push({ text: qsTr("Encoder"),        value: DigishowEnvironment.EndpointRiocEncoderIn,  tag: "encoder_in"  })
            }

            menuRiocType.optionItems = items
            menuRiocType.selectedIndex = 0
        }

        // init rioc channel option menu
        if (menuRiocChannel.count === 0) {
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
        var endpointType = menuRiocType.selectedItemValue
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
        } else if (interfaceMode === DigishowEnvironment.InterfaceRiocAladdin) {
            for (n=2 ; n<70 ; n++) {
                name = digishow.getRiocPinName(interfaceMode, n)
                if ((name.startsWith("DI") && (
                     endpointType===DigishowEnvironment.EndpointRiocDigitalIn ||
                     endpointType===DigishowEnvironment.EndpointRiocEncoderIn
                    )) ||
                    (name.startsWith("DO") && (
                     endpointType===DigishowEnvironment.EndpointRiocDigitalOut ||
                     endpointType===DigishowEnvironment.EndpointRiocPwmOut ||
                     endpointType===DigishowEnvironment.EndpointRiocPfmOut ||
                     endpointType===DigishowEnvironment.EndpointRiocRudderOut ||
                     endpointType===DigishowEnvironment.EndpointRiocStepperOut
                    )) ||
                    (name.startsWith("ADC") &&
                     endpointType===DigishowEnvironment.EndpointRiocAnalogIn
                    )
                ) items.push({ text: name, value: n })
            }
        }
        menuRiocChannel.optionItems = items
        menuRiocChannel.selectedIndex = 0

    }

    function refreshMoreOptions() {

        var endpointType = menuRiocType.selectedItemValue
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

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }
}
