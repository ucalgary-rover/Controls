#pragma once

#include <SDL2/SDL.h>
#include <functional>

#include "ControllerLayout.h"
#include "DriveMotorStateManager.h"
#include "DriveStateManager.h"

using ProcessDriveStateFunc = std::function<DriveMotorState(const DriveState&)>;

class DriveControllerLayout : public ControllerLayout {
public:
    DriveControllerLayout(ProcessDriveStateFunc processFunc) :
        ControllerLayout("DriveController") {
        process = processFunc;
    }

    DriveMotorState getDriveMotorState(uint64_t elapsed_ms) {
        desiredMotorState = process(driveStateManager.getState());
        return desiredMotorState;
    }

    void checkState(SDL_GameControllerButton button);

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

private:
    DriveStateManager driveStateManager;
    DriveMotorState desiredMotorState;

    ProcessDriveStateFunc process;

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    int maxRadialSpeed = 45; // degrees per second
};
