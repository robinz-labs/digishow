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
#define INTERFACE_MIDI         1
#define INTERFACE_RIOC         2
#define INTERFACE_MODBUS       3
#define INTERFACE_HUE          4
#define INTERFACE_DMX          5
#define INTERFACE_ARTNET       6
#define INTERFACE_SCREEN       7
#define INTERFACE_PIPE         9

// controller interface mode id
#define INTERFACE_MIDI_INPUT         11
#define INTERFACE_MIDI_OUTPUT        12
#define INTERFACE_RIOC_GENERAL       21
#define INTERFACE_RIOC_ARDUINO_UNO   22
#define INTERFACE_RIOC_ARDUINO_MEGA  23
#define INTERFACE_RIOC_ALADDIN       24
#define INTERFACE_MODBUS_RTU         31
#define INTERFACE_MODBUS_TCP         32
#define INTERFACE_MODBUS_RTUOVERTCP  33
#define INTERFACE_HUE_HTTP           41
#define INTERFACE_DMX_ENTTEC_USB     51
#define INTERFACE_ARTNET_INPUT       61
#define INTERFACE_ARTNET_OUTPUT      62
#define INTERFACE_SCREEN_LOCAL       71
#define INTERFACE_SCREEN_REMOTE      72
#define INTERFACE_PIPE_LOCAL         91
#define INTERFACE_PIPE_REMOTE        92

// controller endpoint type id
#define ENDPOINT_MIDI_NOTE           101
#define ENDPOINT_MIDI_CONTROL        102
#define ENDPOINT_MIDI_PROGRAM        103
#define ENDPOINT_RIOC_DIGITAL_IN     201
#define ENDPOINT_RIOC_DIGITAL_OUT    202
#define ENDPOINT_RIOC_ANALOG_IN      203
#define ENDPOINT_RIOC_ANALOG_OUT     204
#define ENDPOINT_RIOC_PWM_OUT        205
#define ENDPOINT_RIOC_PFM_OUT        206
#define ENDPOINT_RIOC_ENCODER_IN     207
#define ENDPOINT_RIOC_RUDDER_OUT     208
#define ENDPOINT_MODBUS_DISCRETE_IN  301
#define ENDPOINT_MODBUS_COIL_OUT     302
#define ENDPOINT_MODBUS_COIL_IN      303
#define ENDPOINT_MODBUS_REGISTER_IN  304
#define ENDPOINT_MODBUS_HOLDING_OUT  305
#define ENDPOINT_MODBUS_HOLDING_IN   306
#define ENDPOINT_HUE_LIGHT           401
#define ENDPOINT_DMX_DIMMER          501
#define ENDPOINT_ARTNET_DIMMER       601
#define ENDPOINT_SCREEN_LIGHT        701
#define ENDPOINT_SCREEN_MEDIA        702
#define ENDPOINT_PIPE_ANALOG         901
#define ENDPOINT_PIPE_BINARY         902
#define ENDPOINT_PIPE_NOTE           904

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

// media control number
#define CONTROL_MEDIA_SHOW       1  // show media
#define CONTROL_MEDIA_HIDE       2  // hide media
#define CONTROL_MEDIA_CLEAR      3  // clear all media

#define CONTROL_MEDIA_OPACITY    11
#define CONTROL_MEDIA_SCALE      12
#define CONTROL_MEDIA_ROTATION   13
#define CONTROL_MEDIA_XOFFSET    14
#define CONTROL_MEDIA_YOFFSET    15

#define CONTROL_VIDEO_PLAY       20
#define CONTROL_VIDEO_REPEAT     21
#define CONTROL_VIDEO_VOLUME     22
#define CONTROL_VIDEO_POSITION   23

// midi control number
#define CONTROL_MIDI_BANK        0  // Bank Select
#define CONTROL_MIDI_MODULATION  1  // Modulation Wheel
#define CONTROL_MIDI_BREATH      2  // Breath Controller
#define CONTROL_MIDI_FOOT        4  // Foot Controller
#define CONTROL_MIDI_PORTAMENTO  5  // Portamento Time
#define CONTROL_MIDI_VOLUME      7  // Main Volume
#define CONTROL_MIDI_BALANCE     8  // Balance
#define CONTROL_MIDI_PAN         10 // Pan

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

      labelEPT(""),
      labelEPI("")
    {}

} dgsEndpointInfo;

// slot info struct
typedef struct dgsSlotInfo {

    // io signal types
    char inputSignal;       // type of the signal received from source endpoint
    char outputSignal;      // type of the signal sent to destination endpoint

    // io mapping options
    double inputThreshold;  // threshold 0.0 ~ 1.0
    double inputLow;        // mapping bounds 0.0 ~ 1.0
    double inputHigh;
    double outputLow;
    double outputHigh;
    bool   inputInverted;   // signal inversion
    bool   outputInverted;
    bool   outputLowAsZero; // zero output when reach lower limit

    // output signal envelop
    int    envelopeAttack;  // in milliseconds, default 0
    int    envelopePeak;    // in milliseconds, default 0
    int    envelopeDecay;   // in milliseconds, default 0
    double envelopeSustain; // ratio 0.0 ~ 1.0, default 1.0
    int    envelopeRelease; // in milliseconds, default 0

    // traffic management
    int    outputInterval;  // in milliseconds, default 0

    // defaults
    dgsSlotInfo() :
        inputSignal(0),
        outputSignal(0),
        inputThreshold(0.5),
        inputLow(0.0),
        inputHigh(1.0),
        outputLow(0.0),
        outputHigh(1.0),
        inputInverted(false),
        outputInverted(false),
        outputLowAsZero(false),
        envelopeAttack(0),
        envelopePeak(0),
        envelopeDecay(0),
        envelopeSustain(1.0),
        envelopeRelease(0),
        outputInterval(0)
    { }

} dgsSlotInfo;


#endif // DIGISHOW_COMMON_H
