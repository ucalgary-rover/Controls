#include "Rover.h"

// Regular Constructor
Rover::Rover(Arm arm, Drive drive, SciTool sciTool) {
    instantiateSystems(arm, drive, sciTool);
    instantiateQueues();
    instantiateThreads();
    instantiateHandlers();
}

// Temp constructor for websockets
Rover::Rover(Arm arm, Drive drive, SciTool sciTool, asio::io_context& context,
             const std::string& host, int port) {

    // Instantiate all neccasary components
    instantiateWebsocket(context, host, port);
    instantiateSystems(arm, drive, sciTool);
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
    if (m_wsThread.joinable())
        m_wsThread.join();
}

//---------------------- Start/Stop Functions ----------------------//

void Rover::start() {
    while (true) {
        // do rover shit
    }
}

void Rover::stop() {
    // stop the rover
}

//---------------------- Instantiation Functions ----------------------//

// Websocket instantiation
void Rover::instantiateWebsocket(asio::io_context& context,
                                 const std::string& host, int port) {

    // Create websocket client object
    this->m_wsClient = WebSocketClient(context, host, port);

    // Create thread to constantly recieve messages
    m_wsThread
        = std::thread(&WebSocketClient::receiveMessages, std::ref(m_wsClient));
}

// Driver instantiation
void Rover::instantiateSystems(Arm arm, Drive drive, SciTool sciTool) {
    this->m_armDriver = arm;
    this->m_driveDriver = drive;
    this->m_sciToolDriver = sciTool;
}

// Queue instantiation
void Rover::instantiateQueues() {
    // Instantiate the message queues
    this->m_roverQueue = MessageQueue();
    this->m_armQueue = MessageQueue();
    this->m_driveQueue = MessageQueue();
    this->m_sciToolQueue = MessageQueue();
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
            case MessageFormat::ARM:
                m_armQueue.push(m);
                break;
            case MessageFormat::WHEEL:
                m_driveQueue.push(m);
                break;
            case MessageFormat::SCIENCE_TOOL:
                m_sciToolQueue.push(m);
                break;
            default:
                break;
            }
        }
    });

    this->m_startThread = std::thread(&Rover::start, this);

    // Maybe create these in their handlers?
    // this->m_armQueueThread = std::thread([queue = &m_armQueue]());
    // this->m_driveQueueThread = std::thread([queue = &m_driveQueue]());
    // this->m_sciToolQueueThread = std::thread([queue = &m_sciToolQueue]());
}

// Handler instantiation
void Rover::instantiateHandlers() {
    this->m_armHandler = ArmHandler(
        m_arm, m_armQueue); // Pass in arm object and a reference to m_armQueue

    // NOTE IMPLEMENTED YET:
    // this->driveHandler = DriveHandler();
    // this->sciToolHandler = SciToolHandler();
}

//---------------------- Getters ----------------------//

Arm Rover::getArmHandler() const { return this->m_armHandler; }

// NOT IMPLEMENTED YET
// Drive Rover::getDriveHandler() {
//     return this->driveHandler;
// }

// NOT IMPLEMENTED YET
// SciTool Rover::getSciToolHandler() {
//     return this->sciToolHandler;
// }
