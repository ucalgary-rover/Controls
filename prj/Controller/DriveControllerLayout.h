#pragma once

#include "ControllerLayout.h"
#include "DriveStateManager.h"

#include <SDL2/SDL.h>

class DriveControllerLayout : public ControllerLayout {
public:
    DriveControllerLayout() { }

    DriveControllerLayout(DriveStateManager* driveStateManager) :
        ControllerLayout("DriveController") {
        stateManager = driveStateManager;
        if (stateManager)
            driveState = stateManager->getState();
    }

    void checkState(SDL_GameControllerButton button);

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

private:
    DriveStateManager* stateManager;
    DriveState driveState;

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    int maxRadialSpeed = 45; // degrees per second
};
