#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

class ArmFixedIKControllerLayout : public ControllerLayout {
public:
    ArmFixedIKControllerLayout(ArmStateManager& armStateManager) :
        ControllerLayout("ArmFixedIKController"),
        stateManager(armStateManager) {

        // Initialize Layout API
        // clang-format off

            //No buttons are used!!!

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmStateManager& stateManager;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;
};
