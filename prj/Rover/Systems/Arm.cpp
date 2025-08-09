#include "Arm.h"

static const char* file = "Arm";

Arm::Arm(const std::vector<MotorType> motorTypes) :
    m_degOfFreedom(motorTypes.size()), m_motorTypes(motorTypes) {

    // initialise motors in the arm
    Logging::logD(file, "Initialising Arm Motors");
    for (int motor = 0; motor < m_degOfFreedom; motor++) {
        switch (motorTypes[motor]) {
        case MOTOR_TYPE_DC_MOTOR:
            // Add new DC Motor Handle
            m_handlesDC.push_back(PhidgetDCMotorHandle());
            PhidgetDCMotor_create(&m_handlesDC.at(motor));
            setAddressProperties<PhidgetDCMotorHandle>(
                &m_handlesDC.at(motor), ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor], ARM_MOTOR_PORT[motor]);

            // Add new DC Motor Encoder Handle
            m_handlesEncoder.push_back(PhidgetEncoderHandle());
            PhidgetEncoder_create(&m_handlesEncoder.at(motor));
            setAddressProperties<PhidgetEncoderHandle>(
            &m_handlesEncoder.at(motor), ARM_ENCODER_SERIAL_NUMBER[motor],
            ARM_ENCODER_CHANNEL[motor], ARM_ENCODER_PORT[motor]);

            break;
        case MOTOR_TYPE_STEPPER_MOTOR:
            m_handlesStepper.push_back(PhidgetStepperHandle());
            PhidgetStepper_create(&m_handlesStepper.at(motor));
            setAddressProperties<PhidgetStepperHandle>(
                &m_handlesStepper.at(motor), ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor], ARM_MOTOR_PORT[motor]);

            // Add new empty encoder
            m_handlesEncoder.push_back(PhidgetEncoderHandle());
            break;
        default:
            Logging::logE(
                file,
                "Can not initialise motor at index: %d with motor type: %d",
                motor, motorTypes[motor]);

            break;
        }
    }

    // initialise claw
    Logging::logD(file, "Initialising Arm Claw");
    PhidgetRCServo_create(m_handleClaw);
    setAddressProperties<PhidgetRCServoHandle>(
        m_handleClaw, ARM_CLAW_SERIAL_NUMBER, ARM_CLAW_CHANNEL, ARM_CLAW_PORT);
}

Arm::~Arm() {
    // deinitialise motors in the arm
    Logging::logD(file, "Deinitialising Arm Motors");
    for (int motor = 0; motor < m_degOfFreedom; motor++) {
        switch (m_motorTypes[motor]) {
        case MOTOR_TYPE_DC_MOTOR:
            Phidget_close((PhidgetHandle)m_handlesDC.at(motor));
            PhidgetDCMotor_delete(&m_handlesDC.at(motor));
            break;

        case MOTOR_TYPE_STEPPER_MOTOR:
            Phidget_close((PhidgetHandle)m_handlesStepper.at(motor));
            PhidgetStepper_delete(&m_handlesStepper.at(motor));
            break;

        default:
            Logging::logE(file,
                          "Can not deinitialise for motor at index: %d with "
                          "motor type: %d",
                          motor, m_motorTypes[motor]);
            break;
        }
    }

    // deinitialise encoders
    Logging::logD(file, "Deinitialising Arm Encoders");
    for (int encoder = 0; encoder < m_degOfFreedom; encoder++) {
        Phidget_close((PhidgetHandle)m_handlesEncoder.at(encoder));
        PhidgetEncoder_delete(&m_handlesEncoder.at(encoder));
    }

    // deinitialise claw
    Logging::logD(file, "Deinitialising Arm Claw");
    Phidget_close((PhidgetHandle)*m_handleClaw);
    PhidgetRCServo_delete(m_handleClaw);
}

bool Arm::getArmMotorHandle(MotorHandlerReturn* retVal, int index) {
    switch (m_motorTypes[index]) {
    case MOTOR_TYPE_DC_MOTOR:
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->handler.dcMotor = &m_handlesDC.at(index);
        break;

    case MOTOR_TYPE_STEPPER_MOTOR:
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->handler.stepperMotor = &m_handlesStepper.at(index);
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
        retVal->handler.encoder = &m_handlesEncoder.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Arm::getArmClawHandle(MotorHandlerReturn* retVal) {
    retVal->type = MOTOR_TYPE_SERVO_MOTOR;
    retVal->handler.servoMotor = m_handleClaw;

    return true;
}

int Arm::getDOF() { return m_degOfFreedom; }