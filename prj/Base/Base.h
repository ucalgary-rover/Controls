#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"
#include "Base/ControllerHandler.h"
#include "DriveControllerLayout.h"
#include "Message.h"
#include "MessageQueue.h"
#include "MotorStateManager.h"
#include "RoverStateManager.h"
#include "mission_control.h"
#include "pub_general.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base() = default;

    // Start the loops to have Base working
    void start();

    // Exit the loops that base runs through
    void quit();

private:
    // Chassis state management
    RoverState defaultState = RoverState(); // Default Position of Rover
    RoverStateManager desiredStateManager;
    ArmMotorStateManager armManualChangeManager;

    // Variables for state of rover arm
    ArmMessageType armControlType;

    int exitLoop;

    void incrementJoint(int change);
    void changeArmControlType(ArmMessageType type);

    // Desired Motor State Update Methods
    DriveMotorState
    processDesiredDriveState(const DriveState& desiredDriveState);
    ArmMotorState processDesiredArmState(const ArmState& desiredArmState);
    MotorState processDesiredRoverState();

    // List of all controllers in use
    ControllerHandler* controller;

    DriveControllerLayout driveLayout;
    ArmManualControllerLayout armManualLayout;
    ArmFixedIKControllerLayout armFixedIKLayout;
    ArmVariableIKControllerLayout armVariableIKLayout;

    buttonFunctions* drive_control;
    buttonFunctions* arm_manual_control;
    buttonFunctions* arm_fixed_ik_control;
    buttonFunctions* arm_variable_ik_control;
};

#endif