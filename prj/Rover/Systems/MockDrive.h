#pragma once

#include "Rover/Systems/Drive.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "mission_control.h"
#include "phidget22.h"
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <vector>

class MockDrive : public Drive {
public:
    /**
     * @brief Completes the initialisation for all four DC and stepper motors
     * needed to drive the rover its self.
     * @param width The distance between the motors on the left side of the
     * rover to the motors on the right side of the rover.
     * @param length The distance between the motors on the top side of the
     * rover to the motors on the bottom side of the rover.
     */
    MockDrive();

    /**
     * @brief Completes the deinitialisation for all four DC and stepper motors
     * that were initialised in the constructor.
     */
    ~MockDrive();

    /**
     * @brief Turns the stepper motor of a wheel to a specified angle
     * @param wheel the handler of a particular stepper motor
     * @param angle the angle to turn the stepper
     * @return None
     */
    void setWheelAngle(DriveIndex wheel, float angle) override;
    double getWheelAngle(DriveIndex wheelIndex) override;

    void setWheelSpeed(DriveIndex wheel, float speed) override;
    double getWheelSpeed(DriveIndex wheelIndex) override;

    /**
     * @brief Gets the width of the rover between motors
     * @return The width of the rover from the motor on the left to the motor on
     * the right.
     */
    float getWidth();

    /**
     * @brief Gets the length of the rover between motors
     * @return The length of the rover from the motor on the left to the motor
     * on the right.
     */
    float getLength();

private:
    float steer[WHEEL_COUNT] = {};
    float drive[WHEEL_COUNT] = {};
};