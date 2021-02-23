#ifndef RIOCCONTROLLER_H
#define RIOCCONTROLLER_H

#include <QObject>

#define RIOC_BAUD 115200

// #ifdef Q_OS_LINUX
//    #define RIOC_PORT "/dev/ttyAMA0"
// #endif
// #ifdef Q_OS_MAC
//    #define RIOC_PORT "/dev/cu.usbserial"
// #endif
// #ifdef Q_OS_WIN
//    #define RIOC_PORT "COM3"
// #endif

// Auduino UNO / MEGA
#define RIOC_VID1 0x2341
#define RIOC_PID1 0

#define RIOC_VID2 0x2A03
#define RIOC_PID2 0

// FTDI
// #define RIOC_VID3 1027
// #define RIOC_PID3 24577

// RIOC object types
#define RO_SYSTEM             		    0x00
#define RO_GENERAL_DIGITAL_IN		    0x01
#define RO_GENERAL_DIGITAL_OUT		    0x02
#define RO_GENERAL_ANALOG_IN		    0x03
#define RO_GENERAL_ANALOG_OUT		    0x04
#define RO_GENERAL_UART_SERIAL		    0x05
#define RO_GENERAL_MULTIPLE_DIGITAL_IN  0x06
#define RO_GENERAL_MULTIPLE_DIGITAL_OUT 0x07
#define RO_MOTION_MOTOR				    0x11
#define RO_MOTION_STEPPER			    0x12
#define RO_MOTION_RUDDER			    0x13
#define RO_MOTION_SERVO				   	0x14
#define RO_SENSOR_ENCODER			    0x21
#define RO_SENSOR_ULTRASONIC_RANGER	    0x22
#define RO_SENSOR_THERMOMETER		    0x23
#define RO_SOUND_TONE				    0x31
#define RO_LIGHT_RGBLED				    0x41
#define RO_IR_TRANSMITTER			    0x51
#define RO_IR_RECEIVER				    0x52
#define RO_NTROBOT_MOTOR                0xF1

// RIOC object commands
#define RO_SYSTEM_START                 0x8f
#define RO_OBJECT_SETUP                 0x00

#define RO_GENERAL_DI_READ              0x01
#define RO_GENERAL_DI_SET_NOTIFICATION  0x02
#define RO_GENERAL_DI_NOTIFY            0x83
#define RO_GENERAL_DO_WRITE             0x01
#define RO_GENERAL_DO_WRITE_PWM         0x03
#define RO_GENERAL_DO_PULSE             0x04
#define RO_GENERAL_DO_READ              0x05
#define RO_GENERAL_AI_READ              0x01
#define RO_GENERAL_AI_SET_NOTIFICATION  0x02
#define RO_GENERAL_AI_NOTIFY            0x83
#define RO_GENERAL_AO_WRITE             0x01
#define RO_GENERAL_AO_READ              0x02

#define RO_GENERAL_DO_WRITE_PWM         0x03
#define RO_GENERAL_DO_WRITE_PWM         0x03

#define RO_SOUND_TONE_PLAY  		    0x01
#define RO_SOUND_TONE_STOP  		    0x02

#define RO_SENSOR_ENCODER_READ          0x01
#define RO_SENSOR_ENCODER_SET_NOTIFICATION  0x02
#define RO_SENSOR_ENCODER_NOTIFY        0x83
#define RO_SENSOR_ENCODER_WRITE         0x04

#define RO_MOTION_RUDDER_SET_ANGLE      0x01
#define RO_MOTION_RUDDER_GET_ANGLE      0x02
#define RO_MOTION_RUDDER_SET_ENABLE     0x03
#define RO_MOTION_RUDDER_GET_ENABLE     0x04

#define RO_MOTION_STEPPER_STEP          0x01
#define RO_MOTION_STEPPER_GOTO          0x02
#define RO_MOTION_STEPPER_STOP          0x03
#define RO_MOTION_STEPPER_SET_SPEED     0x04
#define RO_MOTION_STEPPER_GET_SPEED     0x05
#define RO_MOTION_STEPPER_SET_POSITION  0x06
#define RO_MOTION_STEPPER_GET_POSITION  0x07

#define RO_NTROBOT_MOVE                 0x01
#define RO_NTROBOT_ZERO                 0x02
#define RO_NTROBOT_STOP                 0x03
#define RO_NTROBOT_SET_SPEED            0x04
#define RO_NTROBOT_GET_STATUS           0x05
#define RO_NTROBOT_GET_SETTINGS         0x06
#define RO_NTROBOT_SET_POSITION         0x07

