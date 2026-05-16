#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

typedef std::function<void(uint8_t)> ButtonCallback;

#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

class ArmVariableIKControllerLayout : public ControllerLayout {
public:
    ArmVariableIKControllerLayout(ArmStateManager* armStateManager) :
        ControllerLayout("ArmVariableIKController") {
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
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, pitchUp);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, pitchDown);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, rollLeft);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, rollRight);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_MISC1, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE1, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE2, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE3, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_PADDLE4, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_TOUCHPAD, unusedButton);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmStateManager* stateManager;
    ArmState armState = {};

    ButtonCallback buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    // Variables for state of chassis (Rover body)
    int maxPitch = 90; // Maximum pitch of the claw
    int maxRoll = 180; // Maximum roll of the claw

    // Helper functions
    void incrementPitch(int value);
    void incrementRoll(int value);

    // Button Callbacks
    void rollLeft(uint8_t buttonID) { incrementRoll(-5); }
    void rollRight(uint8_t buttonID) { incrementRoll(5); }
    void pitchUp(uint8_t buttonID) { incrementPitch(5); }
    void pitchDown(uint8_t buttonID) { incrementPitch(-5); }
};
