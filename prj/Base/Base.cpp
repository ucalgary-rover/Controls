// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base/Base.h"
#include "Base.h"
#include "Base/Models/ArmModel.h"
#include "Base/Models/DriveModel.h"
#include "UDPHandler.h"
#include <cmath>
#include <unistd.h>
#include <vector>

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

static const char* file = "Base";

Base::Base() {
    Logging::logI(file, "Initializing Base");

    // Initialize Rover State Manager
    Logging::logI(file, "Initializing State Managers");
    desiredStateManager = RoverStateManager(defaultState);

    DriveStateManager* driveStateManager
        = desiredStateManager.getDriveStateManager();
    ArmStateManager* armStateManager = desiredStateManager.getArmStateManager();

    // Initialize Motor State Managers
    armManualChangeManager = ArmMotorStateManager();
    driveManualChangeManager = DriveMotorStateManager();

    // Initialize controller layouts
    Logging::logI(file, "Initializing Controller Layouts");
    exitLoop = 0;
    ControllerHandler::initialize(driveStateManager, &driveManualChangeManager,
                                  armStateManager, &armManualChangeManager);

    Logging::logI(file, "Initializing Base done");
}

void Base::quit() { this->exitLoop = 1; }

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

DriveMotorState Base::processDesiredDriveState(DriveState& state,
                                               DriveMotorState& currentState) {
    return DriveModel::process(state, currentState);
}

MotorState
Base::processDesiredRoverState(MotorStateManager& currentMotorStateManager) {
    constexpr bool armManual
        = true; // TODO: Add Controller logic for determining whether to use
                // armManualState

    RoverState desiredState = desiredStateManager.getState();
    MotorState currentMotorState = currentMotorStateManager.getState();

    MotorState desiredMotorstate = {
        .driveMotorState = processDesiredDriveState(
            desiredState.driveState, currentMotorState.driveMotorState),
        .armMotorState = armManual
                             ? armManualChangeManager.getState()
                             : processDesiredArmState(desiredState.armState),
    };

    return desiredMotorstate;
}

void Base::receive(UDPHandler& receiver,
                   MotorStateManager& currentMotorStateManager) {
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

    MotorStateManager currentMotorStateManager;

    thread controllerThread([&]() { ControllerHandler::eventLoop(); });
    thread sendingThread([&]() { server.run(sendQueue); });
    thread receivingThread(
        [&]() { receive(server, currentMotorStateManager); });

    ArmModel::initialize();
    DriveModel::initialize();

    while (!exitLoop) {
        MotorState desiredMotorState
            = processDesiredRoverState(currentMotorStateManager);

        Message message = Message(desiredMotorState);
        sendQueue.push(message);

        usleep(0.1 * 1000000);
    }

    controllerThread.join();
    sendingThread.join();
    receivingThread.join();
}
