#ifndef ARM_H
#define ARM_H

#pragma once

#include "Arm.h"
#include "Motors/DifferentialClaw.h"
#include "Motors/Motor.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "mission_control.h"
#include "phidget22.h"

#include <iostream>
#include <memory>
#include <unordered_map>

class ArmHardware : public Arm {
public:
    /**
     * @brief Completes the initialisation for all motors That make up the arm
     * and the claw.
     */
    ArmHardware();

    /**
     * @brief Completes the deinitialisation for all motors That make up the arm
     * as well as the claw servo that were initialised in the constructor.
     */
    ~ArmHardware();

    void setJointAngle(MotorID joint, float angle) override;
    float getJointAngle(MotorID joint) override;

private:
    std::unordered_map<MotorID, std::shared_ptr<Motor>> motors = {};
    std::shared_ptr<DifferentialClaw> mrMoraleAndTheBigSteppers;
};

#endif