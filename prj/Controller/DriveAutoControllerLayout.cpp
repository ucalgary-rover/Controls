#include "DriveAutoControllerLayout.h"

#include "Logging.h"

void DriveAutoControllerLayout::checkState(SDL_GameControllerButton button) {
    // Create array length 11 with desired input chain
    static int desiredInputChain[11]
        = { SDL_CONTROLLER_BUTTON_DPAD_UP,   SDL_CONTROLLER_BUTTON_DPAD_UP,
            SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_CONTROLLER_BUTTON_DPAD_DOWN,
            SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
            SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
            SDL_CONTROLLER_BUTTON_B,         SDL_CONTROLLER_BUTTON_A,
            SDL_CONTROLLER_BUTTON_START };

    // create array of length 11 with current input chain
    static int currentInputChain[11] = { SDL_CONTROLLER_BUTTON_INVALID };

    // save index of last saved input
    static int lastSavedIndex = 10;

    // each time a new button is pressed save the input and increment index
    if (button != SDL_CONTROLLER_BUTTON_INVALID) {
        lastSavedIndex++;
        if (lastSavedIndex >= 11) {
            lastSavedIndex = 0; // reset index to 0
        }

        currentInputChain[lastSavedIndex] = button;
    }

    bool match = true;
    // for i in range 11 match current with desired starting index
    for (int i = 0; i < 11; i++) {
        match &= (desiredInputChain[i]
                  == currentInputChain[(i + lastSavedIndex + 1) % 11]);
    }

    if (!match) {
        return;
    }

    Logging::logI(filename.c_str(),
                  "SUPER ULTRA SECRET HACKER SEQUENCE ENTERED");
    Logging::logI(filename.c_str(), "ENGAGING HYPER DRIVE THRUSTERS!!!!");

    absoluteMaxSpeed = 100;
}

void DriveAutoControllerLayout::setVelocity(int X, int Y) {
    DriveState driveState = driveStateManager.getState();
    // get the speed
    setVal(&driveState.speed, stickMagnitude(X, Y), 0, presentMaxSpeed,
           NAMEOF(driveState.speed));

    // get the angle
    setVal(&driveState.heading, stickAngle(X, Y), 0, 360,
           NAMEOF(driveState.heading));

    driveStateManager.updateState(driveState);
}

void DriveAutoControllerLayout::setAngularVelocity(int X, int Y) {
    DriveState driveState = driveStateManager.getState();
    // get the angular velocity
    setVal(&driveState.angularVelocity, (X * maxRadialSpeed) / 255,
           -maxRadialSpeed, maxRadialSpeed, NAMEOF(driveState.angularVelocity));

    driveStateManager.updateState(driveState);
}

void DriveAutoControllerLayout::incrementMaxSpeed(int val) {
    DriveState driveState = driveStateManager.getState();
    incrementVal(&presentMaxSpeed, val, 0, absoluteMaxSpeed, "chassisMaxSpeed");
    driveStateManager.updateState(driveState);
}