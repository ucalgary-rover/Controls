#pragma once

#include "Logging.h"
#include "Rover/pub_rover.h"
#include "SciTool.h"

#define SCI_TOOL_SERVO_MIN_PW 1000
#define SCI_TOOL_SERVO_MAX_PW 2000

class MockSciTool : public SciTool {
public:
    /** 
     * @brief Initilizes the SciToolHardware class and all of its components. This includes the 2 servo motors and 2 DC motors.
     */
    MockSciTool();

    /** 
     * @brief Deinitilizes the SciToolHardware class and all of its components. This includes the 2 servo motors and 2 DC motors.
     */
    ~MockSciTool();

    /**
     * @brief Function to set the angle of a servo motor on the science tool.
     * @param servo The index of the servo motor to set the angle for.
     * @param angle The angle to set the servo motor to.
     */
    void setGateServoAngle(ScienceToolIndex servo, float angle);

    /**
     * @brief Function to set the speed of the DC motor powering the lift on the science tool.
     * @param speedPercentage The speed to set the DC motor to from -100 to 100%.
     */
    void setLiftSpeed(float speedPercentage);

    /**
     * @brief Function to set the speed of the DC motor powering the brush on the science tool.
     * @param speedPercentage The speed to set the DC motor to from -100 to 100%.
     */
    void setBrushSpeed(float speedPercentage);
};