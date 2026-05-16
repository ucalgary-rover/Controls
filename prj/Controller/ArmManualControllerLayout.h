#pragma once

#include "ArmMotorStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

typedef std::function<void(uint8_t)> ButtonCallback;

#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

class ArmManualControllerLayout : public ControllerLayout {
public:
    ArmManualControllerLayout(ArmMotorStateManager* armStateManager) :
        ControllerLayout("ArmManualController") {
        stateManager = armStateManager;
        if (stateManager)
            armState = stateManager->getState();

        // Initialize Layout API
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_BACK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_GUIDE, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_START, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSTICK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSTICK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementJointOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementJointOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_MISC1, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE1, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE2, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE3, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE4, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_TOUCHPAD, unusedButton);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmMotorStateManager* stateManager;
    ArmMotorState armState = {};

    ButtonCallback buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    int manualAngleChange = 0;
    MotorID joint = MOTOR_ID_BASE;

    // helper functions
    void triggerToincrementJointValue(int triggerVal, int increment);

    void incrementJoint(int change);

    // Button Callbacks
    void decrementJointOnce(uint8_t buttonID) { incrementJoint(-1); }
    void incrementJointOnce(uint8_t buttonID) { incrementJoint(1); }
};