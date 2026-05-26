#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ArmControllerLayout.h"
#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"
#include "Base/ControllerHandler.h"
#include "Controller/ControllerLayout.h"
#include "DriveControllerLayout.h"
#include "Message.h"
#include "MessageQueue.h"
#include "MotorStateManager.h"
#include "RoverStateManager.h"
#include "UDPHandler.h"
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
    DriveMotorStateManager driveManualChangeManager;

    // Variables for state of rover arm
    ArmMessageType armControlType;

    int exitLoop;

    // Desired Motor State Update Methods
    DriveMotorState processDesiredDriveState(DriveState& desiredDriveState,
                                             DriveMotorState& currentState);
    ArmMotorState processDesiredArmState(const ArmState& desiredArmState);
    MotorState
    processDesiredRoverState(MotorStateManager& currentMotorStateManager);

    // UDP receiving
    void receive(UDPHandler& receiver,
                 MotorStateManager& currentMotorStateManager);
};
#endif