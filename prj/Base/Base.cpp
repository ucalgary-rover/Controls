// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base/Base.h"

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

static void valLimmit(float* value, int min, int max);
static void unusedButton();
static void unusedStick(int X, int Y);
static void unusedTrigger(int X);
static const char* file = "Base";

Base::Base() {
    Logging::logI(file, "Initializing Base");

    PI = 3.1415926;

    // Variables for state of chassis (Rover body)
    chassisAngle = 0;
    chassisSpeed = 0;
    chassisAngularVelocity = 0;
    chassisMaxSpeed = 0;

    // Variables for state of rover arm
    armControlType = ARM_MESSAGE_TYPE_MANUAL;

    manualAngleChange = 0;
    joint = MOTOR_ID_BASE;

    wristX = 0;
    wristY = 0;
    wristZ = 0;

    clawOpen = 0;
    clawPitch = 0;
    clawRoll = 0;

    exitLoop = 0;

    drive_control = new buttonFunctions();
    drive_control->LEFT_JOYSTICK = &unusedStick;
    drive_control->RIGHT_JOYSTICK = &unusedStick;
    drive_control->LEFT_TRIGGER = &unusedTrigger;
    drive_control->RIGHT_TRIGGER = &unusedTrigger;
    drive_control->buttonArray = {
        [this]() {
            incrementFloat(chassisSpeed, 2, 0, chassisMaxSpeed, "chassisSpeed");
        }, // SDL_CONTROLLER_BUTTON_A
        [this]() {
            incrementFloat(chassisSpeed, -2, 0, chassisMaxSpeed,
                           "chassisSpeed");
        }, // SDL_CONTROLLER_BUTTON_B
        [this]() {
            incrementFloat(chassisMaxSpeed, 2, 0, 100, "chassisMaxSpeed");
        }, // SDL_CONTROLLER_BUTTON_X
        [this]() {
            incrementFloat(chassisMaxSpeed, -2, 0, 100, "chassisMaxSpeed");
        },                        // SDL_CONTROLLER_BUTTON_Y
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_BACK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() { quit(); },     // SDL_CONTROLLER_BUTTON_START
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_manulal_control = new buttonFunctions();
    arm_manulal_control->LEFT_JOYSTICK = &unusedStick;
    arm_manulal_control->RIGHT_JOYSTICK = &unusedStick;
    arm_manulal_control->LEFT_TRIGGER = &unusedTrigger;
    arm_manulal_control->RIGHT_TRIGGER = &unusedTrigger;
    arm_manulal_control->buttonArray = {
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_A
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_B
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_X
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        },                        // SDL_CONTROLLER_BUTTON_START
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_fixed_ik_control = new buttonFunctions();
    arm_fixed_ik_control->LEFT_JOYSTICK = &unusedStick;
    arm_fixed_ik_control->RIGHT_JOYSTICK = &unusedStick;
    arm_fixed_ik_control->LEFT_TRIGGER = &unusedTrigger;
    arm_fixed_ik_control->RIGHT_TRIGGER = &unusedTrigger;
    arm_fixed_ik_control->buttonArray = {
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_A
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_B
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_X
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        },                        // SDL_CONTROLLER_BUTTON_START
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_variable_ik_control = new buttonFunctions();
    arm_variable_ik_control->LEFT_JOYSTICK = &unusedStick;
    arm_variable_ik_control->RIGHT_JOYSTICK = &unusedStick;
    arm_variable_ik_control->LEFT_TRIGGER = &unusedTrigger;
    arm_variable_ik_control->RIGHT_TRIGGER = &unusedTrigger;
    arm_variable_ik_control->buttonArray = {
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_A
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_B
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_X
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_Y
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_FIXED_IK);
        }, // SDL_CONTROLLER_BUTTON_BACK
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_MANUAL);
        }, // SDL_CONTROLLER_BUTTON_GUIDE
        [this]() {
            changeArmControlType(ARM_MESSAGE_TYPE_VARIABLE_IK);
        },                        // SDL_CONTROLLER_BUTTON_START
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        []() { unusedButton(); }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    controller = new ControllerHolder(*drive_control, *arm_manulal_control);

    Logging::logI(file, "Initializing Base done");
}

Base::~Base() { }

// General getter for float member variables
int Base::getFloat(const float& member) { return member; }

