#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <thread>
#include <vector>

#include "Base/Controller.h"
#include "pub_general.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base();

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
    void setButtons(buttonFunctions buttons_controller_1,
                    buttonFunctions buttons_controller_2);

    void start();

private:
    // Constant variables for calculations
    float PI;

    // Variables for state of chassis (Rover body)
    int chassis_angle, chassis_speed, chassis_angular_velocity,
        chassis_max_speed;

    // Variables for state of rover arm
    int arm_x, arm_y, arm_z;

    // Variables for state of rover claw
    // x = left / right direction, y = up / down direction
    int claw_open, claw_x, claw_y, claw_angle;

    // Variables for state of rover wrist
    int wrist_angle;

    // Variables for state of rover science tool
    // x = left / right direction, y = up / down direction
    int tool_x, tool_y;

    // List of all controllers in use
    ControllerHolder* controller;
};

#endif