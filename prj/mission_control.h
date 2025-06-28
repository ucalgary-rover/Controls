#include "pub_general.h"

#pragma once

#define EXTENTION EXTENTION_TYPE_NONE
// #define SIDE_TO_BUILD BUILD_SIDE_BASE

// Configuration file to set serial numbers and channels for each of the motors

// Following consts should follow the order of
// DriveMotorIndex in pub_rover.cpp
const int DRIVE_DC_SERIAL_NUMBER[4] = { 1000, 1000, 1000, 1000 };
const int DRIVE_DC_CHANNEL[4] = { 1, 1, 1, 1 };
const int DRIVE_STEPPER_SERIAL_NUMBER[4] = { 1000, 1000, 1000, 1000 };
const int DRIVE_STEPPER_CHANNEL[4] = { 1, 1, 1, 1 };

const int ARM_MOTOR_SERIAL_NUMBER[6] = { 1000, 1000, 1000, 1000, 1000, 1000 };
const int ARM_MOTOR_CHANNEL[6] = { 1, 1, 1, 1, 1, 1 };
const int ARM_CLAW_SERIAL_NUMBER = 1000;
const int ARM_CLAW_CHANNEL = 1;


// Websocet Definitions
const int WEBSOCKET_PORT = 8080;
const uint8_t WEBSOCKET_ADDR[4] = {127, 0, 0, 1};