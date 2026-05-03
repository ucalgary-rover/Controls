#pragma once

#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"
#include <map>
#include <memory>
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
        layouts[FIXED_IK]
            = std::make_shared<ArmFixedIKControllerLayout>(armStateManager);
        layouts[VARIABLE_IK]
            = std::make_shared<ArmVariableIKControllerLayout>(armStateManager);
        layouts[MANUAL]
            = std::make_shared<ArmManualControllerLayout>(armMotorStateManager);

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
    enum layout currentLayout;
    std::shared_ptr<ControllerLayout> layouts[3];
};
