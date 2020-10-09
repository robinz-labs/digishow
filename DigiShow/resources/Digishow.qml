import QtQml 2.12
import DigiShow 1.0

DigishowEnvironment {

    //readonly property var signalType: ({ A: 65, B: 66, C: 67, N: 78 })

    function getEndpointColor(type, sig) {

        var colorHotPink  = "#ed1b74"
        var colorRosePink = "#f386ad"
        var colorCherry   = "#ee364e"
        var colorSalmon   = "#f7a08d"
        var colorMarigold = "#eae613"
        var colorOrange   = "#ff9900"
        var colorMint     = "#a0d28b"
        var colorLime     = "#00a652"
        var colorRobinEgg = "#18b8b6"
        var colorAqua     = "#44baec"
        var colorOcean    = "#1177dd"
        var colorRoyal    = "#2b459c"
        var colorIris     = "#575aa9"

        switch (type) {

            case DigishowEnvironment.EndpointMidiNote          : return colorLime
            case DigishowEnvironment.EndpointMidiControl       : return colorAqua
            case DigishowEnvironment.EndpointMidiProgram       : return colorIris //colorSalmon
            case DigishowEnvironment.EndpointRiocDigitalIn     : return colorHotPink
            case DigishowEnvironment.EndpointRiocDigitalOut    : return colorHotPink
            case DigishowEnvironment.EndpointRiocAnalogIn      : return colorOcean
            case DigishowEnvironment.EndpointRiocAnalogOut     : return colorOcean
            case DigishowEnvironment.EndpointRiocPwmOut        : return colorOcean
            case DigishowEnvironment.EndpointRiocPfmOut        : return colorOcean
            case DigishowEnvironment.EndpointRiocEncoderIn     : return colorOcean
            case DigishowEnvironment.EndpointRiocRudderOut     : return colorOcean
            case DigishowEnvironment.EndpointModbusDiscreteIn  : return colorCherry
            case DigishowEnvironment.EndpointModbusCoilOut     : return colorCherry
            case DigishowEnvironment.EndpointModbusCoilIn      : return colorCherry
            case DigishowEnvironment.EndpointModbusRegisterIn  : return colorRobinEgg
            case DigishowEnvironment.EndpointModbusHoldingOut  : return colorRobinEgg
            case DigishowEnvironment.EndpointModbusHoldingIn   : return colorRobinEgg
            case DigishowEnvironment.EndpointHueLight          : return colorAqua
            case DigishowEnvironment.EndpointDmxDimmer         : return colorAqua
            case DigishowEnvironment.EndpointArtnetDimmer      : return colorAqua
            case DigishowEnvironment.EndpointScreenLight       : return colorRobinEgg
            case DigishowEnvironment.EndpointScreenMedia       : return sig === DigishowEnvironment.SignalBinary ?
                                                                        colorHotPink : colorIris
            case DigishowEnvironment.EndpointPipeAnalog        : return colorAqua
            case DigishowEnvironment.EndpointPipeBinary        : return colorRosePink
            case DigishowEnvironment.EndpointPipeNote          : return colorMint
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
