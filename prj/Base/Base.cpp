// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include <initializer_list>
#include <mutex>

#include "Base/Base.h"

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

Base::Base() {

    PI = 3.1415926;

    // Variables for state of chassis (Rover body)
    chassis_angle = 0;
    chassis_speed = 0;
    chassis_angular_velocity = 0;
    chassis_max_speed = 0;

    // Variables for state of rover arm
    arm_x = 0;
    arm_y = 0;
    arm_z = 0;

    // Variables for state of rover claw
    // x = left / right direction, y = up / down direction
    claw_open = 0;
    claw_x = 0;
    claw_y = 0;
    claw_angle = 0;

    // Variables for state of rover wrist
    wrist_angle = 0;

    // Variables for state of rover science tool
    // x = left / right direction, y = up / down direction
    tool_x = 0;
    tool_y = 0;
}

Base::~Base() { }

// setter / getter / increment functions for chassis_angle
int Base::getChassisAngle() { return chassis_angle; }

void Base::setChassisAngle(int n) {
    mtx.lock();
    std::cout << "chassis_angle: " << chassis_angle << " -> ";
    chassis_angle = n;
    std::cout << chassis_angle << "\n";
    mtx.unlock();
}

void Base::incrementChassisAngle(int n) {
    mtx.lock();
    std::cout << "chassis_angle: " << chassis_angle << " -> ";
    chassis_angle += n;
    std::cout << chassis_angle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassis_speed
int Base::getChassisSpeed() { return chassis_speed; }

void Base::setChassisSpeed(int n) {
    mtx.lock();
    std::cout << "chassis_speed: " << chassis_speed << " -> ";
    chassis_speed = n;
    std::cout << chassis_speed << "\n";
    mtx.unlock();
}

void Base::incrementChassisSpeed(int n) {
    mtx.lock();
    std::cout << "chassis_speed: " << chassis_speed << " -> ";
    chassis_speed += n;
    std::cout << chassis_speed << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassis_angular_velocity
int Base::getChassisAngularVelocity() { return chassis_angular_velocity; }

void Base::setChassisAngularVelocity(int n) {
    mtx.lock();
    std::cout << "chassis_angular_velocity: " << chassis_angular_velocity
              << " -> ";
    chassis_angular_velocity = n;
    std::cout << chassis_angular_velocity << "\n";
    mtx.unlock();
}

void Base::incrementChassisAngularVelocity(int n) {
    mtx.lock();
    std::cout << "chassis_angular_velocity: " << chassis_angular_velocity
              << " -> ";
    chassis_angular_velocity += n;
    std::cout << chassis_angular_velocity << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for chassis_max_speed
int Base::getChassisMaxSpeed() { return chassis_max_speed; }

void Base::setChassisMaxSpeed(int n) {
    mtx.lock();
    std::cout << "chassis_max_speed: " << chassis_max_speed << " -> ";
    chassis_max_speed = n;
    std::cout << chassis_max_speed << "\n";
    mtx.unlock();
}

void Base::incrementChassisMaxSpeed(int n) {
    mtx.lock();
    std::cout << "chassis_max_speed: " << chassis_max_speed << " -> ";
    chassis_max_speed += n;
    std::cout << chassis_max_speed << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for arm_x
int Base::getArmX() { return arm_x; }

void Base::setArmX(int n) {
    mtx.lock();
    std::cout << "arm_x: " << arm_x << " -> ";
    arm_x = n;
    std::cout << arm_x << "\n";
    mtx.unlock();
}

void Base::incrementArmX(int n) {
    mtx.lock();
    std::cout << "arm_x: " << arm_x << " -> ";
    arm_x += n;
    std::cout << arm_x << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for arm_y
int Base::getArmY() { return arm_y; }

void Base::setArmY(int n) {
    mtx.lock();
    std::cout << "arm_y: " << arm_y << " -> ";
    arm_y = n;
    std::cout << arm_y << "\n";
    mtx.unlock();
}

void Base::incrementArmY(int n) {
    mtx.lock();
    std::cout << "arm_y: " << arm_y << " -> ";
    arm_y += n;
    std::cout << arm_y << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for arm_z
int Base::getArmZ() { return arm_z; }

void Base::setArmZ(int n) {
    mtx.lock();
    std::cout << "arm_z: " << arm_z << " -> ";
    arm_z = n;
    std::cout << arm_z << "\n";
    mtx.unlock();
}

void Base::incrementArmZ(int n) {
    mtx.lock();
    std::cout << "arm_z: " << arm_z << " -> ";
    arm_z += n;
    std::cout << arm_z << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for claw_open
int Base::getClawOpen() { return claw_open; }

void Base::setClawOpen(int n) {
    mtx.lock();
    std::cout << "claw_open: " << claw_open << " -> ";
    claw_open = n;
    std::cout << claw_open << "\n";
    mtx.unlock();
}

void Base::incrementClawOpen(int n) {
    mtx.lock();
    std::cout << "claw_open: " << claw_open << " -> ";
    claw_open += n;
    std::cout << claw_open << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for claw_x
int Base::getClawX() { return claw_x; }

void Base::setClawX(int n) {
    mtx.lock();
    std::cout << "claw_x: " << claw_x << " -> ";
    claw_x = n;
    std::cout << claw_x << "\n";
    mtx.unlock();
}

void Base::incrementClawX(int n) {
    mtx.lock();
    std::cout << "claw_x: " << claw_x << " -> ";
    claw_x += n;
    std::cout << claw_x << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for claw_y
int Base::getClawY() { return claw_y; }

void Base::setClawY(int n) {
    mtx.lock();
    std::cout << "claw_y: " << claw_y << " -> ";
    claw_y = n;
    std::cout << claw_y << "\n";
    mtx.unlock();
}

void Base::incrementClawY(int n) {
    mtx.lock();
    std::cout << "claw_y: " << claw_y << " -> ";
    claw_y += n;
    std::cout << claw_y << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for claw_angle
int Base::getClawAngle() { return claw_angle; }

void Base::setClawAngle(int n) {
    mtx.lock();
    std::cout << "claw_angle: " << claw_angle << " -> ";
    claw_angle = n;
    std::cout << claw_angle << "\n";
    mtx.unlock();
}

void Base::incrementClawAngle(int n) {
    mtx.lock();
    std::cout << "claw_angle: " << claw_angle << " -> ";
    claw_angle += n;
    std::cout << claw_angle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for wrist_angle
int Base::getWristAngle() { return wrist_angle; }

void Base::setWristAngle(int n) {
    mtx.lock();
    std::cout << "wrist_angle: " << wrist_angle << " -> ";
    wrist_angle = n;
    std::cout << wrist_angle << "\n";
    mtx.unlock();
}

void Base::incrementWristAngle(int n) {
    mtx.lock();
    std::cout << "wrist_angle: " << wrist_angle << " -> ";
    wrist_angle += n;
    std::cout << wrist_angle << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for tool_x
int Base::getToolX() { return tool_x; }

void Base::setToolX(int n) {
    mtx.lock();
    std::cout << "tool_x: " << tool_x << " -> ";
    tool_x = n;
    std::cout << tool_x << "\n";
    mtx.unlock();
}

void Base::incrementToolX(int n) {
    mtx.lock();
    std::cout << "tool_x: " << tool_x << " -> ";
    tool_x += n;
    std::cout << tool_x << "\n";
    mtx.unlock();
}

// setter / getter / increment functions for tool_y
int Base::getToolY() { return tool_y; }

void Base::setToolY(int n) {
    mtx.lock();
    std::cout << "tool_y: " << tool_y << " -> ";
    tool_y = n;
    std::cout << tool_y << "\n";
    mtx.unlock();
}

void Base::incrementToolY(int n) {
    mtx.lock();
    std::cout << "tool_y: " << tool_y << " -> ";
    tool_y += n;
    std::cout << tool_y << "\n";
    mtx.unlock();
}

// Converts int (0-255) to radian (0-2pi)
float Base::intToRadian(int n) { return n / 255.0 * 2 * PI; }

// Sets up controllers
void Base::setButtons(buttonFunctions buttons_controller_1,
                      buttonFunctions buttons_controller_2) {
    ControllerHolder* controller
        = new ControllerHolder(buttons_controller_1, buttons_controller_2);
    this->controller = controller;
}

void Base::start() {
    thread th1([&]() { controller->eventLoop(); });
    std::cout << "Main Thread\n";
    th1.join();
}
