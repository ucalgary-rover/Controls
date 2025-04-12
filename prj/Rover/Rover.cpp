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
void Rover::instantiateSystems(Arm arm, Drive drive) {
    this->m_arm = arm;
    this->m_drive = drive;
}

// Queue instantiation
void Rover::instantiateQueues() {
    // Instantiate the message queues
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