#include "Arm.h"


Arm::Arm(int degOfFreedom, MotorType* motorTypes) :
    m_degOfFreedom(degOfFreedom), m_motorTypes(motorTypes) {
    m_handlesDC = new PhidgetDCMotorHandle*[degOfFreedom]();
    m_handlesStepper = new PhidgetStepperHandle*[degOfFreedom]();

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
            Logging::logE(file, "Can not initialise motor at index: %d with motor type: %d", motor, motorTypes[motor]);

            break;
        }
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
            Logging::logE(file, "Can not deinitialise for motor at index: %d with motor type: %d", motor, m_motorTypes[motor]);
            break;
        }
    }

    // deinitialise claw
    Phidget_close((PhidgetHandle)*m_handleClaw);
    PhidgetRCServo_delete(m_handleClaw);
}

bool Arm::getArmMotorHandle(MotorHandlerReturn* retVal, int index) {
    switch (m_motorTypes[index]) {
    case MOTOR_TYPE_DC_MOTOR:
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->motorHandle.dcMotor = m_handlesDC[index];
        break;

    case MOTOR_TYPE_STEPPER_MOTOR:
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->motorHandle.stepperMotor = m_handlesStepper[index];
        break;

    default:
        retVal->type = MOTOR_TYPE_INVALID;
        Logging::logE(file, "Can not get handler for motor at index: %d with motor type: %d", index, m_motorTypes[index]);
        return false;
        break;
    };

    return true;
}

bool Arm::getArmClawHandle(MotorHandlerReturn* retVal) {
    retVal->type = MOTOR_TYPE_SERVO_MOTOR;
    retVal->motorHandle.servo_motor = m_handleClaw;

    return true;
}

int Arm::getDOF() { return m_degOfFreedom; }