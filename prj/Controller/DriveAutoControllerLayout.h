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

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
                        decrementMaxSpeedOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
                        incrementMaxSpeedOneStep);
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

private:
    DriveStateManager& stateManager;
    DriveMotorState desiredMotorState;

    ProcessDriveStateFunc process;

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    int maxRadialSpeed = 45; // degrees per second

    //helper functions

    void checkState(uint8_t button);

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

    //button callbacks
    void decrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(-2); };
    void incrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(2); };
};
