#include "Arm.h"

static const char* file = "Arm";

Arm::Arm(int degOfFreedom, MotorType* motorTypes) :
    m_degOfFreedom(degOfFreedom), m_motorTypes(motorTypes) {
    m_handlesDC = new PhidgetDCMotorHandle*[degOfFreedom]();
    m_handlesStepper = new PhidgetStepperHandle*[degOfFreedom]();
    m_handlesEncoder = new PhidgetEncoderHandle*[degOfFreedom]();

    // initialise motors in the arm
    for (int motor = 0; motor < degOfFreedom; motor++) {
        switch (motorTypes[motor]) {
        case MOTOR_TYPE_DC_MOTOR:
            PhidgetDCMotor_create(m_handlesDC[motor]);
            setAddressProperties<PhidgetDCMotorHandle>(
                m_handlesDC[motor], ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor]);

            break;
        case MOTOR_TYPE_STEPPER_MOTOR:
            PhidgetStepper_create(m_handlesStepper[motor]);
            setAddressProperties<PhidgetStepperHandle>(
                m_handlesStepper[motor], ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor]);

            break;
        default:
            Logging::logE(
                file,
                "Can not initialise motor at index: %d with motor type: %d",
                motor, motorTypes[motor]);

            break;
        }
    }

    for (int encoder = 0; encoder < degOfFreedom; encoder++) {
        PhidgetEncoder_create(m_handlesEncoder[encoder]);
        setAddressProperties<PhidgetEncoderHandle>(
            m_handlesEncoder[encoder], ARM_ENCODER_SERIAL_NUMBER[encoder],
            ARM_ENCODER_CHANNEL[encoder]);
    }

    // initialise claw
    PhidgetRCServo_create(m_handleClaw);
    setAddressProperties<PhidgetRCServoHandle>(
        m_handleClaw, ARM_CLAW_SERIAL_NUMBER, ARM_CLAW_CHANNEL);
}

Arm::~Arm() {
    // deinitialise motors in the arm
    for (int motor = 0; motor < m_degOfFreedom; motor++) {
        switch (m_motorTypes[motor]) {
        case MOTOR_TYPE_DC_MOTOR:
            Phidget_close((PhidgetHandle)*m_handlesDC[motor]);
            PhidgetDCMotor_delete(m_handlesDC[motor]);
            break;

        case MOTOR_TYPE_STEPPER_MOTOR:
            Phidget_close((PhidgetHandle)*m_handlesStepper[motor]);
            PhidgetStepper_delete(m_handlesStepper[motor]);
            break;

        default:
            Logging::logE(file,
                          "Can not deinitialise for motor at index: %d with "
                          "motor type: %d",
                          motor, m_motorTypes[motor]);
            break;
        }
    }
    for (int encoder = 0; encoder < m_degOfFreedom; encoder++) {
        Phidget_close((PhidgetHandle)*m_handlesEncoder[encoder]);
        PhidgetEncoder_delete(m_handlesEncoder[encoder]);
    }

    // deinitialise claw
    Phidget_close((PhidgetHandle)*m_handleClaw);
    PhidgetRCServo_delete(m_handleClaw);
}

bool Arm::getArmMotorHandle(MotorHandlerReturn* retVal, int index) {
    switch (m_motorTypes[index]) {
    case MOTOR_TYPE_DC_MOTOR:
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->handler.dcMotor = m_handlesDC[index];
        break;

    case MOTOR_TYPE_STEPPER_MOTOR:
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->handler.stepperMotor = m_handlesStepper[index];
        break;

    default:
        retVal->type = MOTOR_TYPE_INVALID;
        Logging::logE(
            file,
            "Can not get handler for motor at index: %d with motor type: %d",
            index, m_motorTypes[index]);
        return false;
        break;
    };

    return true;
}

bool Arm::getArmEncoderHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < m_degOfFreedom) {
        retVal->type = MOTOR_TYPE_ENCODER;
        retVal->handler.encoder = m_handlesEncoder[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Arm::getArmClawHandle(MotorHandlerReturn* retVal) {
    retVal->type = MOTOR_TYPE_SERVO_MOTOR;
    retVal->handler.servo_motor = m_handleClaw;

    return true;
}

int Arm::getDOF() { return m_degOfFreedom; }