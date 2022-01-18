/*
    Copyright 2021 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#ifndef DIGISHOWENVIRONMENT_H
#define DIGISHOWENVIRONMENT_H

#include <QObject>
#include "RtMidi.h"
#include "digishow.h"

class DigishowEnvironment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DigishowApp *app READ app NOTIFY appChanged)
    Q_PROPERTY(bool needLogs READ needLogs WRITE setNeedLogs NOTIFY needLogsChanged)

public:

    enum InterfaceType {
        InterfaceMidi   = INTERFACE_MIDI,
        InterfaceRioc   = INTERFACE_RIOC,
        InterfaceModbus = INTERFACE_MODBUS,
        InterfaceHue    = INTERFACE_HUE,
        InterfaceDmx    = INTERFACE_DMX,
        InterfaceArtnet = INTERFACE_ARTNET,
        InterfaceOsc    = INTERFACE_OSC,
        InterfaceScreen = INTERFACE_SCREEN,
        InterfaceAPlay  = INTERFACE_APLAY,
        InterfaceMPlay  = INTERFACE_MPLAY,
        InterfacePipe   = INTERFACE_PIPE,
        InterfaceLaunch = INTERFACE_LAUNCH
    };
    Q_ENUM(InterfaceType)

    enum InterfaceMode {
        InterfaceMidiInput        = INTERFACE_MIDI_INPUT,
        InterfaceMidiOutput       = INTERFACE_MIDI_OUTPUT,
        InterfaceRiocGeneral      = INTERFACE_RIOC_GENERAL,
        InterfaceRiocArduinoUno   = INTERFACE_RIOC_ARDUINO_UNO,
        InterfaceRiocArduinoMega  = INTERFACE_RIOC_ARDUINO_MEGA,
        InterfaceRiocAladdin      = INTERFACE_RIOC_ALADDIN,
        InterfaceModbusRtu        = INTERFACE_MODBUS_RTU,
        InterfaceModbusTcp        = INTERFACE_MODBUS_TCP,
        InterfaceModbusRtuOverTcp = INTERFACE_MODBUS_RTUOVERTCP,
        InterfaceDmxEnttecPro     = INTERFACE_DMX_ENTTEC_PRO,
        InterfaceDmxEnttecOpen    = INTERFACE_DMX_ENTTEC_OPEN,
        InterfaceArtnetInput      = INTERFACE_ARTNET_INPUT,
        InterfaceArtnetOutput     = INTERFACE_ARTNET_OUTPUT,
        InterfaceOscInput         = INTERFACE_OSC_INPUT,
        InterfaceOscOutput        = INTERFACE_OSC_OUTPUT,
        InterfaceScreenLocal      = INTERFACE_SCREEN_LOCAL,
        InterfaceScreenRemote     = INTERFACE_SCREEN_REMOTE,
        InterfaceAPlayLocal       = INTERFACE_APLAY_LOCAL,
        InterfaceAPlayRemote      = INTERFACE_APLAY_REMOTE,
        InterfaceMPlayLocal       = INTERFACE_MPLAY_LOCAL,
        InterfaceMPlayRemote      = INTERFACE_MPLAY_REMOTE,
        InterfacePipeLocal        = INTERFACE_PIPE_LOCAL,
        InterfacePipeRemote       = INTERFACE_PIPE_REMOTE,
        InterfaceLaunchLocal      = INTERFACE_LAUNCH_LOCAL,
        InterfaceLaunchRemote     = INTERFACE_LAUNCH_REMOTE
    };
    Q_ENUM(InterfaceMode)

    enum EndpointType {
        EndpointMidiNote          = ENDPOINT_MIDI_NOTE,
        EndpointMidiControl       = ENDPOINT_MIDI_CONTROL,
        EndpointMidiProgram       = ENDPOINT_MIDI_PROGRAM,
        EndpointMidiCcPulse       = ENDPOINT_MIDI_CC_PULSE,
        EndpointRiocDigitalIn     = ENDPOINT_RIOC_DIGITAL_IN,
        EndpointRiocDigitalOut    = ENDPOINT_RIOC_DIGITAL_OUT,
        EndpointRiocAnalogIn      = ENDPOINT_RIOC_ANALOG_IN,
        EndpointRiocAnalogOut     = ENDPOINT_RIOC_ANALOG_OUT,
        EndpointRiocPwmOut        = ENDPOINT_RIOC_PWM_OUT,
        EndpointRiocPfmOut        = ENDPOINT_RIOC_PFM_OUT,
        EndpointRiocEncoderIn     = ENDPOINT_RIOC_ENCODER_IN,
        EndpointRiocRudderOut     = ENDPOINT_RIOC_RUDDER_OUT,
        EndpointRiocStepperOut    = ENDPOINT_RIOC_STEPPER_OUT,
        EndpointModbusDiscreteIn  = ENDPOINT_MODBUS_DISCRETE_IN,
        EndpointModbusCoilOut     = ENDPOINT_MODBUS_COIL_OUT,
        EndpointModbusCoilIn      = ENDPOINT_MODBUS_COIL_IN,
        EndpointModbusRegisterIn  = ENDPOINT_MODBUS_REGISTER_IN,
        EndpointModbusHoldingOut  = ENDPOINT_MODBUS_HOLDING_OUT,
        EndpointModbusHoldingIn   = ENDPOINT_MODBUS_HOLDING_IN,
        EndpointHueLight          = ENDPOINT_HUE_LIGHT,
        EndpointHueGroup          = ENDPOINT_HUE_GROUP,
        EndpointDmxDimmer         = ENDPOINT_DMX_DIMMER,
        EndpointArtnetDimmer      = ENDPOINT_ARTNET_DIMMER,
        EndpointOscInt            = ENDPOINT_OSC_INT,
        EndpointOscFloat          = ENDPOINT_OSC_FLOAT,
        EndpointOscBool           = ENDPOINT_OSC_BOOL,
        EndpointScreenLight       = ENDPOINT_SCREEN_LIGHT,
        EndpointScreenMedia       = ENDPOINT_SCREEN_MEDIA,
        EndpointScreenCanvas      = ENDPOINT_SCREEN_CANVAS,
        EndpointAPlayMedia        = ENDPOINT_APLAY_MEDIA,
        EndpointMPlayMedia        = ENDPOINT_MPLAY_MEDIA,
        EndpointPipeAnalog        = ENDPOINT_PIPE_ANALOG,
        EndpointPipeBinary        = ENDPOINT_PIPE_BINARY,
        EndpointPipeNote          = ENDPOINT_PIPE_NOTE,
        EndpointLaunchPreset      = ENDPOINT_LAUNCH_PRESET
    };
    Q_ENUM(EndpointType)

    enum SignalType {
        SignalAnalog = DATA_SIGNAL_ANALOG,
        SignalBinary = DATA_SIGNAL_BINARY,
        SignalNote   = DATA_SIGNAL_NOTE
    };
    Q_ENUM(SignalType)

    enum LightControl {
        ControlLightR   = CONTROL_LIGHT_R,
        ControlLightG   = CONTROL_LIGHT_G,
        ControlLightB   = CONTROL_LIGHT_B,
        ControlLightW   = CONTROL_LIGHT_W,
        ControlLightHue = CONTROL_LIGHT_HUE,
        ControlLightSat = CONTROL_LIGHT_SAT,
        ControlLightBri = CONTROL_LIGHT_BRI,
        ControlLightCt  = CONTROL_LIGHT_CT
    };
    Q_ENUM(LightControl)

    enum MediaControl {
        ControlMediaStart    = CONTROL_MEDIA_START,
        ControlMediaStop     = CONTROL_MEDIA_STOP,
        ControlMediaStopAll  = CONTROL_MEDIA_STOP_ALL,

        ControlMediaOpacity  = CONTROL_MEDIA_OPACITY,
        ControlMediaScale    = CONTROL_MEDIA_SCALE,
        ControlMediaRotation = CONTROL_MEDIA_ROTATION,
        ControlMediaXOffset  = CONTROL_MEDIA_XOFFSET,
        ControlMediaYOffset  = CONTROL_MEDIA_YOFFSET
    };
    Q_ENUM(MediaControl)

    explicit DigishowEnvironment(QObject *parent = nullptr);

    Q_INVOKABLE static QString appName()    { return g_appname; }
    Q_INVOKABLE static QString appVersion() { return g_version; }
    Q_INVOKABLE static QString appDataPath(const QString &filename = QString());

    Q_INVOKABLE static QString appBuildDate() { return __DATE__; }
    Q_INVOKABLE static QString appQtVersion() { return QT_VERSION_STR; }
    Q_INVOKABLE static QString appRtMidiVersion() { return RTMIDI_VERSION; }

    Q_INVOKABLE static bool appExperimental() {
#ifdef DIGISHOW_EXPERIMENTAL
        return true;   // this app is a version with experimental features
#else
        return false;  // this app is a standard version
#endif
    }

    Q_INVOKABLE DigishowApp *app() { return g_app; }

    Q_INVOKABLE void setAppOptions(const QVariantMap &options);
    Q_INVOKABLE QVariantMap getAppOptions();

    Q_INVOKABLE bool needLogs();
    Q_INVOKABLE void setNeedLogs(bool need);

    Q_INVOKABLE QVariantMap getSlotConfiguration(int slotIndex);
    Q_INVOKABLE QVariantMap getSlotRuntimeData(int slotIndex);
    Q_INVOKABLE QVariantMap getInterfaceConfiguration(int interfaceIndex);

    Q_INVOKABLE int getSourceInterfaceIndex(int slotIndex);
    Q_INVOKABLE int getSourceEndpointIndex(int slotIndex);
    Q_INVOKABLE int getDestinationInterfaceIndex(int slotIndex);
    Q_INVOKABLE int getDestinationEndpointIndex(int slotIndex);

    Q_INVOKABLE void clearSourceEndpoint(int slotIndex);
    Q_INVOKABLE void clearDestinationEndpoint(int slotIndex);
    Q_INVOKABLE int updateSourceEndpoint(int slotIndex, int interfaceIndex, const QVariantMap &endpointOptions);
    Q_INVOKABLE int updateDestinationEndpoint(int slotIndex, int interfaceIndex, const QVariantMap &endpointOptions);

    Q_INVOKABLE bool restartInterface(int interfaceIndex);

    Q_INVOKABLE int makeInterface(const QVariantMap &interfaceOptions);
    Q_INVOKABLE int findInterfaceWithName(const QString &interfaceName);
    Q_INVOKABLE QString getInterfaceName(int interfaceIndex);
    Q_INVOKABLE QVariantMap getInterfaceOptions(int interfaceIndex);

    Q_INVOKABLE int makeEndpoint(int interfaceIndex, const QVariantMap &endpointOptions);
    Q_INVOKABLE int findEndpointWithName(int interfaceIndex, const QString &endpointName);
    Q_INVOKABLE QString getEndpointName(int interfaceIndex, int endpointIndex);
    Q_INVOKABLE QVariantMap getEndpointOptions(int interfaceIndex, int endpointIndex);

    Q_INVOKABLE int makeMedia(int interfaceIndex, const QString &mediaUrl, const QString &mediaType);
    Q_INVOKABLE int findMediaWithName(int interfaceIndex, const QString &mediaName);
    Q_INVOKABLE QString getMediaName(int interfaceIndex, int mediaIndex);
    Q_INVOKABLE QVariantMap getMediaOptions(int interfaceIndex, int mediaIndex);

    Q_INVOKABLE int makeSlot(const QVariantMap &slotOptions,
                             const QVariantMap &sourceInterfaceOptions, const QVariantMap &sourceEndpointOptions,
                             const QVariantMap &destinationInterfaceOptions, const QVariantMap &destinationEndpointOptions,
                             const QString &mediaUrl = QString(), const QString &mediaType = QString());

    Q_INVOKABLE static QVariantMap listOnline();

    Q_INVOKABLE static QString getLightControlName(int control, bool shortName = false);
    Q_INVOKABLE static QString getMediaControlName(int control, bool forScreen = false);
    Q_INVOKABLE static QString getMidiControlName(int control);
    Q_INVOKABLE static QString getMidiNoteName(int noteNumber, bool longName = false);
    Q_INVOKABLE static QString getRiocPinName(int mode, int pinNumber);

    Q_INVOKABLE static QString getScreenMediaType(const QString &mediaUrl);

signals:
    void appChanged();
    void needLogsChanged();

public slots:
};

#endif // DIGISHOWENVIRONMENT_H
