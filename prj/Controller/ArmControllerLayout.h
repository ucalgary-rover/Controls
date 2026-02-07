#pragma once

#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"
#include <map>
#include <string>

enum layout {
    FixedIK,
    ArmVariableIK,
    ArmManual
};

class ArmControllerLayout : public ControllerLayout {
public:
    ArmControllerLayout() { }

    ArmControllerLayout(ArmStateManager* armStateManager,
                        ArmMotorStateManager* armMotorStateManager) {
        ArmFixedIKControllerLayout armFixedIKLayout
            = ArmFixedIKControllerLayout(armStateManager);
        ArmVariableIKControllerLayout armVariableIKLayout
            = ArmVariableIKControllerLayout(armStateManager);
        ArmManualControllerLayout armManualLayout
            = ArmManualControllerLayout(armMotorStateManager);

        layouts[FixedIK] = &armFixedIKLayout;
        layouts[ArmVariableIK] = &armVariableIKLayout;
        layouts[ArmManual] = &armManualLayout;

        currentLayout = FixedIK;
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

    ArmManualControllerLayout armManualLayout;
    ArmFixedIKControllerLayout armFixedIKLayout;
    ArmVariableIKControllerLayout armVariableIKLayout;

    ControllerLayout* layouts[3];
};
