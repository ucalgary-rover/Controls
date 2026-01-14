#pragma once

#include "ArmStateManager.h"
#include "DriveStateManager.h"
#include "RoverState.h"

class RoverStateManager {
public:
    RoverStateManager() { }

    RoverStateManager(RoverState initialState) :
        driveStateManager(initialState.driveState),
        armStateManager(initialState.armState) { }

    RoverStateManager& operator=(const RoverStateManager& other) {
        if (this != &other) {
            driveStateManager = other.driveStateManager;
            armStateManager = other.armStateManager;
        }

        return *this;
    }

    void updateDriveState(DriveState newDriveState);

    void updateArmState(ArmState newArmState);

    void updateState(RoverState newState);

    RoverState getState();

    DriveStateManager* getDriveStateManager();
    ArmStateManager* getArmStateManager();

private:
    DriveStateManager driveStateManager;
    ArmStateManager armStateManager;
};
