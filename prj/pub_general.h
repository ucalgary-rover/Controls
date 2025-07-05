#ifndef PUB_GENERAL_H
#define PUB_GENERAL_H

#include <SDL2/SDL.h>
#include <functional>
#include <iostream>

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
    int angle_velocity;
};

// Message format for arm

enum MotorID : int {
    BASE,
    SHOULDER,
    ELBOW,
    WRIST,
    CLAW_ROLL,
    CLAW_PITCH,
    CLAW_OPEN,
};

enum ArmMessageType : int {
    MANUAL,
    FIXED_IK,
    VARIABLE_IK,
};

struct ArmManualMessage {
    MotorID motorId;
    float angle_change;
};

struct ArmFixedIKMessage {
    float wrist_x;
    float wrist_y;
    float wrist_z;

    float claw_open;
};

struct ArmVariableIKMessage {
    float wrist_x;
    float wrist_y;
    float wrist_z;

    float claw_incline;
    float claw_twist;

    float claw_open;
};

struct ArmMessage {
    ArmMessageType type;
    union {
        ArmManualMessage manual_message;
        ArmFixedIKMessage fixed_ik_message;
        ArmVariableIKMessage variable_ik_message;
    }
};

// Message format for science tool
struct ScienceToolMessage {
    int moveUpDown;
    int moveLeftRight;

    int xPos;
    int yPos;
};

#endif