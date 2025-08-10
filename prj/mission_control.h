#ifndef MISSION_CONTROL_H
#define MISSION_CONTROL_H

#pragma once

#define EXTENTION_TYPE_ARM 0
#define EXTENTION_TYPE_SCIENCE_TOOL 1
#define EXTENTION_TYPE_NONE 2

#define BUILD_SIDE_BASE 1
#define BUILD_SIDE_ROVER 2

// Logging level:
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_VERBOSE 4

#ifndef SIDE_TO_BUILD
#define SIDE_TO_BUILD BUILD_SIDE_BASE
#endif // SIDE_TO_BUILD
#define EXTENTION EXTENTION_TYPE_NONE
#define LOGGING_LEVEL LOG_LEVEL_DEBUG

// Rover dimentions
#define ROVER_LENGTH 1.0 // in meters
#define ROVER_WIDTH 0.5  // in meters

// Configs for the arm
#define DEGREES_OF_FREEDOM 6

// Encoder configs
#define MAX_ENCODER_POSITIONS 1000

#define NO_MESSAGE_RECIEVED_TIMEOUT 5 // in seconds

// Holds settings for the deadzone sizes
//  any number less than ~30000
//  5000 is a moderate rate
#define MOVING_DEADZONE 3000
#define STATIC_DEADZONE 5000

// Logging past motor positions
#define BASE_LAST_KNOWN_POS_FILE                                               \
    "/home/dawsonvv/Documents/SSRT/Controls/prj/Rover/Handlers/"               \
    "LastKnowPosBase.json"
#define ARM_LAST_KNOWN_POS_FILE                                                \
    "/home/dawsonvv/Documents/SSRT/Controls/prj/Rover/Handlers/"               \
    "LastKnowPosArm.json"

// Configuration file to set serial numbers and channels for each of the motors

// Following consts should follow the order of
// DriveMotorIndex in pub_rover.cpp
const int DRIVE_DC_SERIAL_NUMBER[4] = { 697178, 697178, 697178, 697178 };
const int DRIVE_DC_CHANNEL[4] = { 0, 0, 0, 0 };
const int DRIVE_DC_PORT[4] = { 4, 0, 5, 1 };
const int DRIVE_STEPPER_SERIAL_NUMBER[4] = { 697270, 697270, 697270, 697270 };
const int DRIVE_STEPPER_CHANNEL[4] = { 0, 0, 0, 0 };
const int DRIVE_STEPPER_PORT[4] = { 1, 4, 2, 5 };
const int DRIVE_DIGITAL_INPUT_SERIAL_NUMBER = 697270;
const int DRIVE_DIGITAL_INPUT_CHANNEL[4] = { 0, 1, 2, 3 };
const int DRIVE_DIGITAL_INPUT_PORT = 3;

const int ARM_MOTOR_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_MOTOR_CHANNEL[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_MOTOR_PORT[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_ENCODER_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_ENCODER_CHANNEL[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_ENCODER_PORT[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_CLAW_SERIAL_NUMBER = 1000;
const int ARM_CLAW_CHANNEL = 0;
const int ARM_CLAW_PORT = 0;

// Websocet Definitions
const int WEBSOCKET_PORT = 8080; // For on the nook: 5995
const int WEBSOCKET_ADDR[4]
    = { 127, 0, 0, 1 }; // For on the nook: { 0, 0, 0, 0 }

#endif // MISSION_CONTROL_H