class RiocService;

class RiocController : public QObject
{
    Q_OBJECT
public:
    RiocController(QObject *parent = 0, bool autoConnect = true);
    ~RiocController();

    bool connect(const QString &port = QString());
    void disconnect();

    void setLoggerEnabled(bool enable);

    RiocService* service() { return _riocService; }

    void resetUnit(unsigned char unit);

    bool setupObject(unsigned char unit, unsigned char objectType,
                     unsigned char param1=0, unsigned char param2=0, unsigned char param3=0,
                     unsigned char param4=0, unsigned char param5=0, unsigned char param6=0);

    bool silence(unsigned char unit, bool silent = true);
    bool beginSync(unsigned char unit);
    bool finishSync(unsigned char unit);

    // general IO
    bool digitalInRead(unsigned char unit, unsigned char channel, bool* pValue);
    bool digitalInSetNotification(unsigned char unit, unsigned char channel, bool enabled);
    bool digitalOutWrite(unsigned char unit, unsigned char channel, bool value, bool wait = false);
    bool digitalOutWritePwm(unsigned char unit, unsigned char channel, int value, bool wait = false);
    bool analogInRead(unsigned char unit, unsigned char channel, int* pValue, int* pDecimals = NULL);
    bool analogInSetNotification(unsigned char unit, unsigned char channel, bool enabled = true, int interval = 50, unsigned char significantBits = 0);
    bool analogOutWrite(unsigned char unit, unsigned char channel, int value, int decimals = 0, bool wait = false);

    // tone player (pulse frequency modulation)
    bool tonePlay(unsigned char unit, unsigned char channel, int frequency, int duration = 0, bool wait = false);
    bool toneStop(unsigned char unit, unsigned char channel, bool wait = false);

    // encoder
    bool encoderRead(unsigned char unit, unsigned char channel, int* pValue);
    bool encoderSetNotification(unsigned char unit, unsigned char channel, bool enabled = true, int interval = 50, unsigned char significantBits = 0);
    bool encoderWrite(unsigned char unit, unsigned char channel, int value = 0);

    // rudder servo
    bool rudderSetAngle(unsigned char unit, unsigned char channel, int angle, bool wait = false);
    bool rudderSetEnable(unsigned char unit, unsigned char channel, bool enabled = true);

    // stepper
    bool stepperStep(unsigned char unit, unsigned char channel, int steps, bool wait = false);
    bool stepperGoto(unsigned char unit, unsigned char channel, int position, bool wait = false);
    bool stepperStop(unsigned char unit, unsigned char channel, bool wait = false);
    bool stepperSetSpeed(unsigned char unit, unsigned char channel, unsigned int speed, bool wait = false);
    bool stepperGetSpeed(unsigned char unit, unsigned char channel, unsigned int* pSpeed);
    bool stepperSetPosition(unsigned char unit, unsigned char channel, int position, bool wait = false);
    bool stepperGetPosition(unsigned char unit, unsigned char channel, int* pPosition);

    // motor controls (NTRobot protocols)
    bool motorMove(unsigned char unit, unsigned char channel, int position, int speed, bool brake = true);
    bool motorZero(unsigned char unit, unsigned char channel, int speed, bool brake = true);
    bool motorStop(unsigned char unit, unsigned char channel, bool brake = true);
    bool motorSetSpeed(unsigned char unit, unsigned char channel, int speed);
    bool motorGetStatus(unsigned char unit, unsigned char channel, int* pCurrentPosition, int* pMoving);
    bool motorGetSettings(unsigned char unit, unsigned char channel, int* pTargetPosition, int* pSpeed, bool* pBrake);
    bool motorSetPosition(unsigned char unit, unsigned char channel, int position);

signals:
    // general IO and sensor notifications
    void unitStarted(unsigned char unit);
    void digitalInValueUpdated(unsigned char unit, unsigned char channel, bool value);
    void analogInValueUpdated(unsigned char unit, unsigned char channel, int value);
    void encoderValueUpdated(unsigned char unit, unsigned char channel, int value);

    // trigger notifications (deprecated)
    void triggerActivated(int triggerID);
    void triggerDeactivated(int triggerID);

public slots:
    void handleRiocMessageReceived(unsigned char fromID, unsigned char toID, const QByteArray & data);

private:
    RiocService* _riocService;
};

#endif // RIOCCONTROLLER_H
