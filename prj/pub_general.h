#ifndef PUB_GENERAL_H
#define PUB_GENERAL_H

#include "Logging.h"
#include <SDL2/SDL.h>
#include <functional>

#pragma once

enum MessageFormat : int {
    MESSAGE_FORMAT_MOTOR_STATE,
    MESSAGE_FORMAT_SCIENCE_TOOL,
    MESSAGE_FORMAT_GENERIC
};

enum MessagePriority : int {
    MESSAGE_PRIORITY_LOW,
    MESSAGE_PRIORITY_HIGH,
};

enum GameControllerAxis : int {
    GAME_CONTROLLER_AXIS_INVALID = -1,
    GAME_CONTROLLER_AXIS_LEFTX,
    GAME_CONTROLLER_AXIS_LEFTY,
    GAME_CONTROLLER_AXIS_RIGHTX,
    GAME_CONTROLLER_AXIS_RIGHTY,
    GAME_CONTROLLER_AXIS_LAST_JOYSTICK = GAME_CONTROLLER_AXIS_RIGHTY,
    GAME_CONTROLLER_AXIS_TRIGGERLEFT,
    GAME_CONTROLLER_AXIS_TRIGGERRIGHT,
    GAME_CONTROLLER_AXIS_MAX
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
    std::function<void(int, int)> LEFT_JOYSTICK = nullptr;
    std::function<void(int, int)> RIGHT_JOYSTICK = nullptr;
    std::function<void(int)> LEFT_TRIGGER = nullptr;
    std::function<void(int)> RIGHT_TRIGGER = nullptr;
};

// Generic Message Format
struct Generic {
    int value;
};

enum WheelID : int {
    WHEEL_FR,
    WHEEL_FL,
    WHEEL_BR,
    WHEEL_BL,
    WHEEL_COUNT,
};

// Message format for arm
enum ArmMessageType : int {
    ARM_MESSAGE_TYPE_HALT,
    ARM_MESSAGE_TYPE_MANUAL,
    ARM_MESSAGE_TYPE_FIXED_IK,
    ARM_MESSAGE_TYPE_VARIABLE_IK,
};

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

// Message format for science tool
struct ScienceToolMessage {
    int moveUpDown;
    int moveLeftRight;

    int xPos;
    int yPos;
};

#endif