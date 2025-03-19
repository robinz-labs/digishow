import QtQml 2.12
import DigiShow 1.0

import "components/CColor.js" as CColor

DigishowEnvironment {

    //readonly property var signalType: ({ A: 65, B: 66, C: 67, N: 78 })

    function getEndpointColor(type, sig) {

        switch (type) {

            case DigishowEnvironment.EndpointMidiNote          : return CColor.Lime
            case DigishowEnvironment.EndpointMidiControl       : return CColor.Lake
            case DigishowEnvironment.EndpointMidiProgram       : return CColor.Iris
            case DigishowEnvironment.EndpointMidiPitch         : return CColor.Iris
            case DigishowEnvironment.EndpointMidiCcPulse       : return CColor.Iris
            case DigishowEnvironment.EndpointRiocDigitalIn     : return CColor.HotPink
            case DigishowEnvironment.EndpointRiocDigitalOut    : return CColor.HotPink
            case DigishowEnvironment.EndpointRiocAnalogIn      : return CColor.Royal
            case DigishowEnvironment.EndpointRiocAnalogOut     : return CColor.Royal
            case DigishowEnvironment.EndpointRiocPwmOut        : return CColor.Royal
            case DigishowEnvironment.EndpointRiocPfmOut        : return CColor.Royal
            case DigishowEnvironment.EndpointRiocEncoderIn     : return CColor.Royal
            case DigishowEnvironment.EndpointRiocRudderOut     : return CColor.Royal
            case DigishowEnvironment.EndpointRiocStepperOut    : return CColor.Royal
            case DigishowEnvironment.EndpointRiocUserChannel   : return CColor.Royal
            case DigishowEnvironment.EndpointModbusDiscreteIn  : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusCoilOut     : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusCoilIn      : return CColor.Cherry
            case DigishowEnvironment.EndpointModbusRegisterIn  : return CColor.Royal
            case DigishowEnvironment.EndpointModbusHoldingOut  : return CColor.Royal
            case DigishowEnvironment.EndpointModbusHoldingIn   : return CColor.Royal
            case DigishowEnvironment.EndpointHueLight          : return CColor.Ocean
            case DigishowEnvironment.EndpointHueGroup          : return CColor.Ocean
            case DigishowEnvironment.EndpointDmxDimmer         : return CColor.Ocean
            case DigishowEnvironment.EndpointDmxDimmer2        : return CColor.Ocean
            case DigishowEnvironment.EndpointDmxMedia          : return CColor.HotPink
            case DigishowEnvironment.EndpointDmxMaster         : return CColor.Royal
            case DigishowEnvironment.EndpointArtnetDimmer      : return CColor.Ocean
            case DigishowEnvironment.EndpointArtnetDimmer2     : return CColor.Ocean
            case DigishowEnvironment.EndpointArtnetMedia       : return CColor.HotPink
            case DigishowEnvironment.EndpointArtnetMaster      : return CColor.Royal
            case DigishowEnvironment.EndpointOscInt            : return CColor.Sky
            case DigishowEnvironment.EndpointOscFloat          : return CColor.Sky
            case DigishowEnvironment.EndpointOscBool           : return CColor.RosePink
            case DigishowEnvironment.EndpointAudioInLevel      : return CColor.Ocean
            case DigishowEnvironment.EndpointAudioInLevelDb    : return CColor.Ocean
            case DigishowEnvironment.EndpointAudioInPeak       : return CColor.Ocean
            case DigishowEnvironment.EndpointAudioInPeakDb     : return CColor.Ocean
            case DigishowEnvironment.EndpointAudioInSpectrum   : return CColor.Lake
            case DigishowEnvironment.EndpointScreenLight       : return CColor.Lake
            case DigishowEnvironment.EndpointScreenMedia       : return sig === DigishowEnvironment.SignalBinary ?
                                                                        CColor.HotPink : CColor.Iris
            case DigishowEnvironment.EndpointScreenCanvas      : return CColor.Ocean
            case DigishowEnvironment.EndpointAPlayMedia        : return CColor.HotPink
            case DigishowEnvironment.EndpointMPlayMedia        : return CColor.HotPink
            case DigishowEnvironment.EndpointPipeAnalog        : return CColor.Sky
            case DigishowEnvironment.EndpointPipeBinary        : return CColor.RosePink
            case DigishowEnvironment.EndpointPipeNote          : return CColor.Mint
            case DigishowEnvironment.EndpointLaunchPreset      : return CColor.Cherry
            case DigishowEnvironment.EndpointHotkeyPress       : return CColor.HotPink
            case DigishowEnvironment.EndpointMetronomeBeat     : return CColor.Mint
            case DigishowEnvironment.EndpointMetronomeBPM      : return CColor.Ocean
            case DigishowEnvironment.EndpointMetronomeQuantum  : return CColor.Ocean
            case DigishowEnvironment.EndpointMetronomeRun      : return CColor.Cherry
            case DigishowEnvironment.EndpointMetronomeLink     : return CColor.Cherry
            case DigishowEnvironment.EndpointMetronomeTap      : return CColor.Cherry
            case DigishowEnvironment.EndpointMessagerText      : return CColor.HotPink
            case DigishowEnvironment.EndpointMessagerHexCode   : return CColor.RosePink
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
