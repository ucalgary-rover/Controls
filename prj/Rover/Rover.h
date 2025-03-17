#ifndef ROVER_H
#define ROVER_H

#pragma once

#include "MessageQueue.h"
#include "Systems/Arm.h"
#include "Systems/Drive.h"
#include "Systems/SciTool.h"

#include "Handlers/ArmHandler.h"

#include "websocket.h"

class Rover {
public:
    // Regular Constructor
    Rover(Arm arm, Drive drive, SciTool sciTool);

    // Temporary websocket constructor (need feedback on this)
    Rover(Arm arm, Drive drive, SciTool sciTool, asio::io_context& context,
          const std::string& host, int port);

    ~Rover();

    // Getters
    Arm getArmHandler() const;
    // NOT IMPLEMENTED YET
    // SciTool getSciToolHandler();
    // Drive getDriveHandler();

    /** Instantiate the systems for the rover
     *
     * @param
     * arm: Arm - The arm
     * drive: Drive - The drive r
     * sciTool: SciTool - The science tool
     *
     * @return
     * none
     */
    void instantiateSystems(Arm arm, Drive drive, SciTool sciTool);

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
    SciTool m_sciTool;

    // Rover Handlers
    ArmHandler m_armHandler;
    // DriveHandler driveHandler;
    // SciToolHandler sciToolHandler;

    // Message queues
    MessageQueue m_roverQueue;
    MessageQueue m_armQueue;
    MessageQueue m_driveQueue;
    MessageQueue m_sciToolQueue;

    // Threads
    std::thread m_roverQueueThread;
    std::thread m_startThread;
    std::thread m_wsThread;

    // Don't think I'll need these in Rover
    // std::thread m_armQueueThread;
    // std::thread m_driveQueueThread;
    // std::thread m_sciToolQueueThread;
};

#endif