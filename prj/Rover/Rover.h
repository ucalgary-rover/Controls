#ifndef ROVER_H
#define ROVER_H

#pragma once

#include "MessageQueue.h"
#include "Rover/Handlers/ArmHandler.h"
#include "Rover/Handlers/DriveHandler.h"
#include "Rover/Systems/Arm.h"
#include "Rover/Systems/Drive.h"
#include "pub_rover.h"
#include <thread>

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
    void startClient(MessageQueue* clientQueue, MessageQueue* armQueue,
                     MessageQueue* driveQueue);

    /** Start the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void start();
};

#endif