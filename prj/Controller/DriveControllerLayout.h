#pragma once

#include "DriveDefaultControllerLayout.h"
#include "DriveManualControllerLayout.h"
#include <SDL2/SDL.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

enum drivelayout {
    DRIVE_DEFAULT,
    DRIVE_MANUAL
};

#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

class DriveControllerLayout : public ControllerLayout {
public:
    DriveControllerLayout(DriveStateManager* driveStateManager,
                          DriveMotorStateManager* driveMotorStateManager) {
        drivelayouts[DRIVE_DEFAULT]
            = std::make_shared<DriveDefaultControllerLayout>(driveStateManager);
        drivelayouts[DRIVE_MANUAL]
            = std::make_shared<DriveManualControllerLayout>(
                driveMotorStateManager);

        currentLayout = DRIVE_DEFAULT;

        // Initialize button callbacks
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, swtichToDefault);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, swtichToManual);
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

    //
    std::string getName() { return drivelayouts[currentLayout]->getName(); }

    // button layout here (do the drivelayout switching)
    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

    //buton callbacks
    void swtichToManual(uint8_t buttonID) { currentLayout = DRIVE_MANUAL; }
    void swtichToDefault(uint8_t buttonID) { currentLayout = DRIVE_DEFAULT; }

private:
    enum drivelayout currentLayout;
    std::shared_ptr<ControllerLayout> drivelayouts[3];
    std::function<void(uint8_t)> buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];
};
