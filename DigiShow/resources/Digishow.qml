import QtQml 2.12
import DigiShow 1.0

import "components/CColor.js" as CColor

DigishowEnvironment {

    //readonly property var signalType: ({ A: 65, B: 66, C: 67, N: 78 })

    function getEndpointColor(type, sig) {

        switch (type) {

            case DigishowEnvironment.EndpointMidiNote          : return CColor.Lime
            case DigishowEnvironment.EndpointMidiControl       : return CColor.Aqua
            case DigishowEnvironment.EndpointMidiProgram       : return CColor.Iris //Salmon
            case DigishowEnvironment.EndpointRiocDigitalIn     : return CColor.HotPink
            case DigishowEnvironment.EndpointRiocDigitalOut    : return CColor.HotPink
            case DigishowEnvironment.EndpointRiocAnalogIn      : return CColor.Ocean
            case DigishowEnvironment.EndpointRiocAnalogOut     : return CColor.Ocean
            case DigishowEnvironment.EndpointRiocPwmOut        : return CColor.Ocean
            case DigishowEnvironment.EndpointRiocPfmOut        : return CColor.Ocean
            case DigishowEnvironment.EndpointRiocEncoderIn     : return CColor.Ocean
            case DigishowEnvironment.EndpointRiocRudderOut     : return CColor.Ocean
            case DigishowEnvironment.EndpointModbusDiscreteIn  : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusCoilOut     : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusCoilIn      : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusRegisterIn  : return CColor.RobinEgg
            case DigishowEnvironment.EndpointModbusHoldingOut  : return CColor.RobinEgg
            case DigishowEnvironment.EndpointModbusHoldingIn   : return CColor.RobinEgg
            case DigishowEnvironment.EndpointHueLight          : return CColor.Aqua
            case DigishowEnvironment.EndpointDmxDimmer         : return CColor.Aqua
            case DigishowEnvironment.EndpointArtnetDimmer      : return CColor.Aqua
            case DigishowEnvironment.EndpointScreenLight       : return CColor.RobinEgg
            case DigishowEnvironment.EndpointScreenMedia       : return sig === DigishowEnvironment.SignalBinary ?
                                                                        CColor.HotPink : CColor.Iris
            case DigishowEnvironment.EndpointScreenCanvas      : return CColor.Ocean
            case DigishowEnvironment.EndpointPipeAnalog        : return CColor.Aqua
            case DigishowEnvironment.EndpointPipeBinary        : return CColor.RosePink
            case DigishowEnvironment.EndpointPipeNote          : return CColor.Mint

            case DigishowEnvironment.EndpointLaunchPreset      : return CColor.Cherry
        }

        return "#666666";
    }

    function getSignalIcon(sig) {

        if (sig === DigishowEnvironment.SignalAnalog) {

            return "qrc:///images/icon_signal_analog_white.png"

        } else if (sig === DigishowEnvironment.SignalBinary) {

            return "qrc:///images/icon_signal_binary_white.png"

        } else if (sig === DigishowEnvironment.SignalNote) {

            return "qrc:///images/icon_signal_note_white.png"
        }

        return ""
    }
}
