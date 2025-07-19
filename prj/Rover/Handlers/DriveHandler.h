#ifndef DRIVE_HANDLER_H
#define DRIVE_HANDLER_H

#pragma once

#include "HandlerInterface.h"
#include "MessageQueue.h"
#include "Rover/Systems/Drive.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "phidget22.h"
#include <cmath>
#include <mutex>

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
    void setWheelTargetHit(int index, bool value) {
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
     * @brief Takes in an angle and derives speed value from it
     * @param stickAngle The input angle from the joystick
     * @return A speed value based on the angle
     */
    float spotTurnSpeed(int stickAngle);

    /**
     * @brief Directly moves the rover in a direction without turning the body.
     * Takes velocity and angle
     * @param stickTheta Int of angle of joystick by used for direction (theta)
     * @param stickMagnitude Int of the magnitude of the stick (velocity)
     * @return None
     */
    void strafe(int stickTheta, int stickMagnitude);

    /**
     * @brief Takes in an angle and derives an adujusted angle and direction
     * from it
     * @param stickTheta The input angle from the left joystick
     * @return An adjusted angle within wheel range of motion. Sign determines
     * direction of wheel spin
     */
    int strafeAngleAdjust(int stickTheta);

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
     * @brief Determines the heading angle of the radial turn and updates last
     * radial turn
     * @param stickAngle Int of angle of joystick by used for sharpness of turn
     * (in degrees)
     * @return The absolute value of the heading angle of the turn in radians
     * <----
     */
    float radialTurnHeadingAngle(int stickAngle);

    /**
     * @brief Determines the angle of a leading wheel (inner or outer)
     * @param headingAngle Float angle of desired turn (in radians)
     * @param innerFlag If true, calculates for the inner wheel, if false
     * calculates the outer
     * @return The spcified wheel angle in degrees <---
     */
    float radialTurnWheelAngles(int headingAngle, bool innerFlag);

    /**
     * @brief Stops all wheel movement
     * @return None
     */
    void stopWheels();

    /**
     * @brief Pops messages out of the queue and calls other functions
     * @return None
     */
    void start();

    /**
     * @brief Turns the stepper motor of a wheel to a specified angle
     * @param stepper the handle of a particular stepper motor
     * @param angle the angle to turn the stepper
     * @return None
     */
    void turnWheel(PhidgetStepperHandle stepper, int angle);

private:
    Drive* m_drive;

    // Drive Queue
    MessageQueue* m_driveQueue; // References the DriveQueue in Rover.cpp

    // Variables corresponding to conditions during operation

    // list containing the status of each wheel reaching its target position
    bool m_wheelTargetStatuses[4] = { false, false, false, false };

    // if true, the wheels are in spot turning configuration
    bool m_spotTurnFlag = false;

    // holds the sign of the direction of the last radial output
    int m_lastRadialOutput = 1;
};

#endif