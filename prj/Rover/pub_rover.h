#include "phidget22.h"
#include "pub_general.h"

#pragma once

enum MotorType {
    MOTOR_TYPE_DC_MOTOR,
    MOTOR_TYPE_STEPPER_MOTOR,
    MOTOR_TYPE_SERVO_MOTOR,
    MOTOR_TYPE_DIGITAL_INPUT,
    MOTOR_TYPE_ENCODER,

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
    union handler {
        PhidgetDCMotorHandle* dcMotor;
        PhidgetStepperHandle* stepperMotor;
        PhidgetRCServoHandle* servo_motor;
        PhidgetDigitalInputHandle* digitalInput;
        PhidgetEncoderHandle* encoder;
    } handler;
};
