#include "MotorStateManager.h"

void MotorStateManager::updateDriveMotorState(DriveMotorState newDriveState) {
    driveStateManager.updateState(newDriveState);
}

void MotorStateManager::updateArmMotorState(ArmMotorState newArmState) {
    armStateManager.updateState(newArmState);
}

void MotorStateManager::updateState(MotorState newState) {
    driveStateManager.updateState(newState.driveMotorState);
    armStateManager.updateState(newState.armMotorState);
}

MotorState MotorStateManager::getState() {
    MotorState outputState = { .driveMotorState = driveStateManager.getState(),
                               .armMotorState = armStateManager.getState() };

    return outputState;
}

DriveMotorStateManager* MotorStateManager::getDriveStateManager() {
    return &driveStateManager;
}

ArmMotorStateManager* MotorStateManager::getArmStateManager() {
    return &armStateManager;
}
