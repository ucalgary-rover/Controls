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

    void buttonResponse(uint8_t buttonID); // unused right now

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmStateManager* stateManager;
    ArmState armState = {};

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;
};
