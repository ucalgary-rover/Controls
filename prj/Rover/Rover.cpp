#include "Rover.h"

#include <chrono>
#include <memory>
#include <thread>

#include "pub_rover.h"

#include "Rover/Systems/DriveHardware.h"
#include "Rover/Systems/MockDrive.h"

#include "Rover/Systems/ArmHardware.h"
#include "Rover/Systems/MockArm.h"

#define CURRENT_STATE_PUSH_INTERVAL_US 1000 * 1000 // 1s
#define INACTIVE_SLEEP_US 100 * 1000               // 100ms

static const char* file = "Rover";

#if EXTENTION == EXTENTION_TYPE_ARM
std::shared_ptr<ArmHandler> Rover::armHandler;
#elif EXTENTION == EXTENTION_TYPE_SCIENCE_TOOL
std::shared_ptr<SciToolHandler> Rover::sciToolHandler;
#endif
std::shared_ptr<DriveHandler> Rover::driveHandler;
std::shared_ptr<HeadlightHandler> Rover::headlightHandler;

//---------------------- Rover Initialization ----------------------//
void Rover::initialize() {
    Logging::logI(file, "Initializing Rover");

#if EXTENTION == EXTENTION_TYPE_ARM
    initializeArm();
#elif EXTENTION == EXTENTION_TYPE_SCIENCE_TOOL
    initializeSciTool();
#endif
    initializeDrive();

    initializeHeadlights();
}

#if EXTENTION == EXTENTION_TYPE_ARM
void Rover::initializeArm() {
    // Instantiate the systems for the rover
    Logging::logI(file, "Instantiating Arm System");
    const std::vector<MotorType> motorTypes
        = { MOTOR_TYPE_STEPPER_MOTOR, MOTOR_TYPE_BLDC_MOTOR,
            MOTOR_TYPE_BLDC_MOTOR, MOTOR_TYPE_STEPPER_MOTOR,
            MOTOR_TYPE_STEPPER_MOTOR };

#if ARM_HARDWARE_TEST
    auto arm = std::make_shared<MockArm>();
#else
    auto arm = std::make_shared<ArmHardware>(motorTypes);
#endif
    armHandler = std::make_shared<ArmHandler>(arm, armQueue);
    processes.push_back(std::thread([&]() { armHandler->start(); }));
}
#elif EXTENTION == EXTENTION_TYPE_SCIENCE_TOOL
void Rover::initializeSciTool() {
    Logging::logI(file, "Instantiating Science Tool System");
    sciToolHandler = std::make_shared<SciToolHandler>(sciToolQueue);
    processes.push_back(std::thread([&]() { sciToolHandler.start(); }));
}
#endif

void Rover::initializeDrive() {
    Logging::logI(file, "Instantiating Drive System");
#if DRIVE_HARDWARE_TEST
    auto drive = std::make_shared<MockDrive>();
#else
    auto drive = std::make_shared<DriveHardware>();
#endif
    driveHandler = std::make_shared<DriveHandler>(
        drive, driveQueue, currentMotorStateManager->getDriveStateManager());
    processes.push_back(std::thread([&]() { driveHandler->start(); }));
}

void Rover::initializeHeadlights() {
    Logging::logI(file, "Instantiating Headlight System");
    headlightHandler = std::make_shared<HeadlightHandler>(headlightQueue);
    processes.push_back(std::thread([&]() { headlightHandler->start(); }));
}

//---------------------- Start/Stop Functions ----------------------//
// Note: Only the start() and stop() functions have their own thread //
void Rover::start() {
    Logging::logI(file, "Starting Network Systems");
    UDPHandler client(ROVER_PORT, BASE_PORT);

    // Sending Thread
    processes.push_back(std::thread([&]() { client.run(sendQueue); }));

    // Receiving Thread
    processes.push_back(std::thread([&]() {
        while (true) {
            Message received = client.receive().getPayload();
            receiveQueue->push(received);
        }
    }));

    // Current State Publishing Thread
    processes.push_back(std::thread([&]() {
        while (true) {
            // Read and send current motor state
            Message message = Message(currentMotorStateManager->getState());
            sendQueue->push(message);

            usleep(CURRENT_STATE_PUSH_INTERVAL_US); // Sleep
        }
    }));

    // Main message processing loop
    Message message;
    auto last_reception = std::chrono::system_clock::now();
    bool roverHalted = false;

    while (true) {
        // Check activity timeout
        if (!receiveQueue->empty()) {
            last_reception = std::chrono::system_clock::now();
        } else {
            std::chrono::seconds time_since_reception
                = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now() - last_reception);

            if (time_since_reception.count() > NO_MESSAGE_RECIEVED_TIMEOUT) {
                Logging::logE(file,
                              "Connection to base timed out, halting motors.");

                armQueue->push({});
                driveQueue->push({});
            }

            usleep(INACTIVE_SLEEP_US);
            continue;
        }

        message = receiveQueue->pop();

        switch (message.getFormat()) {
        case MESSAGE_FORMAT_MOTOR_STATE: {
            MotorState motorState = std::get<MotorState>(message.getPayload());
            armQueue->push(motorState.armMotorState);
            driveQueue->push(motorState.driveMotorState);
            break;
        }

        case MESSAGE_FORMAT_DRIVE_ZERO: {
            DriveZeroMessage zeroMessage
                = std::get<DriveZeroMessage>(message.getPayload());
            if (zeroMessage.set) { // currently setting zero
                driveHandler->setWheelZeroState();
            } else { // currently getting zero
                driveHandler->stopWheels();
                driveHandler->currentlyGettingZeroState = true;
            }
            break;
        }
        default:
            break;
        }
    }

    for (auto& process : processes) {
        process.join();
    }
}
