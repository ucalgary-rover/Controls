// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base.h"

#include <chrono>
#include <cmath>
#include <unistd.h>
#include <vector>

#include "ArmControllerLayout.h"
#include "DriveControllerLayout.h"
#include "UDPHandler.h"

static const char* file = "Base";

// Chassis state management
std::shared_ptr<ArmProcessor> Base::armProcessor;
std::shared_ptr<DriveProcessor> Base::driveProcessor;
std::shared_ptr<MessageQueue<Message>> Base::sendQueue;
bool Base::exitLoop = false;

void Base::initialize() {
    Logging::logI(file, "Initializing Base");

    // Initialize Rover State Manager
    Logging::logI(file, "Initializing State Managers");
    armProcessor = std::make_shared<ArmProcessor>(defaultArmMotorState);
    driveProcessor = std::make_shared<DriveProcessor>(defaultDriveMotorState);

    // Initialize controller layouts
    Logging::logI(file, "Initializing Controller Layouts");
    exitLoop = 0;

    std::vector<std::shared_ptr<ControllerLayout>> controllers = {};

    controllers.push_back(std::make_shared<DriveControllerLayout>(
        driveProcessor, sendHeadlightsMessage, sendZeroMessage,
        sendCameraServoMessage));
#if EXTENSION == EXTENSION_TYPE_ARM
    controllers.push_back(std::make_shared<ArmControllerLayout>(armProcessor));
#elif EXTENSION == EXTENSION_TYPE_SCI_TOOL
    controllers.push_back(
        std::make_shared<SciToolControllerLayout>(sendSciToolMessage));
#endif
    ControllerHandler::initialize(controllers);

    Logging::logI(file, "Initializing Base done");
}

void Base::quit() { exitLoop = true; }

void Base::receive(UDPHandler& receiver) {
    while (true) {
        Message reply = receiver.receive();
        reply.printMessage(); // Print the received message

        MotorState newState = std::get<MotorState>(reply.getPayload());

        armProcessor->setRoverState(newState.armMotorState);
        driveProcessor->setRoverState(newState.driveMotorState);
    }
}

void Base::start() {
    Logging::logI(file, "Starting Base");
    auto sendQueue = std::make_shared<MessageQueue<Message>>();
    UDPHandler server(BASE_PORT, ROVER_PORT);

    std::thread controllerThread([&]() { ControllerHandler::eventLoop(); });
    std::thread sendingThread([&]() { server.run(sendQueue); });
    std::thread receivingThread([&]() { receive(server); });

    Logging::logI(file, "Threads started");

    while (!exitLoop) {
        MotorState desiredState = {
            .driveMotorState = driveProcessor->getDriveMotorState(),
            .armMotorState = armProcessor->getJointSpaceState(),
        };

        Message message(desiredState);
        sendQueue->push(message);

        usleep(0.1 * 1000 * 1000); // Sleep 0.1s
    }
    controllerThread.join();
    sendingThread.join();
    receivingThread.join();
}

//
void Base::sendZeroMessage(int setVal) {
    Message message(DriveZeroMessage {});
    sendQueue->push(message);

    Logging::logV(file, "zeroMessage queued. Set");
}

void Base::sendHeadlightsMessage(int brightnessVal) {
    Message message(HeadlightMessage { .brightness = brightnessVal });
    sendQueue->push(message);
    Logging::logV(file, "headlightsMessage queued. Brightness = %d",
                  brightnessVal);
}

void Base::sendCameraServoMessage(int control) {
    Message message(CameraServoMessage {
        .control = static_cast<CameraServoMessageControl>(control) });
    sendQueue->push(message);
    Logging::logV(file, "cameraServoMessage queued. Control = %d", control);
}

void Base::sendSciToolMessage(MessageFormat format, int value) {
    switch (format) {
    case MESSAGE_FORMAT_SCI_TOOL_DOOR: {
        Message message(SciToolDoorMessage {
            .door = static_cast<SciToolDoorControl>(value) });
        sendQueue->push(message);
        Logging::logV(file, "sciToolDoorMessage queued. Door = %d", value);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_BRUSH: {
        Message message(SciToolBrushMessage {
            .control = static_cast<SciToolBrushControl>(value) });
        sendQueue->push(message);
        Logging::logV(file, "sciToolBrushMessage queued. Control = %d", value);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_HEIGHT: {
        Message message(SciToolHeightMessage {
            .control = static_cast<SciToolHeightControl>(value) });
        sendQueue->push(message);
        Logging::logV(file, "sciToolHeightMessage queued. Control = %d", value);
        break;
    }
    default:
        Logging::logW(file, "Ignoring unsupported SciTool message format: %d",
                      static_cast<int>(format));
        break;
    }
}