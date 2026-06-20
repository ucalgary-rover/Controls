#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

class ArmVariableIKControllerLayout : public ControllerLayout {
public:
    ArmVariableIKControllerLayout(ArmStateManager& armStateManager) :
        ControllerLayout("ArmVariableIKController"),
        stateManager(armStateManager) {

        // Initialize Layout API
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, pitchUp);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, pitchDown);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, rollLeft);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, rollRight);
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
