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

    lastrightTriggerValue = 0;
    lastleftTriggerValue = 0;

    drive_control = new buttonFunctions();
    drive_control->LEFT_JOYSTICK = [](int X, int Y) { unusedStick(X, Y); };
    drive_control->RIGHT_JOYSTICK = [](int X, int Y) { unusedStick(X, Y); };
    drive_control->LEFT_TRIGGER = [](int xValue) { unusedTrigger(xValue); };
    drive_control->RIGHT_TRIGGER = [](int xValue) { unusedTrigger(xValue); };
    drive_control->buttonArray = {
        [this]() {
            incrementInt(&chassisSpeed, 2, 0, chassisMaxSpeed, "chassisSpeed");
        }, // SDL_CONTROLLER_BUTTON_A
        [this]() {
            incrementInt(&chassisSpeed, -2, 0, chassisMaxSpeed, "chassisSpeed");
        }, // SDL_CONTROLLER_BUTTON_B
        [this]() {
            incrementInt(&chassisMaxSpeed, 2, 0, 100, "chassisMaxSpeed");
        }, // SDL_CONTROLLER_BUTTON_X
        [this]() {
            incrementInt(&chassisMaxSpeed, -2, 0, 100, "chassisMaxSpeed");
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
    arm_manulal_control->LEFT_JOYSTICK
        = [](int X, int Y) { unusedStick(X, Y); };
    arm_manulal_control->RIGHT_JOYSTICK
        = [](int X, int Y) { unusedStick(X, Y); };
    arm_manulal_control->LEFT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastleftTriggerValue, &manualAngleChange,
                           -5, -20, 20, "manualAngleChange");
    };
    arm_manulal_control->RIGHT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastrightTriggerValue, &manualAngleChange,
                           5, -20, 20, "manualAngleChange");
    };
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
        },                                // SDL_CONTROLLER_BUTTON_START
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_LEFTSTICK
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [this]() { incrementJoint(-1); }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [this]() { incrementJoint(1); },  // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_DPAD_UP
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        []() { unusedButton(); },         // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    arm_fixed_ik_control = new buttonFunctions();
    arm_fixed_ik_control->LEFT_JOYSTICK = [this](int xValue, int yValue) {
        stickChangAxise(xValue, yValue, &wristX, &wristY, 0.0001, 0.0001, 1, 1,
                        "wristX", "wristY");
    };
    arm_fixed_ik_control->RIGHT_JOYSTICK = [this](int xValue, int yValue) {
        stickChangAxise(xValue, yValue, &wristZ, nullptr, 0.0001, 0, 1, 0,
                        "wristZ", "");
    };
    arm_fixed_ik_control->LEFT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastleftTriggerValue, &clawOpen, -5, 0, 100,
                           "clawOpen");
    };
    arm_fixed_ik_control->RIGHT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastrightTriggerValue, &clawOpen, 5, 0, 100,
                           "clawOpen");
    };
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
    arm_variable_ik_control->LEFT_JOYSTICK = [this](int xValue, int yValue) {
        stickChangAxise(xValue, yValue, &wristX, &wristY, 0.0001, 0.0001, 1, 1,
                        "wristX", "wristY");
    };
    arm_variable_ik_control->RIGHT_JOYSTICK = [this](int xValue, int yValue) {
        stickChangAxise(xValue, yValue, &wristZ, nullptr, 0.0001, 0, 1, 0,
                        "wristZ", "");
    };
    arm_variable_ik_control->LEFT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastleftTriggerValue, &clawOpen, -5, 0, 100,
                           "clawOpen");
    };
    arm_variable_ik_control->RIGHT_TRIGGER = [this](int xValue) {
        triggerToIncrement(xValue, &lastrightTriggerValue, &clawOpen, 5, 0, 100,
                           "clawOpen");
    };
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
        [this]() {
            incrementInt(&clawPitch, 5, -90, 90, "clawPitch");
        }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [this]() {
            incrementInt(&clawPitch, -5, -90, 90, "clawPitch");
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [this]() {
            incrementInt(&clawRoll, -5, -180, 180, "clawRoll");
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [this]() {
            incrementInt(&clawRoll, 5, -180, 180, "clawRoll");
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    controller = new ControllerHolder(*drive_control, *arm_manulal_control);

    Logging::logI(file, "Initializing Base done");
}

Base::~Base() { }

void Base::setInt(int* var, int n, int min, int max, const char* name) {
    return setVal<int>(var, n, min, max, name);
}

void Base::setFloat(float* var, float n, float min, float max,
                    const char* name) {
    return setVal<float>(var, n, min, max, name);
}

void Base::incrementInt(int* var, int n, int min, int max, const char* name) {
    return incrementVal<int>(var, n, min, max, name);
}

void Base::incrementFloat(float* var, float n, float min, float max,
                          const char* name) {
    return incrementVal<float>(var, n, min, max, name);
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
    joint
        = static_cast<MotorID>((joint + change + MOTOR_ID_END) % MOTOR_ID_END);
    Logging::logI(file, "Changing to joint: %d", joint);
    manualAngleChange = 0; // Reset manual angle change when changing joint
    mtx.unlock();
}

void Base::triggerToIncrement(int triggerValue, int* compare, int* var, int n,
                              int min, int max, const char* name) {
    if (triggerValue > 0 && *compare < 0) {
        incrementInt(var, n, min, max, name);
    }
    *compare
        = triggerValue; // Update the compare value to the current trigger value
}

void Base::stickChangAxise(int axisX, int axisY, float* varX, float* varY,
                           float maxChangeX, float maxChangeY, float rangeX,
                           float rangeY, const char* nameX, const char* nameY) {
    float changeX = axisX * (maxChangeX / 255.0);
    float changeY = axisY * (maxChangeY / -255.0);

    if (varX != nullptr) {
        Logging::logI(file, "change %f = %d * (%f / -255.0)", changeX, axisX,
                      maxChangeX);
        incrementFloat(varX, changeX, -rangeX, rangeX, nameX);
    }
    if (varY != nullptr) {
        Logging::logI(file, "change %f = %d * (%f / -255.0)", changeY, axisY,
                      maxChangeY);
        incrementFloat(varY, changeY, -rangeY, rangeY, nameY);
    }
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
        wheelMsg.angleVelocity = chassisAngularVelocity;
        wheelMsg.theta = chassisAngle;
        wheelMsg.velocity = chassisAngle;

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
            manualMsg.angleChange = manualAngleChange;
            armMsg.type = ARM_MESSAGE_TYPE_MANUAL;
            armMsg.manual_message = manualMsg;
            manualAngleChange = 0; // Reset manual angle change after sending

            break;

        case ARM_MESSAGE_TYPE_FIXED_IK:
            // Fixed Inverse Kinematics Control
            ArmFixedIKMessage fixedIKMsg;
            fixedIKMsg.wristX = wristX;
            fixedIKMsg.wristY = wristY;
            fixedIKMsg.wristZ = wristZ;
            fixedIKMsg.clawOpen = clawOpen;
            armMsg.type = ARM_MESSAGE_TYPE_FIXED_IK;
            armMsg.fixed_ik_message = fixedIKMsg;

            break;

        case ARM_MESSAGE_TYPE_VARIABLE_IK:
            // Variable Inverse Kinematics Control
            ArmVariableIKMessage variableIKMsg;
            variableIKMsg.wristX = wristX;
            variableIKMsg.wristY = wristY;
            variableIKMsg.wristZ = wristZ;
            variableIKMsg.clawPitch = clawPitch;
            variableIKMsg.clawRoll = clawRoll;
            variableIKMsg.clawOpen = clawOpen;
            armMsg.type = ARM_MESSAGE_TYPE_VARIABLE_IK;
            armMsg.variable_ik_message = variableIKMsg;

            break;
        }
        // Add Arm Message to queue
        Logging::logV(file, "Adding arm message to queue");

        sendQueue.push(Message(MESSAGE_PRIORITY_LOW, armMsg));

#endif
        usleep(3 * 1000000);
    }
    controllerThread.join();
    websocetServerThread.join();
}

