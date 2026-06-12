#pragma once

#include <functional>
#include <memory>
#include <string>

#include "ArmFixedIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmVariableIKControllerLayout.h"

enum ArmLayout {
    ARM_FIXED_IK,
    ARM_VARIABLE_IK,
    ARM_MANUAL,
};

using ProcessArmStateFunc = std::function<ArmMotorState(const ArmState&)>;

class ArmControllerLayout : public ControllerLayout {
public:
    ArmControllerLayout(ProcessArmStateFunc processFunc) :
        ControllerLayout("ArmController") {

        layouts[ARM_FIXED_IK]
            = std::make_shared<ArmFixedIKControllerLayout>(armStateManager);
        layouts[ARM_VARIABLE_IK]
            = std::make_shared<ArmVariableIKControllerLayout>(armStateManager);
        layouts[ARM_MANUAL] = std::make_shared<ArmManualControllerLayout>(
            armManualStateManager);

        process = processFunc;

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B,
                        switchLayoutVariableIK);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X,
                        switchLayoutFixedIK);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y,
                        switchLayoutManual);
    }

    ArmMotorState getArmMotorState(uint64_t elapsed_ms) {
        if (currentLayout == ARM_MANUAL) {
            ArmMotorState delta = armManualStateManager.getState();
            for (int motor = 0; motor < MOTOR_ID_END; motor++) {
                desiredMotorState.motorValues[motor]
                    += delta.motorValues[motor] * elapsed_ms;
            }
        } else {
            desiredMotorState = process(armStateManager.getState());
        }

        return desiredMotorState;
    }

    std::string getName() override { return layouts[currentLayout]->getName(); }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmStateManager armStateManager = {};
    ArmMotorStateManager armManualStateManager = {};
    ArmMotorState desiredMotorState = {};

    ProcessArmStateFunc process;

    ArmLayout currentLayout = ArmLayout::ARM_FIXED_IK;
    std::shared_ptr<ControllerLayout> layouts[3];

    //helper function
    void switchLayout(ArmLayout layout) { currentLayout = layout; }

    //button callbacks
    void switchLayoutManual(uint8_t buttonID) { switchLayout(ARM_MANUAL); }
    void switchLayoutVariableIK(uint8_t buttonID) {
        switchLayout(ARM_VARIABLE_IK);
    }
    void switchLayoutFixedIK(uint8_t buttonID) { switchLayout(ARM_FIXED_IK); }
};
