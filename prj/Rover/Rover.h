#ifndef ROVER_H
#define ROVER_H

#pragma once

#include "MessageQueue.h"

#include "Systems/Arm.h"
#include "Systems/Drive.h"
#include "pub_general.h"
#include "pub_rover.h"
#include <thread>

// #include "Handlers/ArmHandler.h"

#include "Websocket/WebsocketClient.h"

class Rover {
public:
    // Regular Constructor
    Rover();

    ~Rover();

    // Getters
    Arm getArmHandler() const;
    // NOT IMPLEMENTED YET
    // Drive getDriveHandler();

    /** Instantiate the systems for the rover
     *
     * @param
     * arm: Arm - The arm
     * drive: Drive - The drive r
     *
     * @return
     * none
     */
    void instantiateSystems(Arm arm, Drive drive);

    /** Instantiate handlers for each system on the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void instantiateHandlers();

    /** Instantiate the queues for the rover and the handlers
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void instantiateQueues();

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

    /** Stop the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void stop();

private:
    // Threading variables
    std::condition_variable m_roverConVar;
    bool m_roverRunningFlag = false;

    std::condition_variable m_armConVar;
    bool m_armRunningFlag = false;

    std::condition_variable m_driveConVar;
    bool m_driveRunningFlag = false;

    /** Stop the all threads for rover and handlers
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void stopAll();

    /** Pause the rover thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void stopRover();

    /** Pause the arm handler thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void stopArmHandler();

    /** Pause the drive handler thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void stopDriveHandler();

    /** Stop the all threads for rover and handlers
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void resumeAll();

    /** Pause the rover thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void resumeRover();

    /** Pause the arm handler thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void resumeArmHandler();

    /** Pause the drive handler thread
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void resumeDriveHandler();
};

#endif