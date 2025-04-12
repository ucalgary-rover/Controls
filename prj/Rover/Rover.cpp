#include "Rover.h"

// Regular Constructor
Rover::Rover(Arm arm, Drive drive) {
    instantiateSystems(arm, drive);
    instantiateQueues();
    instantiateThreads();
    instantiateHandlers();
}

// Temp constructor for websockets
Rover::Rover(Arm arm, Drive drive, asio::io_context& context,
             const std::string& host, int port) {

    // Instantiate all neccasary components
    instantiateWebsocket(context, host, port);
    instantiateSystems(arm, drive);
    instantiateQueues();
    instantiateThreads();
    instantiateHandlers();
}

// Destructor
Rover::~Rover() {
    // Join all threads if joinable (destroy threads)
    if (m_roverQueueThread.joinable())
        m_roverQueueThread.join();
    if (m_startThread.joinable())
        m_startThread.join();
    if (m_stopThread.joinable())
        m_stopThread.join();
    if (m_wsThread.joinable())
        m_wsThread.join();
}

//---------------------- Start/Stop Functions ----------------------//
// Note: Only the start() and stop() functions have their own thread //

void Rover::start() {

    while (true) {
        std::unique_lock<std::mutex>
            lock; // Required for condition_variable to work

        // Wait to unpause if the thread is paused
        m_roverConVar.wait(lock, [this]() { return m_roverRunningFlag; });

        // main rover functionality
    }
}

void Rover::stop() {
    // Need to add some functionality to how we want to pause the rover and the
    // handlers (2025-04-12)

    // ex:
    /*
    if (controller_input = 'a') {
        stopAll();
    }
    else if (controller_input = 'b') {
        stopArmHandler();
    }
    else if (controller_input = 'c') {
        stopDriveHandler();
    }
    else if (controller_input = 'd') {
        resumeAll();
    }
    else if (controller_input = 'e') {
        resumeArmHandler();
    }
    else if (controller_input = 'f') {
        resumeDriveHandler();
    }
    */
}

// Pause all start() threads
void Rover::stopAll() {
    stopRover();
    stopArmHandler();
    stopDriveHandler();
}

// Pause Rover start() loop
void Rover::stopRover() {
    m_roverRunningFlag = false;
    m_roverConVar.notify_all(); // Notify the Rover loop to pause
}

// Pause ArmHandler start() loop
void Rover::stopArmHandler() {
    m_armRunningFlag = false;
    m_armConVar.notify_all(); // Notify the ArmHandler loop to pause
}

// Pause DriveHandler start() loop
void Rover::stopDriveHandler() {
    m_driveRunningFlag = false;
    m_driveConVar.notify_all(); // Notify the DriveHandler loop to pause
}

// Resume all threads
void Rover::resumeAll() {
    resumeRover();
    resumeArmHandler();
    resumeDriveHandler();
}

// Resume Rover start() loop
void Rover::resumeRover() {
    m_roverRunningFlag = true;
    m_roverConVar
        .notify_all(); // Notify the condition variable to resume the thread
}

// Resume ArmHandler start() loop
void Rover::resumeArmHandler() {
    m_armRunningFlag = true;
    m_armConVar
        .notify_all(); // Notify the condition variable to resume the thread
}

// Resume DriveHandler start() loop
void Rover::resumeDriveHandler() {
    m_driveRunningFlag = true;
    m_driveConVar
        .notify_all(); // Notify the condition variable to resume the thread
}

//---------------------- Instantiation Functions ----------------------//

// Websocket instantiation
// NOTE: I have no idea if this works. Will need to be fixed (2025-04-12)
void Rover::instantiateWebsocket(asio::io_context& context,
                                 const std::string& host, int port) {

    // Create websocket client object
    this->m_wsClient = WebSocketClient(context, host, port);

    // Create thread to constantly recieve messages
    m_wsThread
        = std::thread(&WebSocketClient::receiveMessages, std::ref(m_wsClient));
}

// System instantiation
void Rover::instantiateSystems(Arm arm, Drive drive) {
    this->m_arm = arm;
    this->m_drive = drive;
}

// Queue instantiation
void Rover::instantiateQueues() {
    this->m_roverQueue = MessageQueue();
    this->m_armQueue = MessageQueue();
    this->m_driveQueue = MessageQueue();
}

// Thread instantiation
void Rover::instantiateThreads() {

    // Instantiate threads for MessageQueues
    this->m_roverQueueThread = std::thread([queue = m_roverQueue]() {
        Message m;

        while (true) {
            // Get message from roverQueue
            m = queue->pop();

            // Push message to appropriate queue
            switch (message.getFormat()) {
            case MessageFormat::MESSAGE_FORMAT_ARM:
                m_armQueue.push(m);
                break;

            case MessageFormat::MESSAGE_FORMAT_WHEEL:
                m_driveQueue.push(m);
                break;

            default:
                break;
            }
        }
    });

    this->m_startThread = std::thread(&Rover::start, this);
    this->m_stopThread = std::thread(&Rover::stop, this);
}

// Handler instantiation
void Rover::instantiateHandlers() {
    this->m_armHandler
        = ArmHandler(m_arm, m_armQueue, m_armConVar, m_armRunningFlag);

    // NOTE IMPLEMENTED YET:
    // this->driveHandler = DriveHandler();
}

//---------------------- Getters ----------------------//

Arm Rover::getArmHandler() const { return this->m_armHandler; }

// NOT IMPLEMENTED YET
// Drive Rover::getDriveHandler() {
//     return this->driveHandler;
// }