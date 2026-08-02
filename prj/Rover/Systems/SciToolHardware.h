#pragma once

#include "Logging.h"
#include "Rover/pub_rover.h"
#include "SciTool.h"
#include "mission_control.h"
#include "phidget22.h"
#include "pub_systems.h"
#include <vector>

#define SCI_TOOL_SERVO_MIN_PW 1000
#define SCI_TOOL_SERVO_MAX_PW 2000

class SciToolHardware : public SciTool {
public:
    /** 
     * @brief Initilizes the SciToolHardware class and all of its components. This includes the 2 servo motors and 2 DC motors.
     */
    SciToolHardware();

    /** 
     * @brief Deinitilizes the SciToolHardware class and all of its components. This includes the 2 servo motors and 2 DC motors.
     */
    ~SciToolHardware();

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

private:
    std::vector<PhidgetRCServoHandle> m_handlesServoGate = {};
    PhidgetDCMotorHandle* m_handlesDCLift;
    PhidgetDCMotorHandle* m_handlesDCBrush;

    /**
     * @brief Function to get the servo handler for the servo motors on the science tool at a scpecific index.
     * @param retVal Pointer to a stuct that will get updated with the motor handler and motor type.
     * @param index The index starting from the left servo on the science tool to get the handler for.
     * @return True is successful, false otherwise
     */
    bool getSciToolServoHandle(MotorHandlerReturn* retVal, int index);

    /**
     * @brief Function to get the motor handler and motor type for the DC motor poweriung the lift on the science tool.
     * @param retVal Pointer to a stuct that will get updated with the motor handler and motor type.
     * @return True is successful, false otherwise
     */
    bool getSciToolDCLiftHandle(MotorHandlerReturn* retVal);

    /**
     * @brief Function to get the motor handler and motor type for the DC motor powering the bruch on the science tool.
     * @param retVal Pointer to a stuct that will get updated with the motor handler and motor type.
     * @return True is successful, false otherwise
     */
    bool getSciToolDCBrushHandle(MotorHandlerReturn* retVal);
};