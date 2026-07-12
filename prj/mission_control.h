#ifndef MISSION_CONTROL_H
#define MISSION_CONTROL_H

#pragma once

#define EXTENTION_TYPE_ARM 0
#define EXTENTION_TYPE_SCI_TOOL 1
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
#define LOGGING_LEVEL LOG_LEVEL_DEBUG

// Hardware Enabled
#define ARM_HARDWARE_TEST true
#define DRIVE_HARDWARE_TEST true

// System Update Rates
#define ARM_UPDATE_INTERVAL_US 50 * 1000   // 50ms
#define DRIVE_UPDATE_INTERVAL_US 50 * 1000 // 50ms

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

// Allow inaccuracy in sensor data for drive systems to still be parsed correctly
#define DRIVE_THRESHOLD 0.1 // TODO: configure
#define STEER_THRESHOLD 1.0 // TODO: configure

// Configuration file to set serial numbers and channels for each of the motors

// Following consts should follow the order of
// DriveMotorIndex in pub_rover.cpp -> FL, FR, BL, BR
const int DRIVE_DC_SERIAL_NUMBER[4] = { 697178, 697178, 697270, 697270 };
const int DRIVE_DC_CHANNEL[4] = { 0, 0, 0, 0 };
const int DRIVE_DC_PORT[4] = { 5, 2, 4, 2 };
const int DRIVE_STEPPER_SERIAL_NUMBER[4] = { 697178, 697178, 697270, 697270 };
const int DRIVE_STEPPER_CHANNEL[4] = { 0, 0, 0, 0 };
const int DRIVE_STEPPER_PORT[4] = { 4, 1, 5, 1 };
const int DRIVE_ENCODER_SERIAL_NUMBER[4] = { 697178, 697178, 697270, 697270 };
const int DRIVE_ENCODER_CHANNEL[4] = { 0, 0, 0, 0 };
const int DRIVE_ENCODER_PORT[4] = { 3, 0, 3, 0 };

const int ARM_MOTOR_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_MOTOR_CHANNEL[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_MOTOR_PORT[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_ENCODER_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_ENCODER_CHANNEL[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_ENCODER_PORT[6] = { 0, 0, 0, 0, 0, 0 };
const int ARM_CLAW_SERIAL_NUMBER = 1000;
const int ARM_CLAW_CHANNEL = 0;
const int ARM_CLAW_PORT = 0;

// UDP Definitions
const int ROVER_PORT = 8080; // For on the Nuc: 5995
const int BASE_PORT = 8008;

#endif // MISSION_CONTROL_H