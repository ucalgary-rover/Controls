#ifndef PUB_GENERAL_H
#define PUB_GENERAL_H

#include <SDL2/SDL.h>
#include <functional>
#include "Logging.h"

#pragma once

enum MessageFormat {
    MESSAGE_FORMAT_WHEEL,
    MESSAGE_FORMAT_ARM,
    MESSAGE_FORMAT_SCIENCE_TOOL,
};

enum MessagePriority {
    MESSAGE_PRIORITY_LOW,
    MESSAGE_PRIORITY_HIGH,
};

enum GameControllerAxis
{
    GAME_CONTROLLER_AXIS_INVALID = -1,
    GAME_CONTROLLER_AXIS_LEFTX,
    GAME_CONTROLLER_AXIS_LEFTY,
    GAME_CONTROLLER_AXIS_RIGHTX,
    GAME_CONTROLLER_AXIS_RIGHTY,
    GAME_CONTROLLER_AXIS_LAST_JOYSTICK = GAME_CONTROLLER_AXIS_RIGHTY,
    GAME_CONTROLLER_AXIS_TRIGGERLEFT,
    GAME_CONTROLLER_AXIS_TRIGGERRIGHT,
    GAME_CONTROLLER_AXIS_MAX
} ;

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
    void (*LEFT_TRIGGER)(int xValue) = nullptr;
    void (*RIGHT_TRIGGER)(int xValue) = nullptr;
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

    float clawPitch;
    float clawRoll;

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