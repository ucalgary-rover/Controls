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

using ArmToMotorStateFunc = std::function<ArmMotorState(const ArmState&)>;
using MotorToArmStateFunc = std::function<ArmState(const ArmMotorState&)>;

struct ArmControlState {
    ArmState armState;
    ArmMotorState armMotorState;
};

class ArmControllerLayout : public ControllerLayout {
public:
    ArmControllerLayout(ArmToMotorStateFunc armToMotorStateFunc,
                        MotorToArmStateFunc motorToArmStateFunc) :
        ControllerLayout("ArmController") {

        layouts[ARM_FIXED_IK] = std::make_shared<ArmFixedIKControllerLayout>(
            armAutomaticStateManager);
        layouts[ARM_VARIABLE_IK]
            = std::make_shared<ArmVariableIKControllerLayout>(
                armAutomaticStateManager);
        layouts[ARM_MANUAL] = std::make_shared<ArmManualControllerLayout>(
            armManualStateIncrementManager, armManualStateDeltaManager);

        armToMotorState = armToMotorStateFunc;
        motorToArmState = motorToArmStateFunc;

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B,
                        switchLayoutVariableIK);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X,
                        switchLayoutFixedIK);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y,
                        switchLayoutManual);
    }

    ArmControlState getControlState(uint64_t elapsed_ms);

    std::string getName() override { return layouts[currentLayout]->getName(); }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmStateManager armAutomaticStateManager
        = {}; // Handles arm state passing between base and controller thread
    ArmMotorStateManager armManualStateIncrementManager
        = {}; // Handles motor state increment passing between base and controller thread
    ArmMotorStateManager armManualStateDeltaManager
        = {}; // Handles motor state delta (time dependent) passing between base and controller thread
    ArmMotorStateManager desiredMotorStateManager
        = {}; // Stores desired motor state

    ArmToMotorStateFunc armToMotorState;
    MotorToArmStateFunc motorToArmState;

    ArmLayout currentLayout = ArmLayout::ARM_FIXED_IK;
    std::shared_ptr<ControllerLayout> layouts[3];

    // helper function
    void switchLayout(ArmLayout layout);

    // button callbacks
    void switchLayoutManual(uint8_t buttonID) { switchLayout(ARM_MANUAL); }
    void switchLayoutVariableIK(uint8_t buttonID) {
        switchLayout(ARM_VARIABLE_IK);
    }
    void switchLayoutFixedIK(uint8_t buttonID) { switchLayout(ARM_FIXED_IK); }
};
