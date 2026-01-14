// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base/Base.h"
#include <cmath>
#include <unistd.h>

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
    driveLayout = DriveControllerLayout(driveStateManager);
    armManualLayout = ArmManualControllerLayout(&armManualChangeManager);
    armFixedIKLayout = ArmFixedIKControllerLayout(armStateManager);
    armVariableIKLayout = ArmVariableIKControllerLayout(armStateManager);

    // Variables for state of rover arm
    armControlType = ARM_MESSAGE_TYPE_MANUAL;

    exitLoop = 0;

    Logging::logI(file, "Initializing Button Functions");
    drive_control = new buttonFunctions();
    drive_control->LEFT_JOYSTICK
        = [this](int X, int Y) { driveLayout.setVelocity(X, Y); };
    drive_control->RIGHT_JOYSTICK
        = [this](int X, int Y) { driveLayout.setAngularVelocity(X, Y); };
    drive_control->LEFT_TRIGGER
        = [this](int xValue) { driveLayout.unusedTrigger(xValue); };
    drive_control->RIGHT_TRIGGER
        = [this](int xValue) { driveLayout.unusedTrigger(xValue); };
    drive_control->buttonArray = {
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_A);
        }, // SDL_CONTROLLER_BUTTON_A
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_B);
        }, // SDL_CONTROLLER_BUTTON_B
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_X);
        }, // SDL_CONTROLLER_BUTTON_X
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_Y);
        }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_BACK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_GUIDE);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_START);
        }, // SDL_CONTROLLER_BUTTON_START
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_LEFTSTICK);
        }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        // left shoulder increments down
        [this]() {
            driveLayout.incrementMaxSpeed(-2);
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        // right shoulder increments up
        [this]() {
            driveLayout.incrementMaxSpeed(2);
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_DPAD_UP);
        }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [this]() {
            driveLayout.checkState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_manual_control = new buttonFunctions();
    arm_manual_control->LEFT_JOYSTICK
        = [this](int X, int Y) { armManualLayout.unusedStick(X, Y); };
    arm_manual_control->RIGHT_JOYSTICK
        = [this](int X, int Y) { armManualLayout.unusedStick(X, Y); };
    arm_manual_control->LEFT_TRIGGER = [this](int xValue) {
        armManualLayout.triggerToincrementJointValue(xValue, -5);
    };
    arm_manual_control->RIGHT_TRIGGER = [this](int xValue) {
        armManualLayout.triggerToincrementJointValue(xValue, 5);
    };
    arm_manual_control->buttonArray = {
        [this]() { armManualLayout.unusedButton(); }, // SDL_CONTROLLER_BUTTON_A
        [this]() { armManualLayout.unusedButton(); }, // SDL_CONTROLLER_BUTTON_B
        [this]() { armManualLayout.unusedButton(); }, // SDL_CONTROLLER_BUTTON_X
        [this]() { armManualLayout.unusedButton(); }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        }, // SDL_CONTROLLER_BUTTON_START
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [this]() {
            armManualLayout.incrementJoint(-1);
        }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [this]() {
            armManualLayout.incrementJoint(1);
        }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [this]() {
            armManualLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_fixed_ik_control = new buttonFunctions();
    arm_fixed_ik_control->LEFT_JOYSTICK = [this](int xValue, int yValue) {
        armFixedIKLayout.handleLeftJoyStick(xValue, yValue);
    };
    arm_fixed_ik_control->RIGHT_JOYSTICK = [this](int xValue, int yValue) {
        armFixedIKLayout.handleRightJoyStick(xValue, yValue);
    };
    arm_fixed_ik_control->LEFT_TRIGGER
        = [this](int xValue) { armFixedIKLayout.handleLeftTrigger(xValue); };
    arm_fixed_ik_control->RIGHT_TRIGGER
        = [this](int xValue) { armFixedIKLayout.handleRightTrigger(xValue); };
    arm_fixed_ik_control->buttonArray = {
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_A
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_B
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_X
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        }, // SDL_CONTROLLER_BUTTON_START
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [this]() {
            armFixedIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_variable_ik_control = new buttonFunctions();
    arm_variable_ik_control->LEFT_JOYSTICK = [this](int xValue, int yValue) {
        armVariableIKLayout.handleLeftJoyStick(xValue, yValue);
    };
    arm_variable_ik_control->RIGHT_JOYSTICK = [this](int xValue, int yValue) {
        armVariableIKLayout.handleRightJoyStick(xValue, yValue);
    };
    arm_variable_ik_control->LEFT_TRIGGER
        = [this](int xValue) { armVariableIKLayout.handleLeftTrigger(xValue); };
    arm_variable_ik_control->RIGHT_TRIGGER = [this](int xValue) {
        armVariableIKLayout.handleRightTrigger(xValue);
    };
    arm_variable_ik_control->buttonArray = {
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_A
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_B
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_X
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        }, // SDL_CONTROLLER_BUTTON_START
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [this]() {
            armVariableIKLayout.unusedButton();
        }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [this]() {
            armVariableIKLayout.incrementPitch(5);
        }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [this]() {
            armVariableIKLayout.incrementPitch(-5);
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [this]() {
            armVariableIKLayout.incrementRoll(-5);
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [this]() {
            armVariableIKLayout.incrementRoll(5);
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    controller = new ControllerHandler(*drive_control, *arm_manual_control);

    Logging::logI(file, "Initializing Base done");
}

Base::~Base() { }

void Base::changeArmControlType(ArmMessageType type) {
    mtx.lock();
    Logging::logI(file, "Setting Arm Control Type to %d", type);
    armControlType = type;
    switch (armControlType) {
    case ARM_MESSAGE_TYPE_MANUAL:
        controller->setControllerButtonFuncs(1, *arm_manual_control);
        break;
    case ARM_MESSAGE_TYPE_FIXED_IK:
        controller->setControllerButtonFuncs(1, *arm_fixed_ik_control);
        break;
    case ARM_MESSAGE_TYPE_VARIABLE_IK:
        controller->setControllerButtonFuncs(1, *arm_variable_ik_control);
        break;
    }
    mtx.unlock();
}

void Base::quit() { this->exitLoop = 1; }

void Base::start() {
    MessageQueue sendQueue;
    WebSocketServer server(WEBSOCKET_PORT);

    thread controllerThread([&]() { controller->eventLoop(); });
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
