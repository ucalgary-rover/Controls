#ifndef ROVER_H
#define ROVER_H

#pragma once

#include "MessageQueue.h"
#include "Systems/Arm.h"
#include "Systems/Drive.h"
#include "Systems/SciTool.h"
#include "websocket.h"

class Rover {
public:
    // Regular Constructor
    Rover(Arm arm, Drive drive, SciTool sciTool);

    // Temporary websocket constructor (need feedback on this)
    Rover(Arm arm, Drive drive, SciTool sciTool, asio::io_context& context,
          const std::string& host, int port);

    ~Rover();

    // Need to discuss with Dawson whether or not we need getters and setters
    // Getters

    // Arm getArm();
    // SciTool getSciTool();
    // Drive getDrive();

    // MessageQueue getRoverQueue();
    // MessageQueue getArmQueue();
    // MessageQueue getDriveQueue();
    // MessageQueue getSciToolQueue();

    std::thread getRoverThreadPusher();
    std::thread getRoverThreadPopper();
    std::thread getArmThreadPusher();
    std::thread getArmThreadPopper();
    std::thread getDriveThreadPusher();
    std::thread getDriveThreadPopper();
    std::thread getSciToolThreadPusher();
    std::thread getSciToolThreadPopper();

    // // Setters (May not need these)
    // void setArm(Arm arm);
    // void setDrive(Drive drive);
    // void setSciTool(SciTool sciTool);

    /* Instantiate the drivers for the rover
     *
     * @param
     * arm: Arm - The arm driver
     * drive: Drive - The drive driver
     * sciTool: SciTool - The science tool driver
     *
     * @return
     * none
     */
    void instantiateDrivers(Arm arm, Drive drive, SciTool sciTool);

    /* Instantiate the queues for the rover, and create a pusher and popper
     * thread for each queue
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void instantiateQueues();

    /* Instantiate the websocket client for the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void instantiateWebsocket();

private:
    // Websocket client
    WebSocketClient wsClient;

    // Rover drivers
    Arm armDriver;
    Drive driveDriver;
    SciTool sciToolDriver;

    // Message queues
    MessageQueue roverQueue;
    MessageQueue armQueue;
    MessageQueue driveQueue;
    MessageQueue sciToolQueue;

    // Queue threads
    std::thread roverThreadPusher;
    std::thread roverThreadPopper;

    std::thread armThreadPusher;
    std::thread armThreadPopper;

    std::thread driveThreadPusher;
    std::thread driveThreadPopper;

    std::thread sciToolThreadPusher;
    std::thread sciToolThreadPopper;
};

#endif