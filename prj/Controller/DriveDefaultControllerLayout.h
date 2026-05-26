#pragma once

#include "ControllerLayout.h"
#include "DriveStateManager.h"

#include <SDL2/SDL.h>
#include <functional>

typedef std::function<void(uint8_t)> ButtonCallback;
#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }
class DriveDefaultControllerLayout : public ControllerLayout {
public:
    DriveDefaultControllerLayout() { }

    DriveDefaultControllerLayout(DriveStateManager* driveStateManager) :
        ControllerLayout("DriveDefaultController") {
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
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, unusedButton);
        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

private:
    DriveStateManager* stateManager;
    DriveState driveState;

    ButtonCallback buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    int maxRadialSpeed = 45; // degrees per second
    //Helper fucntions
    void checkState(uint8_t button);

    //sets velocity and heading

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

    //button callbacks
    void decrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(-2); };
    void incrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(2); };
};
