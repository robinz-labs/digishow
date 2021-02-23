#include <QSerialPortInfo>
#include <math.h>
#include "rioc_controller.h"
#include "rioc_service.h"

RiocController::RiocController(QObject *parent, bool autoConnect) :
    QObject(parent)
{
    _riocService = new RiocService();

    QObject::connect(_riocService, SIGNAL(riocMessageReceived(unsigned char,unsigned char,QByteArray)), this, SLOT(handleRiocMessageReceived(unsigned char,unsigned char,QByteArray)));
    if (autoConnect) connect();
}

RiocController::~RiocController()
{
    disconnect();
    delete _riocService;
}

bool RiocController::connect(const QString &port)
{
    // if (g_license.isEmpty() || g_license != getLicenseKey()) return false;

    // prefrered port mode:
    // only open the prefrered serial port to connect the rioc unit
    if (!port.isEmpty()) {
        return (_riocService->addSerialConnection(port, RIOC_BAUD));
    }

    // automatic mode:
    // scan and open all specific serial ports to connect all rioc units
    int connectionCount = 0;

    #ifdef RIOC_PORT

        // open a specific port
        if (_riocService->addSerialConnection(RIOC_PORT, RIOC_BAUD)) connectionCount++;

    #else

        // open all ports matched with the specific VID/PID
        foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

            if (serialPortInfo.isBusy()) continue;

#ifdef Q_OS_MAC
            if (serialPortInfo.portName().startsWith("cu.")) continue;
#endif

            bool found = false;

            if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {

                int vid = serialPortInfo.vendorIdentifier();
                int pid = serialPortInfo.productIdentifier();

                #ifdef RIOC_VID1
                    if (RIOC_VID1==vid && (RIOC_PID1==0 || RIOC_PID1==pid)) found = true;
                #endif
                #ifdef RIOC_VID2
                    if (RIOC_VID2==vid && (RIOC_PID2==0 || RIOC_PID2==pid)) found = true;
                #endif
            }

            if (found) {
                QString port = serialPortInfo.portName();
                if (_riocService->addSerialConnection(port, RIOC_BAUD)) connectionCount++;
            }
        }

    #endif

    return (connectionCount>0);
}

void RiocController::disconnect()
{
    _riocService->clearSerialConnections();
}

void RiocController::setLoggerEnabled(bool enable)
{
    _riocService->setLoggerEnabled(enable);
}

