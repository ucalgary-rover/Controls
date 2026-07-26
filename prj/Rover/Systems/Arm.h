#pragma once

#include "mission_control.h"
#include "pub_general.h"

class Arm { // abstract
public:
    /**
     * @brief Turns the arm joint to a specified angle
     * @param joint the joint to update
     * @param angle the angle to turn the joint to
     * @return None
     */
    virtual void setJointAngle(MotorID joint, float angle) = 0;
    virtual float getJointAngle(MotorID joint) = 0;
};