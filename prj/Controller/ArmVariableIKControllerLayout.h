#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"

class ArmVariableIKControllerLayout : public ControllerLayout {
public:
    ArmVariableIKControllerLayout() { }

    ArmVariableIKControllerLayout(ArmStateManager* armStateManager) :
        ControllerLayout("ArmVariableIKController") {
        stateManager = armStateManager;
        if (stateManager)
            armState = stateManager->getState();
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

    void incrementPitch(int value);

    void incrementRoll(int value);

private:
    ArmStateManager* stateManager;
    ArmState armState = {};

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    // Variables for state of chassis (Rover body)
    int maxPitch = 90; // Maximum pitch of the claw
    int maxRoll = 180; // Maximum roll of the claw
};
