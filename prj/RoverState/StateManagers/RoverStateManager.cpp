#include "RoverStateManager.h"

void RoverStateManager::updateDriveState(DriveState newDriveState) {
    driveStateManager.updateState(newDriveState);
}

void RoverStateManager::updateArmState(ArmState newArmState) {
    armStateManager.updateState(newArmState);
}

void RoverStateManager::updateState(RoverState newState) {
    driveStateManager.updateState(newState.driveState);
    armStateManager.updateState(newState.armState);
}

RoverState RoverStateManager::getState() {
    RoverState outputState = { .driveState = driveStateManager.getState(),
                               .armState = armStateManager.getState() };

    return outputState;
}

DriveStateManager* RoverStateManager::getDriveStateManager() {
    return &driveStateManager;
}

ArmStateManager* RoverStateManager::getArmStateManager() {
    return &armStateManager;
}
