#ifndef ROVER_H
#define ROVER_H

#pragma once

#include "MessageQueue.h"

#include "pub_general.h"
#include "pub_rover.h"

#include "Systems/Arm.h"
#include "Systems/Drive.h"

#include "Handlers/ArmHandler.h"

#include "websocket.h"

class Rover {
public:
    // Regular Constructor
    Rover(Arm arm, Drive drive);

    // Temporary websocket constructor (need feedback on this)
    Rover(Arm arm, Drive drive, asio::io_context& context,
          const std::string& host, int port);

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
    void instantiateThreads();

    /** Instantiate the websocket client for the rover
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void instantiateWebsocket(asio::io_context& context,
                              const std::string& host, int port);

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
    // Websocket client
    WebSocketClient m_wsClient;

    // Rover components
    Arm m_arm;
    Drive m_drive;

    // Rover Handlers
    ArmHandler m_armHandler;
    // DriveHandler driveHandler;

    // Message queues
    MessageQueue m_roverQueue;
    MessageQueue m_armQueue;
    MessageQueue m_driveQueue;

    // Threads
    std::thread m_roverQueueThread;
    std::thread m_startThread;
    std::thread m_stopThread;
    std::thread m_wsThread;

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