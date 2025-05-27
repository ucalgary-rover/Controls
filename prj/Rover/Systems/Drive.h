#ifndef DRIVE_H
#define DRIVE_H

#pragma once

#include "Rover/pub_rover.h"
#include "Rover/systems/pub_systems.h"
#include "mission_control.h"
#include "phidget22.h"
#include <iostream>

class Drive {
public:
    /**
     * @brief Completes the initialisation for all four DC and stepper motors
     * needed to drive the rover its self.
     * @param width The distance between the motors on the left side of the
     * rover to the motors on the right side of the rover.
     * @param length The distance between the motors on the top side of the
     * rover to the motors on the bottom side of the rover.
     */
    Drive(float width, float length);

    /**
     * @brief Completes the deinitialisation for all four DC and stepper motors
     * that were initialised in the constructor.
     */
    ~Drive();

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
     * @param stickAngle Int of angle of joystick by used for direction (theta)
     * @param stickMagnitude Int of the magnitude of the stick (velocity)
     * @return None
     */
    void strafe(int stickAngle, int stickMagnitude);

    /**
     * @brief Moves the rover with a radial turn
     * @param stickAngle Int of angle of joystick by used for direction and
     * sharpness of turn (angle velocity)
     * @param stickMagnitude Int of the magnitude of the stick (velocity)
     * @return None
     */
    void radialTurn(int stickAngle, int stickMagnitude);

    /**
     * @brief Stops all wheel movement
     * @return None
     */
    void stopWheels();

    /**
     * @brief Stops all wheel spin
     * @return None
     */
    void eventLoop();

private:
    PhidgetDCMotorHandle** m_handlesDC;
    PhidgetStepperHandle** m_handlesStepper;

    // if true, the wheels are in spot turning configuration
    bool m_spotTurnFlag = false;

    // Length and width in meters for now, can change once we get dimentions
    // from Mech Rover
    float m_length;
    float m_width;
};

#endif