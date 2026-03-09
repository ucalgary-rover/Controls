#include "Rover.h"
#include "Message.h"
#include "UDPHandler.h"
#include "pub_general.h"

#include <chrono>

static const char* file = "Rover";

using namespace std;

//---------------------- Start/Stop Functions ----------------------//
// Note: Only the start() and stop() functions have their own thread //

void Rover::start() {
    char address[16];

    UDPHandler client(ROVER_PORT, BASE_PORT);
    MessageQueue sendQueue;

    // Instantiate the systems for the rover
#if EXTENTION == EXTENTION_TYPE_ARM
    const std::vector<MotorType> motorTypes
        = { MOTOR_TYPE_STEPPER_MOTOR, MOTOR_TYPE_BLDC_MOTOR,
            MOTOR_TYPE_BLDC_MOTOR, MOTOR_TYPE_STEPPER_MOTOR,
            MOTOR_TYPE_STEPPER_MOTOR };
    Arm arm(motorTypes);
#endif

    Logging::logI(file, "Instantiating Drive system");
    Drive drive(ROVER_WIDTH, ROVER_LENGTH);

    // Create queue for rover
    MessageQueue roverQueue;

    // Create desired state manager
    desiredStateManager = MotorStateManager(defaultState);

    desiredDriveMotorStateManager = desiredStateManager.getDriveStateManager();
    ArmMotorStateManager* desiredArmMotorStateManager
        = desiredStateManager.getArmStateManager();

    // Create current state manager
    currentStateManager = MotorStateManager(defaultState);

    DriveMotorStateManager* currentDriveMotorStateManager
        = currentStateManager.getDriveStateManager();
    ArmMotorStateManager* currentArmMotorStateManager
        = currentStateManager.getArmStateManager(); // TODO: not yet used

    // instantiate handlers
    DriveHandler driveHandler(&drive, desiredDriveMotorStateManager,
                              currentDriveMotorStateManager);
#if EXTENTION == EXTENTION_TYPE_ARM
    ArmHandler armHandler(&arm, &desiredArmMotorStateManager);
#endif
    // Start the client thread
    thread clientThread([&]() { startClient(&roverQueue); });

    // start thread for handlers
    thread driveHandlerThread([&]() { driveHandler.start(); });
#if EXTENTION == EXTENTION_TYPE_ARM
    thread armHandlerThread([&]() { armHandler.start(); });
#endif
    thread sendingThread([&]() { client.run(sendQueue); });

    thread currentStateHandlerThread([&]() {
        while (true) {
            driveHandler.updateCurrentState();

            Message message = Message(currentStateManager.getState());
            sendQueue.push(message);

            usleep(0.1 * 1000000); // Sleep
        }
    });

    while (true) {
        Message reply = client.receive();
        MessagePayload payload = reply.getPayload();

        std::visit(
            [this, &payload](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, MotorState>) {
                    MotorStateManager newStateManager = MotorStateManager(arg);
                    desiredDriveMotorStateManager->updateState(
                        newStateManager.getState().driveMotorState);
                } else {
                    // Handles other message payloads
                    std::cerr << "Non-motor control message received from base!"
                              << std::endl;
                }
            },
            payload);

        roverQueue.push(reply);
    }

    driveHandlerThread.join();
    clientThread.join();
    sendingThread.join();
    currentStateHandlerThread.join();
#if EXTENTION_TYPE_ARM
    armHandlerThread.join();
#endif
}

//---------------------- Instantiation Functions ----------------------//

// Thread instantiation
void Rover::startClient(MessageQueue* clientQueue) {

    Message message;
    auto last_reception = std::chrono::system_clock::now();

    while (true) {
        // Get message from roverQueue

        if (!clientQueue->empty()) {
            last_reception = std::chrono::system_clock::now();
        } else {
            std::chrono::seconds time_since_reception
                = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now() - last_reception);

            Logging::logE(file,
                          "Connection to base timed out, halting motors.");
            //  if (time_since_reception.count() > NO_MESSAGE_RECIEVED_TIMEOUT)
            //  {
            //      armQueue->push(Message(0, ArmMessage()));
            //      driveQueue->push(Message(0, WheelMessage()));
            //  }
        }

        message = clientQueue->pop();

        if (message.getFormat()
            == MESSAGE_FORMAT_MOTOR_STATE) { // Discard invalid messages
            desiredStateManager.updateState(
                std::get<MotorState>(message.getPayload()));
        }
    }
}