#pragma once

#include "ArmMotorStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"

class ArmManualControllerLayout : public ControllerLayout {
public:
    ArmManualControllerLayout() { }

    ArmManualControllerLayout(ArmMotorStateManager* armStateManager) :
        ControllerLayout("ArmManualController") {
        stateManager = armStateManager;
        if (stateManager)
            armState = stateManager->getState();
    }

    void triggerToincrementJointValue(int triggerVal, int increment);

    void incrementJoint(int change);

private:
    ArmMotorStateManager* stateManager;
    ArmMotorState armState = {};

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    int manualAngleChange = 0;
    MotorID joint = MOTOR_ID_BASE;
};