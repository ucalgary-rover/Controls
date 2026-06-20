#pragma once

#include <SDL2/SDL.h>
#include <functional>

#include "ControllerLayout.h"
#include "DriveMotorStateManager.h"
#include "DriveStateManager.h"

using ProcessDriveStateFunc = std::function<DriveMotorState(const DriveState&)>;

class DriveAutoControllerLayout : public ControllerLayout {
public:
    DriveAutoControllerLayout(DriveStateManager& driveStateManager) :
        ControllerLayout("DriveController"), stateManager(driveStateManager) {
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementLightLevelOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementLightLevelOneStep);       
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementMaxSpeedOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementMaxSpeedOneStep);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    DriveStateManager& stateManager;
    DriveMotorState desiredMotorState;

    ProcessDriveStateFunc process;

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    int maxRadialSpeed = 45; // degrees per second

    uint8_t lightLevel = 0;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    //helper functions

    void checkState(uint8_t button);

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

    void incrementLightLevel(uint8_t val);

    //button callbacks
    void decrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(-2); };
    void incrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(2); };

    void decrementLightLevelOneStep(uint8_t buttonID) {
        incrementMaxSpeed(-20);
    }
    void incrementLightLevelOneStep(uint8_t buttonID) { incrementMaxSpeed(20); }
};
