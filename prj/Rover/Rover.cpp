#include "Rover.h"

#include <chrono>
#include <thread>

#include "Message.h"
#include "MessageQueue.h"
#include "UDPHandler.h"
#include "pub_rover.h"

#include "Rover/Handlers/DriveHandler.h"
#include "Rover/Systems/Drive.h"
#include "Rover/Systems/DriveHardware.h"

#if EXTENTION == EXTENTION_TYPE_ARM
#include "Rover/Handlers/ArmHandler.h"
#include "Rover/Systems/Arm.h"
#endif

#define CURRENT_STATE_PUSH_INTERVAL_US 1000 * 1000 // 1s
#define INACTIVE_SLEEP_US 100 * 1000               // 100ms

static const char* file = "Rover";

MotorStateManager Rover::desiredMotorStateManager;
MotorStateManager Rover::currentMotorStateManager;

//---------------------- Start/Stop Functions ----------------------//
// Note: Only the start() and stop() functions have their own thread //
void Rover::initialize() {
    Logging::logI(file, "Initializing Rover");

    // Initialize Rover State Manager
    Logging::logI(file, "Initializing State Managers");
    desiredMotorStateManager = MotorStateManager();
    currentMotorStateManager = MotorStateManager();
}

void Rover::start() {
    // Instantiate the systems for the rover
#if EXTENTION == EXTENTION_TYPE_ARM
    Logging::logI(file, "Instantiating Arm System");
    const std::vector<MotorType> motorTypes
        = { MOTOR_TYPE_STEPPER_MOTOR, MOTOR_TYPE_BLDC_MOTOR,
            MOTOR_TYPE_BLDC_MOTOR, MOTOR_TYPE_STEPPER_MOTOR,
            MOTOR_TYPE_STEPPER_MOTOR };
    Arm arm(motorTypes);
    ArmHandler armHandler(&arm, &desiredArmMotorStateManager);
    std::thread armHandlerThread([&]() { armHandler.start(); });
#endif

    Logging::logI(file, "Instantiating Drive System");
    DriveHardware physicalDrive(ROVER_WIDTH, ROVER_LENGTH);
    DriveHandler driveHandler(&physicalDrive,
                              desiredMotorStateManager.getDriveStateManager(),
                              currentMotorStateManager.getDriveStateManager());
    std::thread driveHandlerThread([&]() { driveHandler.start(); });

    Logging::logI(file, "Instantiating Network Systems");
    UDPHandler client(ROVER_PORT, BASE_PORT);
    MessageQueue sendQueue;
    MessageQueue receiveQueue;
    std::thread sendingThread([&]() { client.run(sendQueue); });
    std::thread receivingThread([&]() {
        while (true) {
            Message received = client.receive().getPayload();
            receiveQueue.push(received);
        }
    });

    std::thread currentStatePublishThread([&]() {
        while (true) {
            // Read and send current motor state
            Message message = Message(currentMotorStateManager.getState());
            sendQueue.push(message);

            usleep(CURRENT_STATE_PUSH_INTERVAL_US); // Sleep
        }
    });

    // Main message processing loop
    Message message;
    auto last_reception = std::chrono::system_clock::now();
    bool roverHalted = false;

    while (true) {
        // Check activity timeout
        if (!receiveQueue.empty()) {
            last_reception = std::chrono::system_clock::now();
        } else {
            std::chrono::seconds time_since_reception
                = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now() - last_reception);

            if (time_since_reception.count() > NO_MESSAGE_RECIEVED_TIMEOUT) {
                Logging::logE(file,
                              "Connection to base timed out, halting motors.");

                desiredMotorStateManager.updateDriveMotorState(
                    {}); // Zero Motors
            }

            usleep(INACTIVE_SLEEP_US);
            continue;
        }

        message = receiveQueue.pop();

        switch (message.getFormat()) {
        case MESSAGE_FORMAT_MOTOR_STATE:
            desiredMotorStateManager.updateState(
                std::get<MotorState>(message.getPayload()));
            break;
        case MESSAGE_FORMAT_DRIVE_ZERO: {
            DriveZeroMessage zeroMessage
                = std::get<DriveZeroMessage>(message.getPayload());
            if (zeroMessage.set) { // currently setting zero
                driveHandler.setWheelZeroState();
            } else { // currently getting zero
                driveHandler.stopWheels();
                driveHandler.currentlyGettingZeroState = true;
            }
            break;
        }
        default:
            break;
        }
    }

#if EXTENTION_TYPE_ARM
    armHandlerThread.join();
#endif
    driveHandlerThread.join();
    receivingThread.join();
    sendingThread.join();
    currentStatePublishThread.join();
}
