#pragma once

#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "mission_control.h"

class Drive { // abstract
public:
    /**
     * @brief Turns the stepper motor of a wheel to a specified angle
     * @param wheel the handler of a particular stepper motor
     * @param angle the angle to turn the stepper
     * @return None
     */
    virtual void setWheelAngle(DriveIndex wheel, float angle) = 0;
    virtual double getWheelAngle(DriveIndex wheelIndex) = 0;

    virtual void setWheelSpeed(DriveIndex wheel, float speed) = 0;
    virtual double getWheelSpeed(DriveIndex wheelIndex) = 0;

    /**
     * @brief Gets the width of the rover between motors
     * @return The width of the rover from the motor on the left to the motor on
     * the right.
     */
    virtual float getWidth() = 0;

    /**
     * @brief Gets the length of the rover between motors
     * @return The length of the rover from the motor on the left to the motor
     * on the right.
     */
    virtual float getLength() = 0;
};