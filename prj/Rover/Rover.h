#ifndef ROVER_H
#define ROVER_H

#pragma once

#include <thread>

#include "MessageQueue.h"
#include "MotorStateManager.h"
// #include "Rover/Handlers/ArmHandler.h"
#include "Rover/Handlers/DriveHandler.h"
#include "Rover/Systems/Arm.h"
#include "Rover/Systems/Drive.h"
#include "pub_rover.h"

// #include "Handlers/ArmHandler.h"

class Rover {
public:
    // Regular Constructor
    Rover() = default;

    // Destructor
    ~Rover() = default;

    /** Instantiate the threads for each queue
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void startClient(MessageQueue* clientQueue, DriveHandler* driveHandler);

    /** Start the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void start();

private:
    // Chassis state management
    MotorState defaultState = MotorState(); // Default Position of Rover
    MotorStateManager desiredStateManager;
    MotorStateManager currentStateManager;
    DriveMotorStateManager* desiredDriveMotorStateManager;
};

#endif