void RiocController::resetUnit(unsigned char unit) {
    unsigned char bytes[] = {
        0x00, 0x01,
        0, 0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    _riocService->sendRiocMessage(0, unit, cmd);
}

bool RiocController::setupObject(unsigned char unit, unsigned char objectType,
        unsigned char param1, unsigned char param2, unsigned char param3,
        unsigned char param4, unsigned char param5, unsigned char param6)
{
    unsigned char bytes[] = {
        objectType, RO_OBJECT_SETUP,
        param1, param2, param3, param4, param5, param6 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    return _riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp);
}

bool RiocController::silence(unsigned char unit, bool silent)
{
    unsigned char bytes[] = {
        0x00, 0x07, (unsigned char)(silent ? 0x01 : 0x00),
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::beginSync(unsigned char unit)
{
    unsigned char bytes[] = {
        0x00, 0x04,
        0, 0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::finishSync(unsigned char unit)
{
    unsigned char bytes[] = {
        0x00, 0x05,
        0, 0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::digitalInRead(unsigned char unit, unsigned char channel, bool* pValue)
{
    unsigned char bytes[] = {
        RO_GENERAL_DIGITAL_IN,
        RO_GENERAL_DI_READ,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        *pValue = (rsp.at(3)==0 ? false : true);
        return true;
    }
    return false;
}

bool RiocController::digitalInSetNotification(unsigned char unit, unsigned char channel, bool enabled)
{
    unsigned char bytes[] = {
        RO_GENERAL_DIGITAL_IN,
        RO_GENERAL_DI_SET_NOTIFICATION,
        channel,
        (unsigned char)(enabled ? 0x01 : 0x00),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::digitalOutWrite(unsigned char unit, unsigned char channel, bool value, bool wait)
{
    unsigned char bytes[] = {
        RO_GENERAL_DIGITAL_OUT,
        RO_GENERAL_DO_WRITE,
        channel,
        (unsigned char)(value ? 0x01 : 0x00),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::digitalOutWritePwm(unsigned char unit, unsigned char channel, int value, bool wait)
{
    unsigned char bytes[] = {
        RO_GENERAL_DIGITAL_OUT,
        RO_GENERAL_DO_WRITE_PWM,
        channel,
        (unsigned char)(value & 0xff),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::analogInRead(unsigned char unit, unsigned char channel, int* pValue, int* pDecimals)
{
    unsigned char bytes[] = {
        RO_GENERAL_ANALOG_IN,
        RO_GENERAL_AI_READ,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int valMSB = (unsigned char)rsp.at(3);
        int valLSB = (unsigned char)rsp.at(4);
        int sign   = (rsp.at(5)==0 ? 1 : -1);
        *pValue = (valMSB*0x0100 + valLSB) * sign;

        if (pDecimals != NULL) {
            int decimals = (unsigned char)rsp.at(6);
            *pDecimals = decimals;
        }

        return true;
    }
    return false;
}

bool RiocController::analogInSetNotification(unsigned char unit, unsigned char channel, bool enabled, int interval, unsigned char significantBits)
{
    unsigned char bytes[] = {
        RO_GENERAL_ANALOG_IN,
        RO_GENERAL_AI_SET_NOTIFICATION,
        channel,
        (unsigned char)(enabled ? 0x01 : 0x00),
        (unsigned char)((interval>>8) & 0xFF),
        (unsigned char)(interval & 0xFF),
        significantBits,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::analogOutWrite(unsigned char unit, unsigned char channel, int value, int decimals, bool wait)
{
    unsigned char valMSB = (abs(value) >> 8) & 0xFF ;
    unsigned char valLSB = abs(value) & 0xFF;
    unsigned char sign   = (value >= 0 ? 0x00 : 0x01);

    unsigned char bytes[] = {
        RO_GENERAL_ANALOG_OUT,
        RO_GENERAL_AO_WRITE,
        channel,
        valMSB,
        valLSB,
        sign,
        (unsigned char)decimals,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::tonePlay(unsigned char unit, unsigned char channel, int frequency, int duration, bool wait)
{
    unsigned char frequencyMSB = (frequency >> 8) & 0xFF ;
    unsigned char frequencyLSB = frequency & 0xFF;
    unsigned char  durationMSB = (duration >> 8) & 0xFF ;
    unsigned char  durationLSB = duration & 0xFF;

    unsigned char bytes[] = {
        RO_SOUND_TONE,
        RO_SOUND_TONE_PLAY,
        channel,
        frequencyMSB,
        frequencyLSB,
        durationMSB,
        durationLSB,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::toneStop(unsigned char unit, unsigned char channel, bool wait)
{
    unsigned char bytes[] = {
        RO_SOUND_TONE,
        RO_SOUND_TONE_STOP,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::encoderRead(unsigned char unit, unsigned char channel, int* pValue)
{
    unsigned char bytes[] = {
        RO_SENSOR_ENCODER,
        RO_SENSOR_ENCODER_READ,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int sign = (rsp.at(3)==0 ? 1 : -1);
        int valH = (unsigned char)rsp.at(4);
        int valM = (unsigned char)rsp.at(5);
        int valL = (unsigned char)rsp.at(6);

        *pValue = (valH*0x010000 + valM*0x0100 + valL) * sign;

        return true;
    }
    return false;

}

bool RiocController::encoderSetNotification(unsigned char unit, unsigned char channel, bool enabled, int interval, unsigned char significantBits)
{
    unsigned char bytes[] = {
        RO_SENSOR_ENCODER,
        RO_SENSOR_ENCODER_SET_NOTIFICATION,
        channel,
        (unsigned char)(enabled ? 0x01 : 0x00),
        (unsigned char)((interval>>8) & 0xFF),
        (unsigned char)(interval & 0xFF),
        significantBits,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;

}

bool RiocController::encoderWrite(unsigned char unit, unsigned char channel, int value)
{
    unsigned char valH = (abs(value) >> 16) & 0xFF ;
    unsigned char valM = (abs(value) >> 8) & 0xFF ;
    unsigned char valL = abs(value) & 0xFF;
    unsigned char sign = (value >= 0 ? 0x00 : 0x01);

    unsigned char bytes[] = {
        RO_SENSOR_ENCODER,
        RO_SENSOR_ENCODER_WRITE,
        channel,
        sign,
        valH,
        valM,
        valL,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::rudderSetAngle(unsigned char unit, unsigned char channel, int angle, bool wait)
{
    unsigned char bytes[] = {
        RO_MOTION_RUDDER,
        RO_MOTION_RUDDER_SET_ANGLE,
        channel,
        (unsigned char)(angle & 0xFF),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::rudderSetEnable(unsigned char unit, unsigned char channel, bool enabled)
{
    unsigned char bytes[] = {
        RO_MOTION_RUDDER,
        RO_MOTION_RUDDER_SET_ENABLE,
        channel,
        (unsigned char)(enabled ? 0x01 : 0x00),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperStep(unsigned char unit, unsigned char channel, int steps, bool wait)
{
    unsigned char dir = (steps<0 ? 1 : 0);
    unsigned int  steps_ = abs(steps);
    unsigned char stepsH = (steps_ >> 8) & 0xFF;
    unsigned char stepsL = steps_ & 0xFF;

    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_STEP,
        channel,
        dir,
        stepsH,
        stepsL,
        0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperGoto(unsigned char unit, unsigned char channel, int position, bool wait)
{
    unsigned char sign = (position<0 ? 1 : 0);
    unsigned int position_ = abs(position);
    unsigned char positionH = (position_ >> 16) & 0xFF;
    unsigned char positionM = (position_ >> 8) & 0xFF;
    unsigned char positionL = position_ & 0xFF;

    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_GOTO,
        channel,
        sign,
        positionH,
        positionM,
        positionL,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperStop(unsigned char unit, unsigned char channel, bool wait)
{
    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_STOP,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperSetSpeed(unsigned char unit, unsigned char channel, unsigned int speed, bool wait)
{
    unsigned char speedH = (speed >> 8) & 0xFF;
    unsigned char speedL = speed & 0xFF;

    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_SET_SPEED,
        channel,
        speedH,
        speedL,
        0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperGetSpeed(unsigned char unit, unsigned char channel, unsigned int* pSpeed)
{
    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_GET_SPEED,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int speedH = (unsigned char)rsp.at(3);
        int speedL = (unsigned char)rsp.at(4);

        *pSpeed = (speedH << 8) | speedL;
        return true;
    }
    return false;
}

bool RiocController::stepperSetPosition(unsigned char unit, unsigned char channel, int position, bool wait)
{
    unsigned char sign = (position<0 ? 1 : 0);
    unsigned char positionH = (position >> 16) & 0xFF;
    unsigned char positionM = (position >> 8) & 0xFF;
    unsigned char positionL = position & 0xFF;

    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_SET_POSITION,
        channel,
        sign,
        positionH,
        positionM,
        positionL,
        0 };

    QByteArray cmd((const char*)bytes, 8);
    if (!wait) {
        _riocService->sendRiocMessage(0, unit, cmd);
        return true;
    }

    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::stepperGetPosition(unsigned char unit, unsigned char channel, int* pPosition)
{
    unsigned char bytes[] = {
        RO_MOTION_STEPPER,
        RO_MOTION_STEPPER_GET_POSITION,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int sign      = (unsigned char)rsp.at(3);
        int positionH = (unsigned char)rsp.at(4);
        int positionM = (unsigned char)rsp.at(5);
        int positionL = (unsigned char)rsp.at(6);

        *pPosition = (sign ? -1 : 1)*((positionH<<16)|(positionM<<8)|positionL);
        return true;
    }
    return false;
}

bool RiocController::motorMove(unsigned char unit, unsigned char channel, int position, int speed, bool brake)
{
    unsigned char positionH = (position >> 16) & 0xFF;
    unsigned char positionM = (position >> 8) & 0xFF;
    unsigned char positionL = position & 0xFF;

    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_MOVE,
        channel,
        positionH,
        positionM,
        positionL,
        (unsigned char)(speed/10),
        (unsigned char)(brake ? 0x01 : 0x00) };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::motorZero(unsigned char unit, unsigned char channel, int speed, bool brake)
{
    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_ZERO,
        channel,
        (unsigned char)(speed/10),
        (unsigned char)(brake ? 0x01 : 0x00),
        0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::motorStop(unsigned char unit, unsigned char channel, bool brake)
{
    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_STOP,
        channel,
        (unsigned char)(brake ? 0x01 : 0x00),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::motorSetSpeed(unsigned char unit, unsigned char channel, int speed)
{
    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_SET_SPEED,
        channel,
        (unsigned char)(speed/10),
        0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

bool RiocController::motorGetStatus(unsigned char unit, unsigned char channel, int* pCurrentPosition, int* pMoving)
{
    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_GET_STATUS,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int positionH = (unsigned char)rsp.at(3);
        int positionM = (unsigned char)rsp.at(4);
        int positionL = (unsigned char)rsp.at(5);
        int moving    = (unsigned char)rsp.at(6);

        *pCurrentPosition = (positionH << 16) | (positionM << 8) | positionL;
        *pMoving = moving;

        return true;
    }
    return false;
}

bool RiocController::motorGetSettings(unsigned char unit, unsigned char channel, int* pTargetPosition, int* pSpeed, bool* pBrake)
{
    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_GET_SETTINGS,
        channel,
        0, 0, 0, 0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {

        int positionH = (unsigned char)rsp.at(3);
        int positionM = (unsigned char)rsp.at(4);
        int positionL = (unsigned char)rsp.at(5);
        int speed     = (unsigned char)rsp.at(6);
        int brake     = (unsigned char)rsp.at(7);

        *pTargetPosition = (positionH << 16) | (positionM << 8) | positionL ;
        *pSpeed = speed*10;
        *pBrake = (brake==0 ? false : true);

        return true;
    }
    return false;
}

bool RiocController::motorSetPosition(unsigned char unit, unsigned char channel, int position)
{
    unsigned char positionH = (position >> 16) & 0xFF;
    unsigned char positionM = (position >> 8) & 0xFF;
    unsigned char positionL = position & 0xFF;

    unsigned char bytes[] = {
        RO_NTROBOT_MOTOR,
        RO_NTROBOT_SET_POSITION,
        channel,
        positionH,
        positionM,
        positionL,
        0, 0 };

    QByteArray cmd((const char*)bytes, 8);
    QByteArray rsp;
    if (_riocService->sendRiocMessageAndWaitResponse(0, unit, cmd, rsp)) {
        return true;
    }
    return false;
}

void RiocController::handleRiocMessageReceived(unsigned char fromID, unsigned char toID, const QByteArray & data)
{
    // qDebug() << fromID << toID << data;

    if (toID !=0 && toID !=0xFF) return; // this message is not for me

    unsigned char classID  = data.at(0);
    unsigned char funcCode = data.at(1);
    unsigned char channel  = data.at(2);

    if (classID == RO_SYSTEM && funcCode == RO_SYSTEM_START) {

        emit unitStarted(fromID);

    } else if (classID == RO_GENERAL_DIGITAL_IN && funcCode == RO_GENERAL_DI_NOTIFY) {

        bool state = data.at(3);
        emit digitalInValueUpdated(fromID, channel, state);

        // trigger notification is deprecated
        if (state == true) {
            emit triggerActivated(channel);
        } else {
            emit triggerDeactivated(channel);
        }

    } else if (classID == RO_GENERAL_ANALOG_IN && funcCode == RO_GENERAL_AI_NOTIFY) {

        int valueMSB = (unsigned char)data.at(3);
        int valueLSB = (unsigned char)data.at(4);
        int value = (valueMSB<<8)|valueLSB;

        emit analogInValueUpdated(fromID, channel, value);

    } else if (classID == RO_SENSOR_ENCODER && funcCode == RO_SENSOR_ENCODER_NOTIFY) {

        int sign = (data.at(3)==0 ? 1 : -1);
        int valH = (unsigned char)data.at(4);
        int valM = (unsigned char)data.at(5);
        int valL = (unsigned char)data.at(6);

        int value = (valH*0x010000 + valM*0x0100 + valL) * sign;

        emit encoderValueUpdated(fromID, channel, value);
    }
}
