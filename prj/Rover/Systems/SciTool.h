#pragma once

#include "Rover/pub_rover.h"

class SciTool { // abstract
public:
    virtual ~SciTool() = default;

    /**
     * @brief Function to set the angle of a servo motor on the science tool.
     * @param servo The index of the servo motor to set the angle for.
     * @param angle The angle to set the servo motor to.
     */
    virtual void setGateServoAngle(ScienceToolIndex servo, float angle) = 0;

    /**
     * @brief Function to set the speed of the DC motor powering the lift on the science tool.
     * @param speedPercentage The speed to set the DC motor to from -100 to 100%.
     */
    virtual void setLiftSpeed(float speedPercentage) = 0;

    /**
     * @brief Function to set the speed of the DC motor powering the brush on the science tool.
     * @param speedPercentage The speed to set the DC motor to from -100 to 100%.
     */
    virtual void setBrushSpeed(float speedPercentage) = 0;
};