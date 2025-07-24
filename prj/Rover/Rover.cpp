#include "Rover.h"

static const char* file = "Rover";

using namespace std;

// Regular Constructor
Rover::Rover() { }

// // Temp constructor for websockets
// Rover::Rover(Arm arm, Drive drive, asio::io_context& context,
//              const char* host, int port) {

//     // Instantiate all neccasary components
//     instantiateWebsocket(context, host, port);
//     instantiateSystems(arm, drive);
//     instantiateQueues();
//     instantiateThreads();
//     instantiateHandlers();
// }

// Destructor
Rover::~Rover() {
    // Join all threads if joinable (destroy threads)
    // if (m_roverQueueThread.joinable())
    //     m_roverQueueThread.join();
    // if (m_startThread.joinable())
    //     m_startThread.join();
    // if (m_stopThread.joinable())
    //     m_stopThread.join();
    // if (m_wsThread.joinable())
    //     m_wsThread.join();
}

//---------------------- Start/Stop Functions ----------------------//
// Note: Only the start() and stop() functions have their own thread //

void Rover::start() {
    char address[16];
    snprintf(address, sizeof(address), "%d.%d.%d.%d", WEBSOCKET_ADDR[0],
             WEBSOCKET_ADDR[1], WEBSOCKET_ADDR[2], WEBSOCKET_ADDR[3]);
    WebSocketClient client(address, std::to_string(WEBSOCKET_PORT));

    // Instantiate the systems for the rover
#if EXTENTION_TYPE_ARM
    const MotorType* motorTypes = [
        MOTOR_TYPE_STEPPER_MOTOR, MOTOR_TYPE_DC_MOTOR, MOTOR_TYPE_DC_MOTOR,
        MOTOR_TYPE_STEPPER_MOTOR,
        MOTOR_TYPE_STEPPER_MOTOR
    ] Arm arm(6, MOTOR_TYPE_STEPPER_MOTOR);
#endif
    Drive drive(ROVER_WIDTH, ROVER_LENGTH);

    // Create queues for rover, arm, and drive
    MessageQueue roverQueue;
    MessageQueue armQueue;
    MessageQueue driveQueue;

    // instantiate handlers
    DriveHandler driveHandler(&drive, &driveQueue);

    // Start the client thread
    thread websocetClientThread(
        [&]() { startClient(&roverQueue, &driveQueue, &armQueue); });

    // start thread for handlers
    thread driveHandlerThread([&]() { driveHandler.start(); });
#if EXTENTION_TYPE_ARM
    thread armHandlerThread[&]() { armHandler.start(); });
#endif

    client.connect();
    while (true) {
        Message reply = client.receive();
        reply.printMessage(); // Print the received message
        roverQueue.push(reply);
    }

    driveHandlerThread.join();
    websocetClientThread.join();
#if EXTENTION_TYPE_ARM
    armHandlerThread.join();
#endif
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

// Thread instantiation
void Rover::startClient(MessageQueue* clientQueue, MessageQueue* armQueue,
                        MessageQueue* driveQueue) {

    Message message;

    while (true) {
        // Get message from roverQueue
        message = clientQueue->pop();

        // Push message to appropriate queue
        switch (message.getFormat()) {
#if EXTENTION_TYPE_ARM
        case MESSAGE_FORMAT_ARM:
            armQueue->push(message);
            message.printMessage(); // Print the received message
            break;
#endif
        case MESSAGE_FORMAT_WHEEL:
            driveQueue->push(message);
            message.printMessage(); // Print the received message
            break;

        default:
            Logging::logE(file,
                          "Recieved message for un-used message format of %d",
                          message.getFormat());
            break;
        }
    }
}

// Handler instantiation
// void Rover::instantiateHandlers() {
//     this->m_armHandler
//         = ArmHandler(m_arm, m_armQueue, m_armConVar, m_armRunningFlag);

//     // NOTE IMPLEMENTED YET:
//     // this->driveHandler = DriveHandler();
// }

//---------------------- Getters ----------------------//

// Arm Rover::getArmHandler() const { return this->m_armHandler; }

// NOT IMPLEMENTED YET
// Drive Rover::getDriveHandler() {
//     return this->driveHandler;
// }