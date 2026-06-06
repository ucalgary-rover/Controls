#pragma once

#include "ControllerLayout.h"
#include "DriveMotorStateManager.h"
#include "pub_general.h"
#include <functional>

typedef std::function<void(uint8_t)> ButtonCallback;

#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

class DriveManualControllerLayout : public ControllerLayout {
public:
    DriveManualControllerLayout(DriveMotorStateManager* driveStateManager) :
        ControllerLayout("DriveManualController") {
        stateManager = driveStateManager;
        if (stateManager)
            driveState = stateManager->getState();

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
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementWheelOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementWheelOnce);
        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    DriveMotorStateManager* stateManager;
    DriveMotorState driveState = {};

    ButtonCallback buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    int manualAngleChange = 0;
    WheelID wheel = WHEEL_FR;

    // helper functions
    void triggerToincrementWheelAngle(int triggerVal, float increment);

    void incrementWheel(int change);

    void incrementWheelAngle(float increment);

    // Button Callbacks
    void decrementWheelAngleOnce(uint8_t buttonID) {
        incrementWheelAngle(-0.1);
    }
    void incrementWheelAngleOnce(uint8_t buttonID) { incrementWheelAngle(0.1); }
    void decrementWheelOnce(uint8_t buttonID) { incrementWheel(-1); }
    void incrementWheelOnce(uint8_t buttonID) { incrementWheel(1); }
};