// General setter for float member variables
void Base::setFloat(float& member, int n, int min, int max, const char* name) {
    mtx.lock();
    member = n;
    valLimmit(&member, min, max);
    Logging::logI(file, "Setting %s to %f", name, member);
    mtx.unlock();
}

// General increment for flaot member variables
void Base::incrementFloat(float& member, int n, int min, int max,
                          const char* name) {
    mtx.lock();
    member += n;
    valLimmit(&member, min, max);
    Logging::logI(file, "Incrementing %s to %f", name, member);
    mtx.unlock();
}

void Base::changeArmControlType(ArmMessageType type) {
    mtx.lock();
    Logging::logI(file, "Setting Arm Control Type to %d", type);
    armControlType = type;
    switch (armControlType) {
    case ARM_MESSAGE_TYPE_MANUAL:
        controller->setControllerButtonFuncs(1, *arm_manulal_control);
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

void Base::incrementJoint(int change) {
    mtx.lock();
    MotorID next = static_cast<MotorID>((joint + change) % MOTOR_ID_END);
    Logging::logI(file, "Changing to joint: %d", next);
    joint = next;
    manualAngleChange = 0; // Reset manual angle change when changing joint
    mtx.unlock();
}

// Converts int (0-255) to radian (0-2pi)
float Base::intToRadian(int n) { return n / 255.0 * 2 * PI; }

void Base::quit() { this->exitLoop = 1; }

void Base::start() {
    MessageQueue sendQueue;
    WebSocketServer server(WEBSOCKET_PORT);

    thread controllerThread([&]() { controller->eventLoop(); });
    thread websocetServerThread([&]() { server.run(sendQueue); });

    WheelMessage wheelMsg;

#if Extention == EXTENTION_TYPE_ARM
    ArmMessage armMsg;
#endif

    while (!exitLoop) {
        // Update message for Drive
        wheelMsg.angleVelocity = getFloat(chassisAngularVelocity);
        wheelMsg.theta = getFloat(chassisAngle);
        wheelMsg.velocity = getFloat(chassisAngle);

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
            manualMsg.motorId = joint;
            manualMsg.angleChange = getFloat(manualAngleChange);
            armMsg.type = ARM_MESSAGE_TYPE_MANUAL;
            armMsg.manual_message = manualMsg;
            manualAngleChange = 0; // Reset manual angle change after sending

            break;

        case ARM_MESSAGE_TYPE_FIXED_IK:
            // Fixed Inverse Kinematics Control
            ArmFixedIKMessage fixedIKMsg;
            fixedIKMsg.wristX = getFloat(wristX);
            fixedIKMsg.wristY = getFloat(wristY);
            fixedIKMsg.wristZ = getFloat(wristZ);
            fixedIKMsg.clawOpen = getFloat(clawOpen);
            armMsg.type = ARM_MESSAGE_TYPE_FIXED_IK;
            armMsg.fixed_ik_message = fixedIKMsg;

            break;

        case ARM_MESSAGE_TYPE_VARIABLE_IK:
            // Variable Inverse Kinematics Control
            ArmVariableIKMessage variableIKMsg;
            variableIKMsg.wristX = getFloat(wristX);
            variableIKMsg.wristY = getFloat(wristY);
            variableIKMsg.wristZ = getFloat(wristZ);
            variableIKMsg.clawPitch = getFloat(clawPitch);
            variableIKMsg.clawRoll = getFloat(clawRoll);
            variableIKMsg.clawOpen = getFloat(clawOpen);
            armMsg.type = ARM_MESSAGE_TYPE_VARIABLE_IK;
            armMsg.variable_ik_message = variableIKMsg;

            break;
        }
        // Add Arm Message to queue
        Logging::logV(file, "Adding arm message to queue");

        sendQueue.push(Message(MESSAGE_PRIORITY_LOW, armMsg));

#endif
        usleep(1 * 1000000);
    }
    controllerThread.join();
    websocetServerThread.join();
}

static void valLimmit(float* value, int min, int max) {
    if (*value < min) {
        *value = min;
    } else if (*value > max) {
        *value = max;
    }
}

static void unusedButton() { Logging::logV(file, "Button Unused\n"); }

static void unusedStick(int X, int Y) {
    Logging::logV(file, "Unused Stick X: %d, Y: %d", X, Y);
}

static void unusedTrigger(int X) {
    Logging::logV(file, "Unused Trigger X: %d", X);
}