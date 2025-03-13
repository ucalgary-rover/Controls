#include "Arm.h"

static T setAddressProperties(T* handle, int serialNum, int channel);

Arm::Arm(int degOfFreedom, MOTOR_TYPE* motorTypes) :
    m_degOfFreedom(degOfFreedom), m_motorTypes(motorTypes) {
    m_handlesDC = new PhidgetDCMotorHandle*[degOfFreedom]();
    m_handlesStepper = new PhidgetStepperHandle*[degOfFreedom]();

    // initialise motors in the arm
    for (int motor = 0; motor < degOfFreedom; motor++) {
        switch (motorTypes[motor]) {
        case DC_MOTOR:
            PhidgetDCMotor_create(m_handlesDC[motor]);
            setAddressProperties<PhidgetDCMotorHandle>(
                m_handlesDC[motor], ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor]);

            break;
        case STEPPER_MOTOR:
            PhidgetStepper_create(m_handlesStepper[motor]);
            setAddressProperties<PhidgetStepperHandle>(
                m_handlesStepper[motor], ARM_MOTOR_SERIAL_NUMBER[motor],
                ARM_MOTOR_CHANNEL[motor]);

            break;
        default:
            std::cout << "Can not initialise motor at index: " << motor
                      << "with motor type: " << motorTypes[motor];

            break;
        }
    }

    // initialise claw
    PhidgetServo_create(m_handleClaw);
    setAddressProperties<PhidgetServoHandle>(
        m_handleClaw, ARM_CLAW_SERIAL_NUMBER, ARM_CLAW_CHANNELR);
}

Arm::~Arm() {
    // deinitialise motors in the arm
    for (int motor = 0; motor < degOfFreedom; motor++) {
        switch (motorTypes[motor]) {
        case DC_MOTOR:
            Phidget_close((PhidgetHandle)*m_handlesDC[motor]);
            PhidgetDCMotor_delete(m_handlesDC[motor]);
            break;

        case STEPPER_MOTOR:
            Phidget_close((PhidgetHandle)*m_handlesStepper[motor]);
            PhidgetStepper_delete(m_handlesStepper[motor]);
            break;

        default:
            std::cout << "Can not deinitialise for motor at index: " << motor
                      << "with motor type: " << motorTypes[motor];
            break;
        }
    }

    // deinitialise claw
    Phidget_close((PhidgetHandle)*m_handleClaw);
    PhidgetServo_delete(m_handleClaw);
}

bool Arm::getArmMotorHandle(MotorHandlerReturn* retVal, int index) {
    switch (motorTypes[index]) {
    case DC_MOTOR:
        retVal->type = DC_MOTOR;
        retVal->dcMotor = m_handlesDC[index];
        break;

    case STEPPER_MOTOR:
        retVal->type = STEPPER_MOTOR;
        retVal->stepperMotor = m_handlesStepper[index];
        break;

    default:
        retVal.type = INVALID_MOTOR_TYPE;
        std::cout << "Can not get handler for motor at index: " << index
                  << "with motor type: " << motorTypes[index];
        return false;
        break;
    };

    return true;
}

bool Arm::getArmClawHandle(MotorHandlerReturn* retVal) {
    retVal->type = SERVO_MOTOR;
    retVal->servo_motor = m_handleClaw;

    return true;
}

int Arm::getDOF() { return m_degOfFreedom; }

static template <typename T>
void setAddressProperties(T* handle, int serialNum, int channel) {
    Phidget_setDeviceSerialNumber((PhidgetHandle)*handle, serialNum);
    Phidget_setIsHubPortDevice((PhidgetHandle)*handle, 1);
    Phidget_setHubPort((PhidgetHandle)*handle, channel);
    Phidget_setOnAttachHandler((PhidgetHandle)*handle, onAttachHandler, NULL);
    Phidget_openWaitForAttachment((PhidgetHandle)*handle, 5000);
}