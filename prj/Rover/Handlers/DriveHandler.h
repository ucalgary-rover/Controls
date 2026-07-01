#pragma once

#include "DriveMotorStateManager.h"
#include "HandlerInterface.h"
#include "Rover/Systems/Drive.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "phidget22.h"
#include <cmath>
#include <mutex>
// #include <iostream>

class DriveHandler : public HandlerInterface {
public:
    bool currentlyGettingZeroState = false;
    DriveHandler(Drive* drive,
                 DriveMotorStateManager* desiredDriveMotorStateManager,
                 DriveMotorStateManager* currentDriveMotorStateManager);
    ~DriveHandler() { };

    void updateCurrentState();

    void translateSpeedAndAngle(DriveMotorState desiredState,
                                DriveIndex wheelIndex);

    /**
     * @brief Stops all wheel movement
     * @return None
     */
    void stopWheels();

    void setWheelZeroState();

    /**
     * @brief Pops messages out of the queue and calls other functions
     * @return None
     */
    void start();

private:
    bool zeroAngleSet = false;
    Drive* m_drive;

    // Drive state manager
    DriveMotorStateManager*
        m_desiredDriveMotorStateManager; // References the (desired) drive state
                                         // Manager in Rover.cpp
    DriveMotorStateManager*
        m_currentDriveMotorStateManager; // References the (current) drive state
                                         // Manager in Rover.cpp

    DriveMotorState
        wheelZeroState; // Direct hardware values, not consistent with software DriveModel
    // Variables corresponding to conditions during operation

    /**
     * @brief Turns the stepper motor of a wheel to a specified angle
     * @param wheel the handler of a particular stepper motor
     * @param angle the angle to turn the stepper
     * @return None
     */
    void setWheelAngle(DriveIndex wheel, float angle);
    double getWheelAngle(DriveIndex wheelIndex);

    void setWheelSpeed(DriveIndex wheel, float speed);
    double getWheelSpeed(DriveIndex wheelIndex);
};
