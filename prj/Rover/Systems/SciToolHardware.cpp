#include "SciToolHardware.h"

static const char* file = "SciToolardware";

SciToolHardware::SciToolHardware() {
    // initialise servo motors
    Logging::logD(file, "Initialising SciToolHardware Servo Motors");
    for (int servo = 0; servo < 2; servo++) {
        m_handlesServoGate.push_back(PhidgetRCServoHandle());
        PhidgetRCServo_create(&m_handlesServoGate.at(servo));
        setAddressProperties<PhidgetRCServoHandle>(
            &m_handlesServoGate.at(servo), SCI_TOOL_SERVO_SERIAL_NUMBER[servo],
            SCI_TOOL_SERVO_CHANNEL[servo], SCI_TOOL_SERVO_PORT[servo]);
        PhidgetRCServo_setEngaged(m_handlesServoGate.at(servo), 1);

        //Set min and Max for servo
        PhidgetRCServo_setMinPosition(m_handlesServoGate.at(servo), 0);
        PhidgetRCServo_setMaxPosition(m_handlesServoGate.at(servo), 180);

        PhidgetRCServo_setMinPulseWidth(m_handlesServoGate.at(servo),
                                        SCI_TOOL_SERVO_MIN_PW);
        PhidgetRCServo_setMaxPulseWidth(m_handlesServoGate.at(servo),
                                        SCI_TOOL_SERVO_MAX_PW);
    }

    // initialise DC motors
    Logging::logD(file, "Initialising SciToolHardware DC Lift Motor");
    PhidgetDCMotor_create(m_handlesDCLift);
    setAddressProperties<PhidgetDCMotorHandle>(
        m_handlesDCLift, SCI_TOOL_DC_LIFT_SERIAL_NUMBER,
        SCI_TOOL_DC_LIFT_CHANNEL, SCI_TOOL_DC_LIFT_PORT);

    Logging::logD(file, "Initialising SciToolHardware DC Brush Motors");
    PhidgetDCMotor_create(m_handlesDCBrush);
    setAddressProperties<PhidgetDCMotorHandle>(
        m_handlesDCBrush, SCI_TOOL_DC_BRUSH_SERIAL_NUMBER,
        SCI_TOOL_DC_BRUSH_CHANNEL, SCI_TOOL_DC_BRUSH_PORT);
}

SciToolHardware::~SciToolHardware() {
    // deinitialise servo motors
    Logging::logD(file, "Deinitialising SciToolHardware Servo Motors");
    for (int servo = 0; servo < 2; servo++) {
        Phidget_close((PhidgetHandle)m_handlesServoGate.at(servo));
        PhidgetRCServo_delete(&m_handlesServoGate.at(servo));
    }

    // deinitialise DC motors
    Logging::logD(file, "Deinitialising SciToolHardware DC Lift Motor");
    Phidget_close((PhidgetHandle)m_handlesDCLift);
    PhidgetDCMotor_delete(m_handlesDCLift);

    Logging::logD(file, "Deinitialising SciToolHardware DC Brush Motors");
    Phidget_close((PhidgetHandle)m_handlesDCBrush);
    PhidgetDCMotor_delete(m_handlesDCBrush);
}

void SciToolHardware::setGateServoAngle(ScienceToolIndex servo, float angle) {
    MotorHandlerReturn handler;
    getSciToolServoHandle(&handler, servo);
    PhidgetRCServo_setEngaged(*handler.handler.servoMotor, 1);
    PhidgetRCServo_setTargetPosition(*handler.handler.servoMotor, angle);
}

void SciToolHardware::setLiftSpeed(float speedPercentage) {
    MotorHandlerReturn handler;
    double maxSpeed;

    getSciToolDCLiftHandle(&handler);
    PhidgetDCMotor_getMaxVelocity(*handler.handler.dcMotor, &maxSpeed);
    PhidgetDCMotor_setTargetVelocity(*handler.handler.dcMotor,
                                     speedPercentage * maxSpeed / 100.0);
}

void SciToolHardware::setBrushSpeed(float speedPercentage) {
    MotorHandlerReturn handler;
    double maxSpeed;

    getSciToolDCBrushHandle(&handler);
    PhidgetDCMotor_getMaxVelocity(*handler.handler.dcMotor, &maxSpeed);
    PhidgetDCMotor_setTargetVelocity(*handler.handler.dcMotor,
                                     speedPercentage * maxSpeed / 100.0);
}

bool SciToolHardware::getSciToolServoHandle(MotorHandlerReturn* retVal,
                                            int index) {
    if (index >= 0 && index < SCI_TOOL_INDEX_GATE_COUNT) {
        retVal->type = MOTOR_TYPE_SERVO_MOTOR;
        retVal->handler.servoMotor = &m_handlesServoGate.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool SciToolHardware::getSciToolDCLiftHandle(MotorHandlerReturn* retVal) {
    retVal->type = MOTOR_TYPE_DC_MOTOR;
    retVal->handler.dcMotor = m_handlesDCLift;
    return true;
}

bool SciToolHardware::getSciToolDCBrushHandle(MotorHandlerReturn* retVal) {
    retVal->type = MOTOR_TYPE_DC_MOTOR;
    retVal->handler.dcMotor = m_handlesDCBrush;
    return true;
}
