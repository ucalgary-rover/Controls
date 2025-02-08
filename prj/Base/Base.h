// Schulich Space Program Controls Software Division (2025)
// Written by Gavin Grubert

#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <initializer_list>
#include <vector>

#include "Controller.h"

using namespace std;

#pragma once

class Base
{
private:
  // Constant variables for calculations
  float PI = 3.1415926;

  // Variables for state of chassis (Rover body)
  int chassis_angle = 0;
  int chassis_speed = 0;
  int chassis_angular_velocity = 0;
  int chassis_max_speed = 0;

  // Variables for state of rover arm
  int arm_x = 0;
  int arm_y = 0;
  int arm_z = 0;

  // Variables for state of rover claw
  // x = left / right direction, y = up / down direction
  int claw_open = 0;
  int claw_x = 0;
  int claw_y = 0;
  int claw_angle = 0;

  // Variables for state of rover wrist
  int wrist_angle = 0;

  // Variables for state of rover science tool
  // x = left / right direction, y = up / down direction
  int tool_x = 0;
  int tool_y = 0;

  // List of all controllers in use
  vector<Controller> controller_list;

public:
  // setter / getter / increment functions for chassis_angle
  int getChassisAngle();
  void setChassisAngle(int n);
  void incrementChassisAngle(int n);

  // setter / getter / increment functions for chassis_speed
  int getChassisSpeed();
  void setChassisSpeed(int n);
  void incrementChassisSpeed(int n);

  // setter / getter / increment functions for chassis_angular_velocity
  int getChassisAngularVelocity();
  void setChassisAngularVelocity(int n);
  void incrementChassisAngularVelocity(int n);

  // setter / getter / increment functions for chassis_max_speed
  int getChassisMaxSpeed();
  void setChassisMaxSpeed(int n);
  void incrementChassisMaxSpeed(int n);

  // setter / getter / increment functions for arm_x
  int getArmX();
  void setArmX(int n);
  void incrementArmX(int n);

  // setter / getter / increment functions for arm_y
  int getArmY();
  void setArmY(int n);
  void incrementArmY(int n);

  // setter / getter / increment functions for arm_z
  int getArmZ();
  void setArmZ(int n);
  void incrementArmZ(int n);

  // setter / getter / increment functions for claw_open
  int getClawOpen();
  void setClawOpen(int n);
  void incrementClawOpen(int n);

  // setter / getter / increment functions for claw_x
  int getClawX();
  void setClawX(int n);
  void incrementClawX(int n);

  // setter / getter / increment functions for claw_y
  int getClawY();
  void setClawY(int n);
  void incrementClawY(int n);

  // setter / getter / increment functions for claw_angle
  int getClawAngle();
  void setClawAngle(int n);
  void incrementClawAngle(int n);

  // setter / getter / increment functions for wrist_angle
  int getWristAngle();
  void setWristAngle(int n);
  void incrementWristAngle(int n);

  // setter / getter / increment functions for tool_x
  int getToolX();
  void setToolX(int n);
  void incrementToolX(int n);

  // setter / getter / increment functions for tool_y
  int getToolY();
  void setToolY(int n);
  void incrementToolY(int n);

  // Converter functions
  float intToRadian(int n);

  // Function which sets up controllers
  void setControllers(std::initializer_list<Controller> args);
};

#endif