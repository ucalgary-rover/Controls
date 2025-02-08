// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#include <initializer_list>
#include <mutex>

#include "Base.h"
#include "Controller.h"

using namespace std;

// Create object for mutex (thread locks)
mutex mtx;

// setter / getter / increment functions for chassis_angle
int Base::getChassisAngle() { return chassis_angle; }

void Base::setChassisAngle(int n) {
  mtx.lock();
  chassis_angle = n;
  mtx.unlock();
}

void Base::incrementChassisAngle(int n) {
  mtx.lock();
  chassis_angle += n;
  mtx.unlock();
}

// setter / getter / increment functions for chassis_speed
int Base::getChassisSpeed() { return chassis_speed; }

void Base::setChassisSpeed(int n) {
  mtx.lock();
  chassis_speed = n;
  mtx.unlock();
}

void Base::incrementChassisSpeed(int n) {
  mtx.lock();
  chassis_speed += n;
  mtx.unlock();
}

// setter / getter / increment functions for chassis_angular_velocity
int Base::getChassisAngularVelocity() { return chassis_angular_velocity; }

void Base::setChassisAngularVelocity(int n) {
  mtx.lock();
  chassis_angular_velocity = n;
  mtx.unlock();
}

void Base::incrementChassisAngularVelocity(int n) {
  mtx.lock();
  chassis_angular_velocity += n;
  mtx.unlock();
}

// setter / getter / increment functions for chassis_max_speed
int Base::getChassisMaxSpeed() { return chassis_max_speed; }

void Base::setChassisMaxSpeed(int n) {
  mtx.lock();
  chassis_max_speed = n;
  mtx.unlock();
}

void Base::incrementChassisMaxSpeed(int n) {
  mtx.lock();
  chassis_max_speed += n;
  mtx.unlock();
}

// setter / getter / increment functions for arm_x
int Base::getArmX() { return arm_x; }

void Base::setArmX(int n) {
  mtx.lock();
  arm_x = n;
  mtx.unlock();
}

void Base::incrementArmX(int n) {
  mtx.lock();
  arm_x += n;
  mtx.unlock();
}

// setter / getter / increment functions for arm_y
int Base::getArmY() { return arm_y; }

void Base::setArmY(int n) {
  mtx.lock();
  arm_y = n;
  mtx.unlock();
}

void Base::incrementArmY(int n) {
  mtx.lock();
  arm_y += n;
  mtx.unlock();
}

// setter / getter / increment functions for arm_z
int Base::getArmZ() { return arm_z; }

void Base::setArmZ(int n) {
  mtx.lock();
  arm_z = n;
  mtx.unlock();
}

void Base::incrementArmZ(int n) {
  mtx.lock();
  arm_z += n;
  mtx.unlock();
}

// setter / getter / increment functions for claw_open
int Base::getClawOpen() { return claw_open; }

void Base::setClawOpen(int n) {
  mtx.lock();
  claw_open = n;
  mtx.unlock();
}

void Base::incrementClawOpen(int n) {
  mtx.lock();
  claw_open += n;
  mtx.unlock();
}

// setter / getter / increment functions for claw_x
int Base::getClawX() { return claw_x; }

void Base::setClawX(int n) {
  mtx.lock();
  claw_x = n;
  mtx.unlock();
}

void Base::incrementClawX(int n) {
  mtx.lock();
  claw_x += n;
  mtx.unlock();
}

// setter / getter / increment functions for claw_y
int Base::getClawY() { return claw_y; }

void Base::setClawY(int n) {
  mtx.lock();
  claw_y = n;
  mtx.unlock();
}

void Base::incrementClawY(int n) {
  mtx.lock();
  claw_y += n;
  mtx.unlock();
}

// setter / getter / increment functions for claw_angle
int Base::getClawAngle() { return claw_angle; }

void Base::setClawAngle(int n) {
  mtx.lock();
  claw_angle = n;
  mtx.unlock();
}

void Base::incrementClawAngle(int n) {
  mtx.lock();
  claw_angle += n;
  mtx.unlock();
}

// setter / getter / increment functions for wrist_angle
int Base::getWristAngle() { return wrist_angle; }

void Base::setWristAngle(int n) {
  mtx.lock();
  wrist_angle = n;
  mtx.unlock();
}

void Base::incrementWristAngle(int n) {
  mtx.lock();
  wrist_angle += n;
  mtx.unlock();
}

// setter / getter / increment functions for tool_x
int Base::getToolX() { return tool_x; }

void Base::setToolX(int n) {
  mtx.lock();
  tool_x = n;
  mtx.unlock();
}

void Base::incrementToolX(int n) {
  mtx.lock();
  tool_x += n;
  mtx.unlock();
}

// setter / getter / increment functions for tool_y
int Base::getToolY() { return tool_y; }

void Base::setToolY(int n) {
  mtx.lock();
  tool_y = n;
  mtx.unlock();
}

void Base::incrementToolY(int n) {
  mtx.lock();
  tool_y += n;
  mtx.unlock();
}

// Converts int (0-255) to radian (0-2pi)
float Base::intToRadian(int n) { return n / 255.0 * 2 * PI; }

// Sets up controllers
void Base::setControllers(std::initializer_list<Controller> args) {
  mtx.lock();

  controller_list.clear();

  int i = 1;
  for (auto const& controller : args) {
    controller_list.insert(controller_list.end(), controller);
    cout << "Controller " << i << " Initalized\n";
    i++;
  }

  mtx.unlock();

  buttonFunctions buttons_controller_1
    = { &incrementChassisSpeed,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
      }
}