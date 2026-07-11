#ifndef PUB_GENERAL_H
#define PUB_GENERAL_H

#include "Logging.h"
#include <SDL2/SDL.h>
#include <functional>

#pragma once

enum MessageFormat : int {
    MESSAGE_FORMAT_MOTOR_STATE,
    MESSAGE_FORMAT_SCI_TOOL_DOOR,
    MESSAGE_FORMAT_SCI_TOOL_HEIGHT,
    MESSAGE_FORMAT_SCI_TOOL_BRUSH,
    MESSAGE_FORMAT_DRIVE_ZERO,
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

enum SciToolDoorControl : int {
    LEFT,
    MIDDLE,
    RIGHT,
};

// Message formats for science tool
struct SciToolDoorMessage {
    SciToolDoorControl door;
};

enum SciToolHeightControl : int {
    RAISE,
    LOWER,
    STOP,
};

struct SciToolHeightMessage {
    SciToolHeightControl control;
};

enum SciToolBrushControl : int {
    START_BRUSH,
    STOP_BRUSH,
};

struct SciToolBrushMessage {
    SciToolBrushControl control;
};

struct HeadlightMessage {
    int brightness;
};

struct DriveZeroMessage {
    bool set;
};

#endif