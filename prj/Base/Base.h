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
#include "DriveControllerLayout.h"
#include "Message.h"
#include "MessageQueue.h"
#include "MotorStateManager.h"
#include "RoverStateManager.h"
#include "Websocket/WebsocketServer.h"
#include "mission_control.h"
#include "pub_general.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base();

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

    // List of all controllers in use
    ControllerHandler* controller;

    DriveControllerLayout driveLayout;
    ArmControllerLayout armLayout;
};
#endif