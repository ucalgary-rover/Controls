// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include "Base/Base.h"

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

Base::Base() {

    PI = 3.1415926;

    // Variables for state of chassis (Rover body)
    chassisAngle = 0;
    chassisSpeed = 0;
    chassisAngularVelocity = 0;
    chassisMaxSpeed = 0;

    // Variables for state of rover arm
    armX = 0;
    armY = 0;
    armZ = 0;

    // Variables for state of rover claw
    // x = left / right direction, y = up / down direction
    clawOpen = 0;
    clawX = 0;
    clawY = 0;
    clawAngle = 0;

    // Variables for state of rover wrist
    wristAngle = 0;

    // Variables for state of rover science tool
    // x = left / right direction, y = up / down direction
    toolX = 0;
    toolY = 0;

    exitLoop = 0;
}

Base::~Base() { }

// setter / getter / increment functions for chassisAngle
int Base::getChassisAngle() { return chassisAngle; }

void Base::setChassisAngle(int n) {
    mtx.lock();
    std::cout << "chassisAngle: " << chassisAngle << " -> ";
    chassisAngle = n;
    std::cout << chassisAngle << "\n";
    mtx.unlock();
}

void Base::incrementChassisAngle(int n) {
    mtx.lock();
    std::cout << "chassisAngle: " << chassisAngle << " -> ";
    chassisAngle += n;
    std::cout << chassisAngle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassisSpeed
int Base::getChassisSpeed() { return chassisSpeed; }

void Base::setChassisSpeed(int n) {
    mtx.lock();
    std::cout << "chassisSpeed: " << chassisSpeed << " -> ";
    chassisSpeed = n;
    std::cout << chassisSpeed << "\n";
    mtx.unlock();
}

void Base::incrementChassisSpeed(int n) {
    mtx.lock();
    std::cout << "chassisSpeed: " << chassisSpeed << " -> ";
    chassisSpeed += n;
    std::cout << chassisSpeed << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassisAngularVelocity
int Base::getChassisAngularVelocity() { return chassisAngularVelocity; }

void Base::setChassisAngularVelocity(int n) {
    mtx.lock();
    std::cout << "chassisAngularVelocity: " << chassisAngularVelocity << " -> ";
    chassisAngularVelocity = n;
    std::cout << chassisAngularVelocity << "\n";
    mtx.unlock();
}

void Base::incrementChassisAngularVelocity(int n) {
    mtx.lock();
    std::cout << "chassisAngularVelocity: " << chassisAngularVelocity << " -> ";
    chassisAngularVelocity += n;
    std::cout << chassisAngularVelocity << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassisMaxSpeed
int Base::getChassisMaxSpeed() { return chassisMaxSpeed; }

void Base::setChassisMaxSpeed(int n) {
    mtx.lock();
    std::cout << "chassisMaxSpeed: " << chassisMaxSpeed << " -> ";
    chassisMaxSpeed = n;
    std::cout << chassisMaxSpeed << "\n";
    mtx.unlock();
}

void Base::incrementChassisMaxSpeed(int n) {
    mtx.lock();
    std::cout << "chassisMaxSpeed: " << chassisMaxSpeed << " -> ";
    chassisMaxSpeed += n;
    std::cout << chassisMaxSpeed << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for armX
int Base::getArmX() { return armX; }

void Base::setArmX(int n) {
    mtx.lock();
    std::cout << "armX: " << armX << " -> ";
    armX = n;
    std::cout << armX << "\n";
    mtx.unlock();
}

void Base::incrementArmX(int n) {
    mtx.lock();
    std::cout << "armX: " << armX << " -> ";
    armX += n;
    std::cout << armX << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for armY
int Base::getArmY() { return armY; }

void Base::setArmY(int n) {
    mtx.lock();
    std::cout << "armY: " << armY << " -> ";
    armY = n;
    std::cout << armY << "\n";
    mtx.unlock();
}

void Base::incrementArmY(int n) {
    mtx.lock();
    std::cout << "armY: " << armY << " -> ";
    armY += n;
    std::cout << armY << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for armZ
int Base::getArmZ() { return armZ; }

void Base::setArmZ(int n) {
    mtx.lock();
    std::cout << "armZ: " << armZ << " -> ";
    armZ = n;
    std::cout << armZ << "\n";
    mtx.unlock();
}

void Base::incrementArmZ(int n) {
    mtx.lock();
    std::cout << "armZ: " << armZ << " -> ";
    armZ += n;
    std::cout << armZ << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for clawOpen
int Base::getClawOpen() { return clawOpen; }

void Base::setClawOpen(int n) {
    mtx.lock();
    std::cout << "clawOpen: " << clawOpen << " -> ";
    clawOpen = n;
    std::cout << clawOpen << "\n";
    mtx.unlock();
}

void Base::incrementClawOpen(int n) {
    mtx.lock();
    std::cout << "clawOpen: " << clawOpen << " -> ";
    clawOpen += n;
    std::cout << clawOpen << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for clawX
int Base::getClawX() { return clawX; }

void Base::setClawX(int n) {
    mtx.lock();
    std::cout << "clawX: " << clawX << " -> ";
    clawX = n;
    std::cout << clawX << "\n";
    mtx.unlock();
}

void Base::incrementClawX(int n) {
    mtx.lock();
    std::cout << "clawX: " << clawX << " -> ";
    clawX += n;
    std::cout << clawX << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for clawY
int Base::getClawY() { return clawY; }

void Base::setClawY(int n) {
    mtx.lock();
    std::cout << "clawY: " << clawY << " -> ";
    clawY = n;
    std::cout << clawY << "\n";
    mtx.unlock();
}

void Base::incrementClawY(int n) {
    mtx.lock();
    std::cout << "clawY: " << clawY << " -> ";
    clawY += n;
    std::cout << clawY << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for clawAngle
int Base::getClawAngle() { return clawAngle; }

void Base::setClawAngle(int n) {
    mtx.lock();
    std::cout << "clawAngle: " << clawAngle << " -> ";
    clawAngle = n;
    std::cout << clawAngle << "\n";
    mtx.unlock();
}

void Base::incrementClawAngle(int n) {
    mtx.lock();
    std::cout << "clawAngle: " << clawAngle << " -> ";
    clawAngle += n;
    std::cout << clawAngle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for wristAngle
int Base::getWristAngle() { return wristAngle; }

void Base::setWristAngle(int n) {
    mtx.lock();
    std::cout << "wristAngle: " << wristAngle << " -> ";
    wristAngle = n;
    std::cout << wristAngle << "\n";
    mtx.unlock();
}

void Base::incrementWristAngle(int n) {
    mtx.lock();
    std::cout << "wristAngle: " << wristAngle << " -> ";
    wristAngle += n;
    std::cout << wristAngle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for toolX
int Base::getToolX() { return toolX; }

void Base::setToolX(int n) {
    mtx.lock();
    std::cout << "toolX: " << toolX << " -> ";
    toolX = n;
    std::cout << toolX << "\n";
    mtx.unlock();
}

void Base::incrementToolX(int n) {
    mtx.lock();
    std::cout << "toolX: " << toolX << " -> ";
    toolX += n;
    std::cout << toolX << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for toolY
int Base::getToolY() { return toolY; }

void Base::setToolY(int n) {
    mtx.lock();
    std::cout << "toolY: " << toolY << " -> ";
    toolY = n;
    std::cout << toolY << "\n";
    mtx.unlock();
}

void Base::incrementToolY(int n) {
    mtx.lock();
    std::cout << "toolY: " << toolY << " -> ";
    toolY += n;
    std::cout << toolY << "\n";
    mtx.unlock();
}

// Converts int (0-255) to radian (0-2pi)
float Base::intToRadian(int n) { return n / 255.0 * 2 * PI; }

// Sets up controllers
void Base::setButtons(buttonFunctions buttonsController1,
                      buttonFunctions buttonsController2) {
    ControllerHolder* controller
        = new ControllerHolder(buttonsController1, buttonsController2);
    this->controller = controller;
}

void Base::quit() { this->exitLoop = 1; }

void Base::start() {
    MessageQueue sendQueue;
    thread controllerThread([&]() { controller->eventLoop(); });
    // thread websocetThread([&]() { ; });
    std::cout << "Main Thread\n";

    WheelMessage wheelMsg;

#if Extention == EXTENTION_TYPE_ARM
    ArmMessage armMsg;
#endif

    while (!exitLoop) {
        // Update message for Drive
        wheelMsg.angle_velocity = this->getChassisAngularVelocity();
        wheelMsg.theta = this->getChassisAngle();
        wheelMsg.velocity = this->getChassisAngle();

        Message driveMessage(MESSAGE_PRIORITY_LOW, wheelMsg);

        // Add Drive Message to queue
        sendQueue.push(driveMessage);

#if Extention == EXTENTION_TYPE_ARM
        // Update messgae for Arm
        armMsg.armXPos = this->getArmX();
        armMsg.armYPos = this->getArmY();
        armMsg.armZPos = this->getArmZ();
        armMsg.clawOpen = this->getClawOpen();
        armMsg.clawRotation = this->getClawAngle();
        armMsg.clawXPos = this->getClawX();
        armMsg.clawYPos = this->getClawY();
        armMsg.wristRotation = this->getWristAngle();

        Message armMessage(MESSAGE_PRIORITY_LOW, armMsg);

        // Add Arm Message to queue
        sendQueue.push(armMessage);
#endif
        Sleep(500);
    }
    controllerThread.join();
    // websocetThread.join();
}
