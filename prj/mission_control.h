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
#define EXTENTION EXTENTION_TYPE_ARM
#define LOGGING_LEVEL LOG_LEVEL_VERBOSE

// Rover dimentions
#define ROVER_LENGTH 1.0 // in meters
#define ROVER_WIDTH 0.5  // in meters

// Configs for the arm
#define DEGREES_OF_FREEDOM 6

// Encoder configs
#define MAX_ENCODER_POSITIONS 1000

// Logging past motor positions
#define BASE_LAST_KNOWN_POS_FILE "Rover/Handlers/LastKnowPosBase.json"
#define ARM_LAST_KNOWN_POS_FILE "Rover/Handlers/LastKnowPosArm.json"

// Configuration file to set serial numbers and channels for each of the motors

// Following consts should follow the order of
// DriveMotorIndex in pub_rover.cpp
const int DRIVE_DC_SERIAL_NUMBER[4] = { 1000, 1000, 1000, 1000 };
const int DRIVE_DC_CHANNEL[4] = { 1, 1, 1, 1 };
const int DRIVE_STEPPER_SERIAL_NUMBER[4] = { 1000, 1000, 1000, 1000 };
const int DRIVE_STEPPER_CHANNEL[4] = { 1, 1, 1, 1 };
const int DRIVE_DIGITAL_INPUT_SERIAL_NUMBER[4] = { 1000, 1000, 1000, 1000 };
const int DRIVE_DIGITAL_INPUT_CHANNEL[4] = { 1, 1, 1, 1 };

const int ARM_MOTOR_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_MOTOR_CHANNEL[6] = { 1, 1, 1, 1, 1, 1 };
const int ARM_ENCODER_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_ENCODER_CHANNEL[6] = { 1, 1, 1, 1, 1, 1 };
const int ARM_CLAW_SERIAL_NUMBER = 1000;
const int ARM_CLAW_CHANNEL = 1;

// Websocet Definitions
const int WEBSOCKET_PORT = 8080;
const int WEBSOCKET_ADDR[4] = { 127, 0, 0, 1 };

#endif // MISSION_CONTROL_H