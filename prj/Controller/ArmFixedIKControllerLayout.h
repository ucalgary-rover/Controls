#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"

class ArmFixedIKControllerLayout : public ControllerLayout {
public:
    ArmFixedIKControllerLayout() { }

    ArmFixedIKControllerLayout(ArmStateManager* armStateManager) :
        ControllerLayout("ArmFixedIKController") {
        stateManager = armStateManager;
        if (stateManager)
            armState = stateManager->getState();
    }

    void handleLeftJoyStick(int xValue, int yValue);

    void handleRightJoyStick(int xValue, int yValue);

    void handleLeftTrigger(int xValue);

    void handleRightTrigger(int xValue);

private:
    ArmStateManager* stateManager;
    ArmState armState = {};

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;
};
