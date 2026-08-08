#include "ArmHardware.h"

#include <unistd.h>

#include "Motors/LinearActuator.h"
#include "Motors/RCServo.h"
#include "Motors/Stepper.h"

static const char* file = "ArmHardware";

#define SHOULDER_KP 0
#define SHOULDER_KI 0
#define SHOULDER_KD 0

#define ELBOW_KP 0
#define ELBOW_KI 0
#define ELBOW_KD 0

ArmHardware::ArmHardware() {
    // initialise motors in the ArmHardware
    Logging::logD(file, "Initialising ArmHardware Motors");

    auto stepper1 = std::make_shared<Stepper>(0, 0, 0);
    auto stepper2 = std::make_shared<Stepper>(0, 0, 0);

    motors[MOTOR_ID_BASE] = std::make_shared<Stepper>(0, 0, 0);
    motors[MOTOR_ID_SHOULDER] = std::make_shared<LinearActuator>(
        0, 0, 0, 0, 0, 0, SHOULDER_KP, SHOULDER_KI, SHOULDER_KD);
    motors[MOTOR_ID_ELBOW] = std::make_shared<LinearActuator>(
        0, 0, 0, 0, 0, 0, ELBOW_KP, ELBOW_KI, ELBOW_KD);
    motors[MOTOR_ID_CLAW_ROLL] = stepper1;
    motors[MOTOR_ID_CLAW_PITCH] = stepper2;
    motors[MOTOR_ID_CLAW_OPEN]
        = std::make_shared<RCServo>(0, 0, 0, -124234, 234235);

    mrMoraleAndTheBigSteppers
        = std::make_shared<DifferentialClaw>(stepper1, stepper2);
}

ArmHardware::~ArmHardware() {
    // deinitialise motors in the ArmHardware
    Logging::logD(file, "Deinitialising ArmHardware Motors");

    motors = {};
}

void ArmHardware::setJointAngle(MotorID joint, float angle) {
    switch (joint) {
    case MotorID::MOTOR_ID_BASE:
        motors[joint]->setTargetPosition(angle);
        break;
    case MotorID::MOTOR_ID_SHOULDER:
    case MotorID::MOTOR_ID_ELBOW:
        motors[joint]->setTargetPosition(angle);
        break;
    case MotorID::MOTOR_ID_CLAW_OPEN:
        motors[joint]->setTargetPosition(angle);
        break;

    case MotorID::MOTOR_ID_CLAW_ROLL:
        // Actuate Claw
        mrMoraleAndTheBigSteppers->setTargetPitch(angle);
        break;

    case MotorID::MOTOR_ID_CLAW_PITCH:
        mrMoraleAndTheBigSteppers->setTargetRoll(angle);
        break;
    default:
        break;
    }
}

float ArmHardware::getJointAngle(MotorID joint) {
    switch (joint) {
    case MotorID::MOTOR_ID_BASE:
        return motors[joint]->getCurrentPosition();
    case MotorID::MOTOR_ID_SHOULDER:
    case MotorID::MOTOR_ID_ELBOW:
        return motors[joint]->getCurrentPosition();
    case MotorID::MOTOR_ID_CLAW_OPEN:
        return motors[joint]->getCurrentPosition();
    case MotorID::MOTOR_ID_CLAW_ROLL:
        return mrMoraleAndTheBigSteppers->getCurrentPitch();
    case MotorID::MOTOR_ID_CLAW_PITCH:
        return mrMoraleAndTheBigSteppers->getCurrentRoll();
    default:
        return 0;
    }
}

void ArmHardware::setZero() {
    motors[MOTOR_ID_BASE]->setZeroPosition();
    mrMoraleAndTheBigSteppers->setZero();
};