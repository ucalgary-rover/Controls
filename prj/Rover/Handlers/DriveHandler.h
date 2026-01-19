#ifndef DRIVE_HANDLER_H
#define DRIVE_HANDLER_H

#pragma once

#include "DriveModel.h"
#include "HandlerInterface.h"
#include "MessageQueue.h"
#include "Rover/Systems/Drive.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "phidget22.h"
#include <cmath>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <mutex>
// #include <iostream>

class DriveHandler : public HandlerInterface {
public:
    DriveHandler(Drive* drive, MessageQueue* driveQueue);
    ~DriveHandler() { };

    /**
     * @brief Changes the target status of a wheel
     * @param index The index associated with the wheel
     * @param value A boolean value signifying if the wheel has hit it's target
     * @return None
     */
    void setWheelTargetStatus(int index, bool value) {
        m_wheelTargetStatuses[index] = value;
    }

    /**
     * @brief Enters spot turn wheel configuration, turns
     * @param stickAngle Int of angle of joystick to determine direction /
     * turning speed (angle velocity)
     * @return None
     */
    void spotTurn(int stickAngle);

    /**
     * @brief Directly moves the rover in a direction without turning the body.
     * Takes velocity and angle
     * @param stickTheta Int of angle of joystick by used for direction (theta)
     * @param stickMagnitude Int of the magnitude of the stick (velocity)
     * @return None
     */
    void strafe(int stickTheta, int stickMagnitude);

    /**
     * @brief Moves the rover with a radial turn
     * @param stickAngle Int of angle of joystick by used for direction and
     * sharpness of turn (angle velocity)
     * @param stickMagnitude Int of the magnitude of the left joystick
     * (velocity)
     * @param stickTheta Int of angle of the left stick used for direction of
     * wheel spin (forward or backward)
     * @return None
     */
    void radialTurn(int stickAngle, int stickMagnitude, int stickTheta);

    /**
     * @brief Stops all wheel movement
     * @return None
     */
    void stopWheels();

    /**
     * @brief Waits until all wheel targets are met
     * @return None
     */
    void awaitWheelTargets();

    /**
     * @brief Pops messages out of the queue and calls other functions
     * @return None
     */
    void start();

    /**
     * @brief Turns the stepper motor of a wheel to a specified angle
     * @param stepper the handler of a particular stepper motor
     * @param angle the angle to turn the stepper
     * @return None
     */
    void turnWheel(DriveIndex wheel, int angle);

    /**
     * @brief Calibrates a wheel to face forward
     * @note This function should be called before the rover is used to ensure
     * the wheels are facing the correct direction
     * @param wheel the index of the wheel to calibrate
     * @return None
     */
    void calibrateWheel(DriveIndex wheel);

private:
    Drive* m_drive;

    Json::Value m_lastKnownPos; // Last known position of the rover

    // Drive Queue
    MessageQueue* m_driveQueue; // References the DriveQueue in Rover.cpp

    // Variables corresponding to conditions during operation

    // list containing the status of each wheel reaching its target position
    bool m_wheelTargetStatuses[4] = { false, false, false, false };

    // if true, the wheels are in spot turning configuration
    bool m_spotTurnFlag = false;
};

#endif