#pragma once

#include "Arm.h"
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <vector>

class MockArm : public Arm {
public:
    MockArm();
    ~MockArm();

    /**
     * @brief Turns the arm joint to a specified angle
     * @param joint the joint to update
     * @param angle the angle to turn the joint to
     * @return None
     */
    void setJointAngle(MotorID joint, float angle) override;
    float getJointAngle(MotorID joint) override;

private:
    float joints[MotorID::MOTOR_ID_END] = {};
};