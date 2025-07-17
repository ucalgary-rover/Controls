#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>

#include "Base/Controller.h"
#include "Message.h"
#include "MessageQueue.h"
#include "mission_control.h"
#include "pub_general.h"
#include "Websocket/WebsocketServer.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base();

    // General getter, setter, and incrementor for int member variables
    int getFloat(const float& member);
    void setFloat(float& member, int n, int min, int max, const char* name);
    void incrementFloat(float& member, int n, int min, int max, const char* name);
    void changeArmControlType(ArmMessageType type);
    void incrementJoint(int change);

    // Converter functions
    float intToRadian(int n);

    // Start the loops to have Base working
    void start();

    // Exit the loops that base runs through
    void quit();

private:
    // Constant variables for calculations
    float PI;

    // Variables for state of chassis (Rover body)
    float chassisAngle, chassisSpeed, chassisAngularVelocity, chassisMaxSpeed;

    // Variables for state of rover arm
    ArmMessageType armControlType;

    float manualAngleChange;
    MotorID joint;

    float wristX, wristY, wristZ;
    float clawOpen;
    float clawPitch, clawRoll;

    int exitLoop;

    // List of all controllers in use
    ControllerHolder* controller;

    buttonFunctions* drive_control;
    buttonFunctions* arm_manulal_control; 
    buttonFunctions* arm_fixed_ik_control;
    buttonFunctions* arm_variable_ik_control;
};

#endif