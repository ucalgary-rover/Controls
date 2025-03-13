#ifndef ARM_H
#define ARM_H

#pragma once

#include "Configs.h"
#include "phidget22.h"
#include "pub_rover.h"
#include <iostream>

static class Arm {
public:
    /**
     * @brief Completes the initialisation for all motors That make up the arm
     * and the claw.
     * @param degOfFreedom Number of degrees of freedom in the arm.
     * @param motorTypes Pointer to an array of length degOfFreedom t oknow what
     * type of motor each DOF uses.
     */
    Arm(int degOfFreedom, MOTOR_TYPE* motorTypes);

    /**
     * @brief Completes the deinitialisation for all motors That make up the arm
     * as well as the claw servo that were initialised in the constructor.
     */
    ~Arm();

    /**
     * @brief Function to get the motor handler and motor type for a motor on
     * the arm at a scpecific index, reguardless of motor type.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @param index The index starting from the base motor on the arm to get the
     * handler for.
     * @return True is successful, false otherwise
     */
    bool getArmMotorHandle(MotorHandlerReturn* retVal, int index);

    /**
     * @brief Gets the servo handler for the servo initialised as the claw.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @return True is successful, false otherwise
     */
    bool getArmClawHandle(MotorHandlerReturn* retVal);

    /**
     * @brief Returns the degrees of freedom of the arm
     * @return Degrees of freedom of the arm.
     */
    int getDOF();

private:
    int m_degOfFreedom;
    MOTOR_TYPE* m_motorTypes;
    PhidgetDCMotorHandle** m_handlesDC;
    PhidgetStepperHandle** m_handlesStepper;
    PhidgetRCServoHandle* m_handleClaw;
};

#endif