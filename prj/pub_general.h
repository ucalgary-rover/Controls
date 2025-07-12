#ifndef PUB_GENERAL_H
#define PUB_GENERAL_H

#include <SDL2/SDL.h>
#include <functional>
#include "Logging.h"

#pragma once

#define BUILD_SIDE_BASE 1
#define BUILD_SIDE_ROVER 2

enum ExtentionType {
    EXTENTION_TYPE_ARM,
    EXTENTION_TYPE_SCIENCE_TOOL,
    EXTENTION_TYPE_NONE,
};

enum MessageFormat {
    MESSAGE_FORMAT_WHEEL,
    MESSAGE_FORMAT_ARM,
    MESSAGE_FORMAT_SCIENCE_TOOL,
};

enum MessagePriority {
    MESSAGE_PRIORITY_LOW,
    MESSAGE_PRIORITY_HIGH,
};

enum LogLevel {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,
};

// Used to streamline the struct (Don't keep in final)<<<<<<<<<<<<<<<<<<<<<<<<<<
typedef void (*ControllerFunc)(void* args);

// a struct for function pointers used for button mapping
// fill buttonArray with button functions
// Can treat the array as a dictionary and these values as keys
// SDL_CONTROLLER_BUTTON_A
// SDL_CONTROLLER_BUTTON_B
// SDL_CONTROLLER_BUTTON_X
// SDL_CONTROLLER_BUTTON_Y
// SDL_CONTROLLER_BUTTON_BACK
// SDL_CONTROLLER_BUTTON_GUIDE
// SDL_CONTROLLER_BUTTON_START
// SDL_CONTROLLER_BUTTON_LEFTSTICK
// SDL_CONTROLLER_BUTTON_RIGHTSTICK
// SDL_CONTROLLER_BUTTON_LEFTSHOULDER
// SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
// SDL_CONTROLLER_BUTTON_DPAD_UP
// SDL_CONTROLLER_BUTTON_DPAD_DOWN
// SDL_CONTROLLER_BUTTON_DPAD_LEFT
// SDL_CONTROLLER_BUTTON_DPAD_RIGHT
struct buttonFunctions {
    std::array<std::function<void()>, 15> buttonArray = { nullptr };
    void (*LEFT_JOYSTICK)(int xValue, int yValue) = nullptr;
    void (*RIGHT_JOYSTICK)(int xValue, int yValue) = nullptr;
};

// Generic Message Format
struct Generic {
    int value;
};

// Message format for wheels
struct WheelMessage {
    int velocity;
    int theta;
    int angleVelocity;
};

// Message format for arm

enum MotorID : int {
    MOTOR_ID_BASE,
    MOTOR_ID_SHOULDER,
    MOTOR_ID_ELBOW,
    MOTOR_ID_WRIST,
    MOTOR_ID_CLAW_ROLL,
    MOTOR_ID_CLAW_PITCH,
    MOTOR_ID_CLAW_OPEN,
    MOTOR_ID_END,
};

enum ArmMessageType : int {
    ARM_MESSAGE_TYPE_MANUAL,
    ARM_MESSAGE_TYPE_FIXED_IK,
    ARM_MESSAGE_TYPE_VARIABLE_IK,
};

struct ArmManualMessage {
    MotorID motorId;
    float angleChange;
};

struct ArmFixedIKMessage {
    float wristX;
    float wristY;
    float wristZ;

    float clawOpen;
};

struct ArmVariableIKMessage {
    float wristX;
    float wristY;
    float wristZ;

    float clawIncline;
    float clawTwist;

    float clawOpen;
};

struct ArmMessage {
    ArmMessageType type;
    union {
        ArmManualMessage manual_message;
        ArmFixedIKMessage fixed_ik_message;
        ArmVariableIKMessage variable_ik_message;
    };
};

// Message format for science tool
struct ScienceToolMessage {
    int moveUpDown;
    int moveLeftRight;

    int xPos;
    int yPos;
};

#endif