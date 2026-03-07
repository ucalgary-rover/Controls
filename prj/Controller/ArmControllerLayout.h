#pragma once

#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"
#include <map>
#include <string>

enum layout {
    FIXED_IK,
    VARIABLE_IK,
    MANUAL
};

class ArmControllerLayout : public ControllerLayout {
public:
    ArmControllerLayout(ArmStateManager* armStateManager,
                        ArmMotorStateManager* armMotorStateManager) {
        layouts[FIXED_IK] = new ArmFixedIKControllerLayout(armStateManager);
        layouts[VARIABLE_IK]
            = new ArmVariableIKControllerLayout(armStateManager);
        layouts[MANUAL] = new ArmManualControllerLayout(armMotorStateManager);

        currentLayout = FIXED_IK;
    }

    //
    std::string getName() { return layouts[currentLayout]->getName(); }

    // button layout here (do the layout switching)
    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    int currentLayout;
    ControllerLayout* layouts[3];
};
