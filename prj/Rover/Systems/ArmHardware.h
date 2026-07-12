#ifndef ARM_H
#define ARM_H

#pragma once

#include "Arm.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "mission_control.h"
#include "phidget22.h"
#include <iostream>
#include <vector>

class ArmHardware : Arm {
public:
    /**
     * @brief Completes the initialisation for all motors That make up the arm
     * and the claw.
     * @param motorTypes List of motor types  with length equal to the DOF of
     * the arm
     */
    ArmHardware(const std::vector<MotorType> motorTypes);

    /**
     * @brief Completes the deinitialisation for all motors That make up the arm
     * as well as the claw servo that were initialised in the constructor.
     */
    ~ArmHardware();

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
     * @brief Function to get the motor handler and motor type for the claw
     * servo.
     * @param retVal Pointer to a stuct that will get updated with the motor
     * handler and motor type.
     * @param index The index starting from the base motor on the arm to get the
     * handler for.
     * @return True is successful, false otherwise
     */
    bool getArmEncoderHandle(MotorHandlerReturn* retVal, int index);

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
    const std::vector<MotorType> m_motorTypes;
    std::vector<PhidgetDCMotorHandle> m_handlesDC = {};
    std::vector<PhidgetStepperHandle> m_handlesStepper = {};
    std::vector<PhidgetEncoderHandle> m_handlesEncoder = {};
    PhidgetRCServoHandle* m_handleClaw;
};

#endif