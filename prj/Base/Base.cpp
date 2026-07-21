// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base.h"

#include <chrono>
#include <cmath>
#include <string>
#include <unistd.h>
#include <vector>

#include "ArmControllerLayout.h"
#include "Config/Config.h"
#include "DriveControllerLayout.h"
#include "MqttPublisher/MqttPublisher.h"
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

    // MQTT setup: load config and connect the (singleton) publisher.
    // Adjust the path to wherever the binary is launched from.
    Config config("prj/Config/config.json");
    std::cout << "MQTT serverUrl=[" << config.mqttConfig.serverUrl << "] clientId=[" << config.mqttConfig.clientId << "] topic=[" << config.mqttConfig.topic << "]" << std::endl;
    MqttPublisher mqttPublisher(config.mqttConfig);

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

        // Publish the same desired state to the dashboard over MQTT.
        // Caught locally so a transient publish failure never kills the loop.
        try {
            mqttPublisher.publish(config.mqttConfig.topic, desiredState);
        } catch (const std::exception& e) {
            Logging::logE(file, "MQTT publish failed: %s", e.what());
        }

        usleep(0.1 * 1000 * 1000); // Sleep 0.1s
    }

    MqttPublisher::shutdown();

    controllerThread.join();
    sendingThread.join();
    receivingThread.join();
}