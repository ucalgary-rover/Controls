#include "ArmHardware.h"

#include <unistd.h>

#include "Motors/LinearActuator.h"
#include "Motors/RCServo.h"
#include "Motors/Stepper.h"

static const char* file = "ArmHardware";

ArmHardware::ArmHardware() {
    // initialise motors in the ArmHardware
    Logging::logD(file, "Initialising ArmHardware Motors");

    motors[MOTOR_ID_BASE] = std::make_shared<Stepper>(0, 0, 0);
    motors[MOTOR_ID_SHOULDER]
        = std::make_shared<LinearActuator>(0, 0, 0, 0, 0, 0);
    motors[MOTOR_ID_ELBOW] = std::make_shared<LinearActuator>(0, 0, 0, 0, 0, 0);
    motors[MOTOR_ID_CLAW_ROLL] = std::make_shared<Stepper>(0, 0, 0);
    motors[MOTOR_ID_CLAW_PITCH] = std::make_shared<Stepper>(0, 0, 0);
    motors[MOTOR_ID_CLAW_OPEN] = std::make_shared<RCServo>(0, 0, 0);
}

ArmHardware::~ArmHardware() {
    // deinitialise motors in the ArmHardware
    Logging::logD(file, "Deinitialising ArmHardware Motors");

    motors = {};
}

void ArmHardware::setJointAngle(MotorID joint, float angle) {
    switch (joint) {
    case MotorID::MOTOR_ID_BASE:
    case MotorID::MOTOR_ID_SHOULDER:
    case MotorID::MOTOR_ID_ELBOW:
    case MotorID::MOTOR_ID_CLAW_OPEN:
        motors[joint]->setTargetPosition(angle);
        break;

    case MotorID::MOTOR_ID_CLAW_ROLL:
        while (clawInUse) {
            usleep(0.001);
        }

        clawInUse = true;

        // Actuate Claw
        motors[joint]->setTargetPosition(angle);
        break;

    case MotorID::MOTOR_ID_CLAW_PITCH:
        while (clawInUse) {
            usleep(0.001);
        }

        clawInUse = true;

        // Actuate Claw
        motors[joint]->setTargetPosition(angle);
        break;
    default:
        break;
    }
}

float ArmHardware::getJointAngle(MotorID joint) {
    switch (joint) {
    case MotorID::MOTOR_ID_BASE:
    case MotorID::MOTOR_ID_SHOULDER:
    case MotorID::MOTOR_ID_ELBOW:
    case MotorID::MOTOR_ID_CLAW_OPEN:
    case MotorID::MOTOR_ID_CLAW_ROLL:
    case MotorID::MOTOR_ID_CLAW_PITCH:
        return motors[joint]->getCurrentPosition();
    default:
        return 0;
    }
}
