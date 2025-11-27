/*
    Copyright 2021-2025 Robin Zhang & Labs

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

#ifndef DIGISHOW_COMMON_H
#define DIGISHOW_COMMON_H

#include <QtCore>

// function errors
#define ERR_NONE               0 // success
#define ERR_INVALID_DATA      -1
#define ERR_INVALID_INDEX     -2
#define ERR_INVALID_OPTION    -3
#define ERR_DEVICE_NOT_READY  -4
#define ERR_DEVICE_BUSY       -5
#define ERR_DEVICE_FAILURE    -6
#define ERR_UNDEFINED         -9

// controller interface type id
#define INTERFACE_MIDI         101
#define INTERFACE_RIOC         102
#define INTERFACE_MODBUS       103
#define INTERFACE_HUE          104
#define INTERFACE_DMX          105
#define INTERFACE_ARTNET       106
#define INTERFACE_OSC          107
#define INTERFACE_AUDIOIN      108
#define INTERFACE_SCREEN       801
#define INTERFACE_APLAY        802
#define INTERFACE_MPLAY        803
#define INTERFACE_PIPE         901
#define INTERFACE_LAUNCH       902
#define INTERFACE_HOTKEY       903
#define INTERFACE_METRONOME    904
#define INTERFACE_MESSENGER    905

// controller interface mode id
#define INTERFACE_MIDI_INPUT         10101
#define INTERFACE_MIDI_OUTPUT        10102
#define INTERFACE_RIOC_GENERAL       10201
#define INTERFACE_RIOC_ARDUINO_UNO   10202
#define INTERFACE_RIOC_ARDUINO_MEGA  10203
#define INTERFACE_RIOC_ARDUINO_MICRO 10204
#define INTERFACE_RIOC_ALADDIN       10211
#define INTERFACE_RIOC_PLC1          10212
#define INTERFACE_RIOC_PLC2          10213
#define INTERFACE_RIOC_ALADDIN1      10214
#define INTERFACE_RIOC_ALADDIN2      10215
#define INTERFACE_RIOC_ALADDIN3      10216
#define INTERFACE_MODBUS_RTU         10301
#define INTERFACE_MODBUS_TCP         10302
#define INTERFACE_MODBUS_RTUOVERTCP  10303
#define INTERFACE_HUE_DEFAULT        10401
#define INTERFACE_DMX_ENTTEC_PRO     10501
#define INTERFACE_DMX_ENTTEC_OPEN    10502
#define INTERFACE_ARTNET_INPUT       10601
#define INTERFACE_ARTNET_OUTPUT      10602
#define INTERFACE_OSC_INPUT          10701
#define INTERFACE_OSC_OUTPUT         10702
#define INTERFACE_AUDIOIN_DEFAULT    10801
#define INTERFACE_SCREEN_DEFAULT     80101
#define INTERFACE_APLAY_DEFAULT      80201
#define INTERFACE_MPLAY_DEFAULT      80301
#define INTERFACE_PIPE_LOCAL         90101
#define INTERFACE_PIPE_REMOTE        90102
#define INTERFACE_PIPE_CLOUD         90103
#define INTERFACE_LAUNCH_DEFAULT     90201
#define INTERFACE_HOTKEY_DEFAULT     90301
#define INTERFACE_METRONOME_DEFAULT  90401
#define INTERFACE_MESSENGER_COM      90501
#define INTERFACE_MESSENGER_TCP      90502
#define INTERFACE_MESSENGER_UDP_IN   90503
#define INTERFACE_MESSENGER_UDP_OUT  90504

// controller endpoint type id
#define ENDPOINT_MIDI_NOTE          101001
#define ENDPOINT_MIDI_CONTROL       101002
#define ENDPOINT_MIDI_PROGRAM       101003
#define ENDPOINT_MIDI_PITCH         101004
#define ENDPOINT_MIDI_CC_PULSE      101005
#define ENDPOINT_RIOC_DIGITAL_IN    102001
#define ENDPOINT_RIOC_DIGITAL_OUT   102002
#define ENDPOINT_RIOC_ANALOG_IN     102003
#define ENDPOINT_RIOC_ANALOG_OUT    102004
#define ENDPOINT_RIOC_PWM_OUT       102005
#define ENDPOINT_RIOC_PFM_OUT       102006
#define ENDPOINT_RIOC_ENCODER_IN    102007
#define ENDPOINT_RIOC_RUDDER_OUT    102008
#define ENDPOINT_RIOC_STEPPER_OUT   102009
#define ENDPOINT_RIOC_STEPPER_SET   102010
#define ENDPOINT_RIOC_USER_CHANNEL  102090
#define ENDPOINT_MODBUS_DISCRETE_IN 103001
#define ENDPOINT_MODBUS_COIL_OUT    103002
#define ENDPOINT_MODBUS_COIL_IN     103003
#define ENDPOINT_MODBUS_REGISTER_IN 103004
#define ENDPOINT_MODBUS_HOLDING_OUT 103005
#define ENDPOINT_MODBUS_HOLDING_IN  103006
#define ENDPOINT_HUE_LIGHT          104001
#define ENDPOINT_HUE_GROUP          104002
#define ENDPOINT_DMX_DIMMER         105001
#define ENDPOINT_DMX_DIMMER2        105002
#define ENDPOINT_DMX_MEDIA          105003
#define ENDPOINT_DMX_MASTER         105004
#define ENDPOINT_ARTNET_DIMMER      106001
#define ENDPOINT_ARTNET_DIMMER2     106002
#define ENDPOINT_ARTNET_MEDIA       106003
#define ENDPOINT_ARTNET_MASTER      106004
#define ENDPOINT_OSC_INT            107001
#define ENDPOINT_OSC_FLOAT          107002
#define ENDPOINT_OSC_BOOL           107003
#define ENDPOINT_AUDIOIN_LEVEL      108001
#define ENDPOINT_AUDIOIN_LEVEL_DB   108002
#define ENDPOINT_AUDIOIN_PEAK       108003
#define ENDPOINT_AUDIOIN_PEAK_DB    108004
#define ENDPOINT_AUDIOIN_SPECTRUM   108005
#define ENDPOINT_SCREEN_LIGHT       801001
#define ENDPOINT_SCREEN_MEDIA       801002
#define ENDPOINT_SCREEN_CANVAS      801003
#define ENDPOINT_APLAY_MEDIA        802001
#define ENDPOINT_MPLAY_MEDIA        803001
#define ENDPOINT_PIPE_ANALOG        901001
#define ENDPOINT_PIPE_BINARY        901002
#define ENDPOINT_PIPE_NOTE          901003
#define ENDPOINT_LAUNCH_PRESET      902001
#define ENDPOINT_HOTKEY_PRESS       903001
#define ENDPOINT_METRONOME_BEAT     904001
#define ENDPOINT_METRONOME_BPM      904002
#define ENDPOINT_METRONOME_QUANTUM  904003
#define ENDPOINT_METRONOME_RUN      904004
#define ENDPOINT_METRONOME_LINK     904005
#define ENDPOINT_METRONOME_TAP      904006
#define ENDPOINT_METRONOME_COUNT    904007
#define ENDPOINT_METRONOME_RESET    904008
#define ENDPOINT_MESSENGER_TEXT     905001
#define ENDPOINT_MESSENGER_HEXCODE  905002


// signal data type
#define DATA_SIGNAL_ANALOG  'A'  // analog signal
#define DATA_SIGNAL_BINARY  'B'  // binary signal
#define DATA_SIGNAL_NOTE    'N'  // midi note signal

// light control numbers
#define CONTROL_LIGHT_R    1  // red
#define CONTROL_LIGHT_G    2  // green
#define CONTROL_LIGHT_B    3  // blue
#define CONTROL_LIGHT_W    4  // white
#define CONTROL_LIGHT_HUE  5  // hue
#define CONTROL_LIGHT_SAT  6  // saturation
#define CONTROL_LIGHT_BRI  7  // brightness
#define CONTROL_LIGHT_CT   8  // color temperature

// media control numbers
#define CONTROL_MEDIA_START      1
#define CONTROL_MEDIA_STOP       2
#define CONTROL_MEDIA_STOP_ALL   3

#define CONTROL_MEDIA_OPACITY    11
#define CONTROL_MEDIA_SCALE      12
#define CONTROL_MEDIA_ROTATION   13
#define CONTROL_MEDIA_XOFFSET    14
#define CONTROL_MEDIA_YOFFSET    15
#define CONTROL_MEDIA_VOLUME     16
#define CONTROL_MEDIA_SPEED      17
#define CONTROL_MEDIA_POSITION   18
#define CONTROL_MEDIA_MASTER     19

// motion control numbers
#define CONTROL_MOTION_POSITION   1
#define CONTROL_MOTION_SPEED      2
#define CONTROL_MOTION_STOP       3

// signal data package struct
typedef struct dgsSignalData {

    char  signal;       // signal data type
    int   aValue;       // analog data value
    int   aRange;       // analog data range, example: 127 for a midi cc data, 65535 for a modbus register data
    bool  bValue;       // binary data value

    // defaults
    dgsSignalData() :
      signal(0),
      aValue(0),
      aRange(0),
      bValue(false)
    {}

} dgsSignalData;

// interface info struct
typedef struct dgsInterfaceInfo {

    int type;           // interface type id
    int mode;           // interface mode id
    bool output;        // can output signal
    bool input;         // can input signal
    QString label;      // a label describes the interface

    // defaults
    dgsInterfaceInfo() :
      type(0),
      mode(0),
      output(false),
      input(false),
      label("")
    {}

} dgsInterfaceInfo;

// endpoint info struct
typedef struct dgsEndpointInfo {

    bool enabled;

    int type;           // endpoint type id
    int unit;           // unit address number
    int channel;        // channel number
    int note;           // note number (for midi only)
    int control;        // control number (for midi and some others)
    char signal;        // signal data type
    bool output;        // is output signal
    bool input;         // is input signal
    int range;          // data range of the analog signal (optional)
    bool ready;         // is ready to work with the endpoint (optional)
    double initial;     // initial singal value rate 0 ~ 1.0 (optional)

    QString address;    // address string, like url or path

    QString labelEPT;   // a label describes the endpoint
    QString labelEPI;   // EPT is endpoint type, EPI is endpoint identity

    // defaults
    dgsEndpointInfo() :

      enabled(true),

      type(0),
      unit(0),
      channel(0),
      note(0),
      control(0),
      signal(0),
      output(false),
      input(false),
      range(0),
      ready(false),
      initial(-1),

      address(""),

      labelEPT(""),
      labelEPI("")
    {}

} dgsEndpointInfo;

// slot info struct
typedef struct dgsSlotInfo {

    // io signal types
    char inputSignal;       // type of the signal received from source endpoint
    char outputSignal;      // type of the signal sent to destination endpoint
    int inputRange;
    int outputRange;

    // io mapping options
    double inputLow;        // mapping bounds 0.0 ~ 1.0
    double inputHigh;
    double outputLow;
    double outputHigh;
    bool   inputInverted;   // signal inversion
    bool   outputInverted;
    bool   outputLowAsZero; // zero output while reaching lower limit

    // output signal envelop
    int    envelopeAttack;  // in milliseconds, default 0
    int    envelopeHold;    // in milliseconds, default 0
    int    envelopeDecay;   // in milliseconds, default 0
    double envelopeSustain; // ratio 0.0 ~ 1.0, default 1.0
    int    envelopeRelease; // in milliseconds, default 0
    int    envelopeInDelay; // in milliseconds, default 0
    int    envelopeOutDelay;

    // output smoothing
    int    outputSmoothing; // in milliseconds, default 0

    // traffic management
    int    outputInterval;  // in milliseconds, default 0

    // defaults
    dgsSlotInfo() :
        inputSignal(0),
        outputSignal(0),
        inputRange(0),
        outputRange(0),
        inputLow(0.0),
        inputHigh(1.0),
        outputLow(0.0),
        outputHigh(1.0),
        inputInverted(false),
        outputInverted(false),
        outputLowAsZero(false),
        envelopeAttack(0),
        envelopeHold(0),
        envelopeDecay(0),
        envelopeSustain(1.0),
        envelopeRelease(0),
        envelopeInDelay(0),
        envelopeOutDelay(0),
        outputSmoothing(0),
        outputInterval(0)
    { }

} dgsSlotInfo;


#endif // DIGISHOW_COMMON_H
