#include "Rover.h"

// Regular Constructor
Rover::Rover(Arm arm, Drive drive, SciTool sciTool) {
    instantiateDrivers(arm, drive, sciTool);
    instantiateQueues();
}

// Temp constructor for websockets
Rover::Rover(Arm arm, Drive drive, SciTool sciTool, asio::io_context& context,
             const std::string& host, int port) {
    instantiateDrivers(arm, drive, sciTool);
    instantiateQueues();
    instantiateWebsocket();
}

Rover::~Rover() { }

// Driver instantiation
void Rover::instantiateDrivers(Arm arm, Drive drive, SciTool sciTool) {
    this->armDriver = arm;
    this->driveDriver = drive;
    this->sciToolDriver = sciTool;
}

// Queue instantiation
void Rover::instantiateQueues() {
    // Instantiate the message queues
    this->roverQueue = MessageQueue();
    this->armQueue = MessageQueue();
    this->driveQueue = MessageQueue();
    this->sciToolQueue = MessageQueue();

    // Instantiate pusher and popper threads
    this->roverThreadPusher = std::thread(&MessageQueue::push, &roverQueue);
    this->roverThreadPopper = std::thread(&MessageQueue::pop, &roverQueue);

    this->armThreadPusher = std::thread(&MessageQueue::push, &armQueue);
    this->armThreadPopper = std::thread(&MessageQueue::pop, &armQueue);

    this->driveThreadPusher = std::thread(&MessageQueue::push, &driveQueue);
    this->driveThreadPopper = std::thread(&MessageQueue::pop, &driveQueue);

    this->sciToolThreadPusher = std::thread(&MessageQueue::push, &sciToolQueue);
    this->sciToolThreadPopper = std::thread(&MessageQueue::pop, &sciToolQueue);
}

// Websocket instantiation
void Rover::instantiateWebsocket(asio::io_context& context,
                                 const std::string& host, int port) {
    this->wsClient = WebSocketClient(context, host, port);
}

// Getters
// Arm Rover::getArm() {
//     return this->armDriver;
// }

// Drive Rover::getDrive() {
//     return this->driveDriver;
// }

// SciTool Rover::getSciTool() {
//     return this->sciToolDriver;
// }

// MessageQueue Rover::getRoverQueue() {
//     return this->roverQueue;
// }

// MessageQueue Rover::getArmQueue() {
//     return this->armQueue;
// }

// MessageQueue Rover::getDriveQueue() {
//     return this->driveQueue;
// }

// MessageQueue Rover::getSciToolQueue() {
//     return this->sciToolQueue;
// }

std::thread Rover::getRoverThreadPusher() { return this->roverThreadPusher; }

std::thread Rover::getRoverThreadPopper() { return this->roverThreadPopper; }

std::thread Rover::getArmThreadPusher() { return this->armThreadPusher; }

std::thread Rover::getArmThreadPopper() { return this->armThreadPopper; }

std::thread Rover::getDriveThreadPusher() { return this->driveThreadPusher; }

std::thread Rover::getDriveThreadPopper() { return this->driveThreadPopper; }

std::thread Rover::getSciToolThreadPusher() {
    return this->sciToolThreadPusher;
}

std::thread Rover::getSciToolThreadPopper() {
    return this->sciToolThreadPopper;
}

// // Setters (May not need these)
// void Rover::setArm(Arm arm) {
//     this->armDriver = arm;
// }

// void Rover::setDrive(Drive drive) {
//     this->driveDriver = drive;
// }

// void Rover::setSciTool(SciTool sciTool) {
//     this->sciToolDriver = sciTool;
// }
