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

// controller interface mode id
#define INTERFACE_MIDI_INPUT        1011
#define INTERFACE_MIDI_OUTPUT       1012
#define INTERFACE_RIOC_GENERAL      1021
#define INTERFACE_RIOC_ARDUINO_UNO  1022
#define INTERFACE_RIOC_ARDUINO_MEGA 1023
#define INTERFACE_RIOC_ALADDIN      1024
#define INTERFACE_MODBUS_RTU        1031
#define INTERFACE_MODBUS_TCP        1032
#define INTERFACE_MODBUS_RTUOVERTCP 1033
#define INTERFACE_HUE_DEFAULT       1041
#define INTERFACE_DMX_ENTTEC_PRO    1051
#define INTERFACE_DMX_ENTTEC_OPEN   1052
#define INTERFACE_ARTNET_INPUT      1061
#define INTERFACE_ARTNET_OUTPUT     1062
#define INTERFACE_OSC_INPUT         1071
#define INTERFACE_OSC_OUTPUT        1072
#define INTERFACE_AUDIOIN_DEFAULT   1081
#define INTERFACE_SCREEN_DEFAULT    8011
#define INTERFACE_APLAY_DEFAULT     8021
#define INTERFACE_MPLAY_DEFAULT     8031
#define INTERFACE_PIPE_LOCAL        9011
#define INTERFACE_PIPE_REMOTE       9012
#define INTERFACE_LAUNCH_DEFAULT    9021
#define INTERFACE_HOTKEY_DEFAULT    9031
#define INTERFACE_METRONOME_DEFAULT 9041

// controller endpoint type id
#define ENDPOINT_MIDI_NOTE          10101
#define ENDPOINT_MIDI_CONTROL       10102
#define ENDPOINT_MIDI_PROGRAM       10103
#define ENDPOINT_MIDI_CC_PULSE      10104
#define ENDPOINT_RIOC_DIGITAL_IN    10201
#define ENDPOINT_RIOC_DIGITAL_OUT   10202
#define ENDPOINT_RIOC_ANALOG_IN     10203
#define ENDPOINT_RIOC_ANALOG_OUT    10204
#define ENDPOINT_RIOC_PWM_OUT       10205
#define ENDPOINT_RIOC_PFM_OUT       10206
#define ENDPOINT_RIOC_ENCODER_IN    10207
#define ENDPOINT_RIOC_RUDDER_OUT    10208
#define ENDPOINT_RIOC_STEPPER_OUT   10209
#define ENDPOINT_MODBUS_DISCRETE_IN 10301
#define ENDPOINT_MODBUS_COIL_OUT    10302
#define ENDPOINT_MODBUS_COIL_IN     10303
#define ENDPOINT_MODBUS_REGISTER_IN 10304
#define ENDPOINT_MODBUS_HOLDING_OUT 10305
#define ENDPOINT_MODBUS_HOLDING_IN  10306
#define ENDPOINT_HUE_LIGHT          10401
#define ENDPOINT_HUE_GROUP          10402
#define ENDPOINT_DMX_DIMMER         10501
#define ENDPOINT_ARTNET_DIMMER      10601
#define ENDPOINT_OSC_INT            10701
#define ENDPOINT_OSC_FLOAT          10702
#define ENDPOINT_OSC_BOOL           10703
#define ENDPOINT_AUDIOIN_LEVEL      10801
#define ENDPOINT_SCREEN_LIGHT       80101
#define ENDPOINT_SCREEN_MEDIA       80102
#define ENDPOINT_SCREEN_CANVAS      80103
#define ENDPOINT_APLAY_MEDIA        80201
#define ENDPOINT_MPLAY_MEDIA        80301
#define ENDPOINT_PIPE_ANALOG        90101
#define ENDPOINT_PIPE_BINARY        90102
#define ENDPOINT_PIPE_NOTE          90103
#define ENDPOINT_LAUNCH_PRESET      90201
#define ENDPOINT_HOTKEY_PRESS       90301
#define ENDPOINT_METRONOME_BEAT     90401

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
    int control;        // control number (for midi only)
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
        outputSmoothing(0),
        outputInterval(0)
    { }

} dgsSlotInfo;


#endif // DIGISHOW_COMMON_H
