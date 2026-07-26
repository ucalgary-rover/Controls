#pragma once

#include <memory>

#include "ArmMotorStateManager.h"
#include "DriveMotorStateManager.h"
#include "MotorState.h"

class MotorStateManager {
public:
    MotorStateManager(MotorState initialState = {}) {
        driveStateManager = std::make_shared<DriveMotorStateManager>(
            initialState.driveMotorState);
        armStateManager = std::make_shared<ArmMotorStateManager>(
            initialState.armMotorState);
    }

    MotorStateManager& operator=(const MotorStateManager& other) {
        if (this != &other) {
            driveStateManager = other.driveStateManager;
            armStateManager = other.armStateManager;
        }

        return *this;
    }

    void updateDriveMotorState(DriveMotorState newDriveState);

    void updateArmMotorState(ArmMotorState newArmState);

    void updateState(MotorState newState);

    MotorState getState();

    std::shared_ptr<DriveMotorStateManager> getDriveStateManager();
    std::shared_ptr<ArmMotorStateManager> getArmStateManager();

private:
    std::shared_ptr<DriveMotorStateManager> driveStateManager;
    std::shared_ptr<ArmMotorStateManager> armStateManager;
};
