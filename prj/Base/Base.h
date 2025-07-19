#ifndef BASE_H
#define BASE_H

#include <initializer_list>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

#include "Base/Controller.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Websocket/WebsocketServer.h"
#include "mission_control.h"
#include "pub_general.h"

using namespace std;

#pragma once

class Base {
public:
    Base();
    ~Base();

    // General getter, setter, and incrementor for int member variables

    /**
     * Sets an int variable to a value within specified limits
     *
     * @param var The variable to set
     * @param n The value to set the variable to
     * @param min The minimum limit for the variable
     * @param max The maximum limit for the variable
     * @param name The name of the variable (for logging)
     */
    void setInt(int* var, int n, int min, int max, const char* name);

    /**
     * Increments an int variable by a value within specified limits
     * @param var The variable to increment
     * @param n The value to increment the variable by
     * @param min The minimum limit for the variable
     * @param max The maximum limit for the variable
     * @param name The name of the variable (for logging)
     */
    void incrementInt(int* var, int n, int min, int max, const char* name);

    /**
     * Sets a float variable to a value within specified limits
     *
     * @param var The variable to set
     * @param n The value to set the variable to
     * @param min The minimum limit for the variable
     * @param max The maximum limit for the variable
     * @param name The name of the variable (for logging)
     */
    void setFloat(float* var, float n, float min, float max, const char* name);

    /**
     * Increments a float variable by a value within specified limits
     *
     * @param var The variable to increment
     * @param n The value to increment the variable by
     * @param min The minimum limit for the variable
     * @param max The maximum limit for the variable
     * @param name The name of the variable (for logging)
     */
    void incrementFloat(float* var, float n, float min, float max,
                        const char* name);

    void changeArmControlType(ArmMessageType type);
    void incrementJoint(int change);
    void triggerToIncrement(int triggerValue, int* compare, int* var, int n,
                            int min, int max, const char* name);

    void stickChangeAxis(int axisX, int axisY, float* varX, float* varY,
                         float maxChangeX, float maxChangeY, float rangeX,
                         float rangeY, const char* nameX, const char* nameY);

    /**
     * Calculates the magnitude of the stick's position from the center
     *
     * @param axisX The distance along the x axis
     * @param axisY The distance along the y axis
     * @param max The maximum limit for the output
     * @return The magnitude of the stick's position as a percentage of 255 (the
     * full radius)
     */
    int Base::stickMagnitdude(int axisX, int axisY, int speedMax)

        /**
         * Calculates the angle of the stick's position counterclockwise from
         * the positive y axis in DEGREES (which is ironically reported with a
         * negative value by SDL)
         *
         * @param axisX The distance along the x axis
         * @param axisY The distance along the y axis
         */
        int Base::stickAngle(int axisX, int axisY)

        // Converter functions
        float degreeToRadian(int n);

    int Base::radianToDegree(float n);

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
    ArmMessageType armControlType;

    int manualAngleChange;
    MotorID joint;

    float wristX, wristY, wristZ;
    int clawOpen;
    int clawPitch, clawRoll;

    int exitLoop;

    int lastleftTriggerValue;
    int lastrightTriggerValue;

    // Template functions for setting and incrementing member variables
    /**
     * Sets a member variable to a value within specified limits
     *
     * @param member The member variable to set
     * @param n The value to set the member variable to
     * @param min The minimum limit for the member variable
     * @param max The maximum limit for the member variable
     * @param name The name of the member variable (for logging)
     */
    template <typename T>
    void setVal(T* member, T n, T min, T max, const char* name);

    /**
     * Increments a member variable by a value within specified limits
     *
     * @param member The member variable to increment
     * @param n The value to increment the member variable by
     * @param min The minimum limit for the member variable
     * @param max The maximum limit for the member variable
     * @param name The name of the member variable (for logging)
     */
    template <typename T>
    void incrementVal(T* member, T n, T min, T max, const char* name);

    // List of all controllers in use
    ControllerHolder* controller;

    buttonFunctions* drive_control;
    buttonFunctions* arm_manulal_control;
    buttonFunctions* arm_fixed_ik_control;
    buttonFunctions* arm_variable_ik_control;
};

#endif