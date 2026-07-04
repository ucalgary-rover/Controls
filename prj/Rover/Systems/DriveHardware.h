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

class DriveHardware : public Drive {
public:
    /**
     * @brief Completes the initialisation for all four DC and stepper motors
     * needed to drive the rover its self.
     */
    DriveHardware();

    /**
     * @brief Completes the deinitialisation for all four DC and stepper motors
     * that were initialised in the constructor.
     */
    ~DriveHardware();

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
    std::vector<PhidgetBLDCMotorHandle> m_handlesDC = {};
    std::vector<PhidgetStepperHandle> m_handlesStepper = {};
    std::vector<PhidgetEncoderHandle> m_handlesDriveEncoder = {};

    /**
     * @brief Edits retVal to contain the motor type (in this case DC) and the
     * phidgets handler for that motor.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @param index Valid value from DriveMotorIndex to specify what wheel you
     * want the DC handler for.
     * @return True is successful, false otherwise
     */
    bool getDriveDCHandle(MotorHandlerReturn* retVal, int index);

    /**
     * @brief Edits retVal to contain the motor type (in this case stepper) and
     * the phidgets handler for that motor.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @param index Valid value from DriveMotorIndex to specify what wheel you
     * want the stepper handler for.
     * @return True is successful, false otherwise.
     */
    bool getDriveStepperHandle(MotorHandlerReturn* retVal, int index);

    /**
     * @brief Edits retVal to contain the motor type (in this case encoder
     * and the phidgets handler for that motor.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @param index Valid value from DriveMotorIndex to specify what wheel you
     * want the encoder handler for.
     * @return True is successful, false otherwise.
     */
    bool getDriveEncoderHandle(MotorHandlerReturn* retVal, int index);
};