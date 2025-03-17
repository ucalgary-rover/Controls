#include "phidget22.h"

#pragma once

enum MotorType {
    MOTOR_TYPE_DC_MOTOR,
    MOTOR_TYPE_STEPPER_MOTOR,
    MOTOR_TYPE_SERVO_MOTOR,

    // Additional motor types should be added above this comment
    MOTOR_TYPE_COUNT,
    MOTOR_TYPE_INVALID = MOTOR_TYPE_COUNT,
};

enum DriveIndex {
    DRIVE_INDEX_FRONT_LEFT,
    DRIVE_INDEX_FRONT_RIGHT,
    DRIVE_INDEX_BACK_LEFT,
    DRIVE_INDEX_BACK_RIGHT,

    // Additional drive motors should be added above this comment
    DRIVE_INDEX_WHEEL_COUNT,
};

struct MotorHandlerReturn {
    MotorType type;
    union motorHandle {
        PhidgetDCMotorHandle* dcMotor;
        PhidgetStepperHandle* stepperMotor;
        PhidgetRCServoHandle* servo_motor;
    } motorHandle;
};
