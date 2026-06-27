// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base.h"

#include <chrono>
#include <cmath>
#include <unistd.h>
#include <vector>

#include "ArmControllerLayout.h"
#include "DriveControllerLayout.h"
#include "Models/ArmModel.h"
#include "Models/DriveModel.h"
#include "UDPHandler.h"

static const char* file = "Base";

// Chassis state management
MotorStateManager Base::desiredMotorStateManager;
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
    desiredMotorStateManager = MotorStateManager();
    currentMotorStateManager = MotorStateManager();

    // Initialize controller layouts
    Logging::logI(file, "Initializing Controller Layouts");
    exitLoop = 0;

    driveController
        = std::make_shared<DriveControllerLayout>(processDesiredDriveState);
    armController = std::make_shared<ArmControllerLayout>(
        processDesiredArmState, armForwardsKinematics);

    ControllerHandler::initialize({ driveController, armController });

    Logging::logI(file, "Initializing Base done");
}

void Base::quit() { exitLoop = true; }

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

ArmState Base::armForwardsKinematics(const ArmMotorState& motorState) {

    std::array<int, 6> motorValues = {};
    for (int i = 0; i <= MOTOR_ID_CLAW_PITCH; i++) {
        motorValues[i] = motorState.motorValues[i];
    }

    ArmFKOutput fkOut = ArmModel::forwardsKinematics(motorValues);

    ArmState out = {
        .x = (float)fkOut.wrist_position[0],
        .y = (float)fkOut.wrist_position[1],
        .z = (float)fkOut.wrist_position[2],
        .pitch = (int)fkOut.claw_pitch,
        .roll = (int)fkOut.claw_roll,
    };

    return out;
}

DriveMotorState Base::processDesiredDriveState(const DriveState& state) {
    DriveMotorState currentMotorState
        = currentMotorStateManager.getState().driveMotorState;
    return DriveModel::process(state, currentMotorState);
}

void Base::updateDesiredRoverState(uint64_t elapsed_ms) {
    MotorState desiredMotorstate = {
        .driveMotorState
        = driveController->getControlState(elapsed_ms).driveMotorState,
        .armMotorState
        = armController->getControlState(elapsed_ms).armMotorState,
    };

    desiredMotorStateManager.updateState(desiredMotorstate);

    static uint64_t accumulated_ms = 0;
    accumulated_ms += elapsed_ms;

    if (accumulated_ms >= 1000) {
        accumulated_ms = 0;

        // Log desired Drive state
        Logging::logI(file, "=== Desired Drive State ===");
        Logging::logI(file,
                      "steer[FR=%.2f FL=%.2f BR=%.2f BL=%.2f] drive[FR=%.2f "
                      "FL=%.2f BR=%.2f BL=%.2f]",
                      desiredMotorstate.driveMotorState.steer[WHEEL_FR],
                      desiredMotorstate.driveMotorState.steer[WHEEL_FL],
                      desiredMotorstate.driveMotorState.steer[WHEEL_BR],
                      desiredMotorstate.driveMotorState.steer[WHEEL_BL],
                      desiredMotorstate.driveMotorState.drive[WHEEL_FR],
                      desiredMotorstate.driveMotorState.drive[WHEEL_FL],
                      desiredMotorstate.driveMotorState.drive[WHEEL_BR],
                      desiredMotorstate.driveMotorState.drive[WHEEL_BL]);

        // Log desdired Arm state
        Logging::logI(file, "=== Desired Arm State ===");
        Logging::logI(
            file,
            "motors[BASE=%d SHOULDER=%d ELBOW=%d WRIST=%d CLAW_ROLL=%d "
            "CLAW_PITCH=%d CLAW_OPEN=%d]",
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_BASE],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_SHOULDER],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_ELBOW],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_WRIST],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_CLAW_ROLL],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_CLAW_PITCH],
            desiredMotorstate.armMotorState.motorValues[MOTOR_ID_CLAW_OPEN]);
    }
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

    std::thread controllerThread([&]() { ControllerHandler::eventLoop(); });
    std::thread sendingThread([&]() { server.run(sendQueue); });
    std::thread receivingThread([&]() { receive(server); });

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

        updateDesiredRoverState(elapsed_ms);

        Message message(desiredMotorStateManager.getState());
        sendQueue.push(message);

        usleep(0.1 * 1000 * 1000); // Sleep 0.1s
    }
    controllerThread.join();
    sendingThread.join();
    receivingThread.join();
}
