#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <windows.h>

#include "Base/Controller.h"
#include "Message.h"
#include "MessageQueue.h"
#include "pub_general.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base();

    // setter / getter / increment functions for chassisAngle
    int getChassisAngle();
    void setChassisAngle(int n);
    void incrementChassisAngle(int n);

    // setter / getter / increment functions for chassisSpeed
    int getChassisSpeed();
    void setChassisSpeed(int n);
    void incrementChassisSpeed(int n);

    // setter / getter / increment functions for chassisAngularVelocity
    int getChassisAngularVelocity();
    void setChassisAngularVelocity(int n);
    void incrementChassisAngularVelocity(int n);

    // setter / getter / increment functions for chassisMaxSpeed
    int getChassisMaxSpeed();
    void setChassisMaxSpeed(int n);
    void incrementChassisMaxSpeed(int n);

    // setter / getter / increment functions for armX
    int getArmX();
    void setArmX(int n);
    void incrementArmX(int n);

    // setter / getter / increment functions for armY
    int getArmY();
    void setArmY(int n);
    void incrementArmY(int n);

    // setter / getter / increment functions for armZ
    int getArmZ();
    void setArmZ(int n);
    void incrementArmZ(int n);

    // setter / getter / increment functions for clawOpen
    int getClawOpen();
    void setClawOpen(int n);
    void incrementClawOpen(int n);

    // setter / getter / increment functions for clawX
    int getClawX();
    void setClawX(int n);
    void incrementClawX(int n);

    // setter / getter / increment functions for clawY
    int getClawY();
    void setClawY(int n);
    void incrementClawY(int n);

    // setter / getter / increment functions for clawAngle
    int getClawAngle();
    void setClawAngle(int n);
    void incrementClawAngle(int n);

    // setter / getter / increment functions for wristAngle
    int getWristAngle();
    void setWristAngle(int n);
    void incrementWristAngle(int n);

    // Converter functions
    float intToRadian(int n);

    // Function which sets up controllers
    void setButtons(buttonFunctions buttons_controller_1,
                    buttonFunctions buttons_controller_2);

    // Start the loops to have Base working
    void start();

    // Exit the loops that base runs through
    void quit();

private:
    // Constant variables for calculations
    float PI;

    // Variables for state of chassis (Rover body)
    int chassisAngle, chassisSpeed, chassisAngularVelocity, chassisMaxSpeed;

    // Variables for state of rover arm
    int armX, armY, armZ;

    // Variables for state of rover claw
    // x = left / right direction, y = up / down direction
    int clawOpen, clawX, clawY, clawAngle;

    // Variables for state of rover wrist
    int wristAngle;

    int exitLoop;

    // List of all controllers in use
    ControllerHolder* controller;
};

#endif