#pragma once

#include <memory>

#include "ArmStateManager.h"
#include "DriveStateManager.h"
#include "RoverState.h"

class RoverStateManager {
public:
    RoverStateManager() { }

    RoverStateManager(RoverState initialState) {
        driveStateManager
            = std::make_shared<DriveStateManager>(initialState.driveState);
        armStateManager
            = std::make_shared<ArmStateManager>(initialState.armState);
    }

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

    std::shared_ptr<DriveStateManager> getDriveStateManager();
    std::shared_ptr<ArmStateManager> getArmStateManager();

private:
    std::shared_ptr<DriveStateManager> driveStateManager;
    std::shared_ptr<ArmStateManager> armStateManager;
};
