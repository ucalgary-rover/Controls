// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include <chrono>
#include <cmath>
#include <unistd.h>
#include <vector>

#include "ArmControllerLayout.h"
#include "Base.h"
#include "Base/Base.h"
#include "Base/Models/ArmModel.h"
#include "Base/Models/DriveModel.h"
#include "DriveControllerLayout.h"
#include "UDPHandler.h"

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

static const char* file = "Base";

// Chassis state management
RoverStateManager Base::desiredStateManager;
MotorStateManager Base::currentMotorStateManager;

// Variables for state of rover arm
ArmMessageType Base::armControlType;

bool Base::exitLoop = false;

std::shared_ptr<DriveControllerLayout> Base::driveController;
std::shared_ptr<ArmControllerLayout> Base::armController;

void Base::initialize() {
    Logging::logI(file, "Initializing Base");

    // Initialize Rover State Manager
    Logging::logI(file, "Initializing State Managers");
    desiredStateManager = RoverStateManager();
    currentMotorStateManager = MotorStateManager();

    DriveStateManager* driveStateManager
        = desiredStateManager.getDriveStateManager();
    ArmStateManager* armStateManager = desiredStateManager.getArmStateManager();

    // Initialize controller layouts
    Logging::logI(file, "Initializing Controller Layouts");
    exitLoop = 0;

    driveController
        = std::make_shared<DriveControllerLayout>(processDesiredDriveState);
    armController
        = std::make_shared<ArmControllerLayout>(processDesiredArmState);

    ControllerHandler::initialize({ driveController, armController });

    Logging::logI(file, "Initializing Base done");
}

void Base::quit() { exitLoop = 1; }

ArmMotorState Base::processDesiredArmState(const ArmState& desiredArmState) {
    ArmMotorState armMs = {};

    // Wrist position IK
    std::array<double, 3> desiredPos
        = { (double)desiredArmState.x, (double)desiredArmState.y,
            (double)desiredArmState.z };
    std::array<int, 6> angles = ArmModel::generateWristPosition(desiredPos);

    // Apply claw orientation
    double pitch = (double)desiredArmState.pitch;
    double roll = (double)desiredArmState.roll;

    angles = ArmModel::generateClawOrientation(angles, pitch, roll);

    // Map angles to motor IDs
    armMs.motorValues[MOTOR_ID_BASE] = angles[0];
    armMs.motorValues[MOTOR_ID_SHOULDER] = angles[1];
    armMs.motorValues[MOTOR_ID_ELBOW] = angles[2];
    armMs.motorValues[MOTOR_ID_WRIST] = angles[3];

    // ArmState desired orientation
    armMs.motorValues[MOTOR_ID_CLAW_ROLL] = angles[4];
    armMs.motorValues[MOTOR_ID_CLAW_PITCH] = angles[5];

    // Open/close value
    armMs.motorValues[MOTOR_ID_CLAW_OPEN] = desiredArmState.clawOpen;

    return armMs;
}

DriveMotorState Base::processDesiredDriveState(const DriveState& state) {
    DriveMotorState currentMotorState
        = currentMotorStateManager.getState().driveMotorState;
    return DriveModel::process(state, currentMotorState);
}

MotorState Base::getDesiredRoverState(uint64_t elapsed_ms) {
    MotorState desiredMotorstate = {
        .driveMotorState = driveController->getDriveMotorState(elapsed_ms),
        .armMotorState = armController->getArmMotorState(elapsed_ms),
    };

    return desiredMotorstate;
}

void Base::receive(UDPHandler& receiver) {
    while (true) {
        Message reply = receiver.receive();
        reply.printMessage(); // Print the received message

        currentMotorStateManager.updateState(
            std::get<MotorState>(reply.getPayload()));
    }
}

void Base::start() {
    MessageQueue sendQueue;
    UDPHandler server(BASE_PORT, ROVER_PORT);

    thread controllerThread([&]() { ControllerHandler::eventLoop(); });
    thread sendingThread([&]() { server.run(sendQueue); });
    thread receivingThread([&]() { receive(server); });

    ArmModel::initialize();
    DriveModel::initialize();

    std::chrono::steady_clock::time_point previous
        = std::chrono::steady_clock::now();

    while (!exitLoop) {
        auto now = std::chrono::steady_clock::now();

        uint64_t elapsed_ms
            = std::chrono::duration_cast<std::chrono::milliseconds>(now
                                                                    - previous)
                  .count();

        previous = now;

        MotorState desiredMotorState = getDesiredRoverState(elapsed_ms);
        Message message(desiredMotorState);
        sendQueue.push(message);

        usleep(0.1 * 1000000);
    }
    controllerThread.join();
    sendingThread.join();
    receivingThread.join();
}
