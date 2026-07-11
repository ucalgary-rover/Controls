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

    controllers.push_back(std::make_shared<DriveControllerLayout>(driveProcessor));
#if EXTENSION == EXTENSION_TYPE_ARM
    controllers.push_back(std::make_shared<ArmControllerLayout>(armProcessor));
#elif EXTENSION == EXTENSION_TYPE_SCI_TOOL
    controllers.push_back(std::make_shared<SciToolControllerLayout>());
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