// General setter for float member variables
template <typename T>
void Base::setVal(T* val, T n, T min, T max, const char* name) {
    mtx.lock();
    *val = n;
    if (*val < min) {
        *val = min;
    } else if (*val > max) {
        *val = max;
    }
    if (std::is_same<T, int>::value) {
        Logging::logI(file, "Setting %s to %d", name, *val);
    } else if (std::is_same<T, float>::value) {
        Logging::logI(file, "Setting %s to %f", name, *val);
    }
    Logging::logI(file, "Setting %s to %f", name, *val);
    mtx.unlock();
}

// General increment for flaot val variables
template <typename T>
void Base::incrementVal(T* val, T n, T min, T max, const char* name) {
    mtx.lock();
    *val += n;
    if (*val < min) {
        *val = min;
    } else if (*val > max) {
        *val = max;
    }
    if (std::is_same<T, int>::value) {
        Logging::logI(file, "Setting %s to %d", name, *val);
    } else if (std::is_same<T, float>::value) {
        Logging::logI(file, "Setting %s to %f", name, *val);
    }
    mtx.unlock();
}

static void unusedButton() { Logging::logV(file, "Button Unused\n"); }

static void unusedStick(int X, int Y) {
    Logging::logV(file, "Unused Stick X: %d, Y: %d", X, Y);
}

static void unusedTrigger(int X) {
    Logging::logV(file, "Unused Trigger X: %d", X);
}