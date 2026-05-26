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

#define REGISTER_ARM_BUTTON(callbacks, buttonID, buttonCallback)               \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

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

        // Initialize button callbacks
        // clang-format off
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_BACK, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_GUIDE, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_START, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSTICK, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSTICK, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, unusedButton);
        REGISTER_ARM_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, unusedButton);
        // clang-format on
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
    std::function<void(uint8_t)> buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];
};
