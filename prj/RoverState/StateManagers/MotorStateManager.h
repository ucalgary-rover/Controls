#pragma once

#include "ArmMotorStateManager.h"
#include "DriveMotorStateManager.h"
#include "MotorState.h"

class MotorStateManager {
public:
    MotorStateManager() { }

    MotorStateManager(MotorState initialState) :
        driveStateManager(initialState.driveMotorState),
        armStateManager(initialState.armMotorState) { }

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

    DriveMotorStateManager* getDriveStateManager();
    ArmMotorStateManager* getArmStateManager();

private:
    DriveMotorStateManager driveStateManager;
    ArmMotorStateManager armStateManager;
};
