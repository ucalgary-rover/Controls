#include "phidget22.h"

T setAddressProperties(T* handle, int serialNum, int channel);

enum MOTOR_TYPE {
    DC_MOTOR,
    STEPPER_MOTOR,
    SERVO_MOTOR,

    // Additional motor types should be added above this comment
    MOTOR_TYPE_COUNT,
    INVALID_MOTOR_TYPE = MOTOR_TYPE_COUNT,
};

enum DriveMotorIndex {
    FRONT_LEFT,
    FRONT_RIGHT,
    BACK_LEFT,
    BACK_RIGHT,

    // Additional drive motors should be added above this comment
    WHEEL_COUNT,
};

struct MotorHandlerReturn {
    MOTOR_TYPE type;
    union motorHandle {
        PhidgetDCMotorHandle dcMotor;
        PhidgetStepperHandle stepperMotor;
        PhidgetServoHandle servo_motor;
    };
};
