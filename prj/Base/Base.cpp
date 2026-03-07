// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base/Base.h"
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

    // Initialize Motor State Manager
    armManualChangeManager = ArmMotorStateManager();

    // Initialize controller layouts
    Logging::logI(file, "Initializing Controller Layouts");
    exitLoop = 0;
    ControllerHandler::initialize(driveStateManager, armStateManager,
                                  &armManualChangeManager);

    Logging::logI(file, "Initializing Base done");
}

Base::~Base() { }

void Base::quit() { this->exitLoop = 1; }

void Base::start() {
    MessageQueue sendQueue;
    WebSocketServer server(WEBSOCKET_PORT);

    thread controllerThread([&]() { ControllerHandler::eventLoop(); });
    thread websocketServerThread([&]() { server.run(sendQueue); });

    WheelMessage wheelMsg;

#if Extention == EXTENTION_TYPE_ARM
    ArmMessage armMsg;
#endif

    while (!exitLoop) {
        RoverState desiredState = desiredStateManager.getState();

        // Update message for Drive
        wheelMsg.angleVelocity = desiredState.driveState.angularVelocity;
        wheelMsg.theta = desiredState.driveState.heading;
        wheelMsg.velocity = desiredState.driveState.speed;

        Message driveMessage(MESSAGE_PRIORITY_LOW, wheelMsg);

        // Add Drive Message to queue
        Logging::logV(file, "Adding drive message to queue");

        sendQueue.push(driveMessage);

#if Extention == EXTENTION_TYPE_ARM
        // Update messgae for Arm
        switch (armControlType) {
        case ARM_MESSAGE_TYPE_MANUAL:
            // Manual Control
            ArmManualMessage manualMsg;
            manualMsg.motorId
                = MOTOR_ID_BASE; // TODO: Replace with correct logic once
                                 // MotorState refactor is complete
            manualMsg.angleChange = 0;
            armMsg.type = ARM_MESSAGE_TYPE_MANUAL;
            armMsg.manual_message = manualMsg;
            break;

        case ARM_MESSAGE_TYPE_FIXED_IK:
            // Fixed Inverse Kinematics Control
            ArmFixedIKMessage fixedIKMsg;
            fixedIKMsg.wristX = desiredState.armState.x;
            fixedIKMsg.wristY = desiredState.armState.y;
            fixedIKMsg.wristZ = desiredState.armState.z;
            fixedIKMsg.clawOpen = desiredState.armState.clawOpen;
            armMsg.type = ARM_MESSAGE_TYPE_FIXED_IK;
            armMsg.fixed_ik_message = fixedIKMsg;

            break;

        case ARM_MESSAGE_TYPE_VARIABLE_IK:
            // Variable Inverse Kinematics Control
            ArmVariableIKMessage variableIKMsg;
            variableIKMsg.wristX = desiredState.armState.x;
            variableIKMsg.wristY = desiredState.armState.y;
            variableIKMsg.wristZ = desiredState.armState.z;
            variableIKMsg.clawPitch = desiredState.armState.pitch;
            variableIKMsg.clawRoll = desiredState.armState.roll;
            variableIKMsg.clawOpen = desiredState.armState.clawOpen;
            armMsg.type = ARM_MESSAGE_TYPE_VARIABLE_IK;
            armMsg.variable_ik_message = variableIKMsg;

            break;
        }
        // Add Arm Message to queue
        Logging::logV(file, "Adding arm message to queue");

        sendQueue.push(Message(MESSAGE_PRIORITY_LOW, armMsg));

#endif
        usleep(0.1 * 1000000);
    }
    controllerThread.join();
    websocketServerThread.join();
}
