#include "MotorStateManager.h"

void MotorStateManager::updateDriveMotorState(DriveMotorState newDriveState) {
    driveStateManager->updateState(newDriveState);
}

void MotorStateManager::updateArmMotorState(ArmMotorState newArmState) {
    armStateManager->updateState(newArmState);
}

void MotorStateManager::updateState(MotorState newState) {
    driveStateManager->updateState(newState.driveMotorState);
    armStateManager->updateState(newState.armMotorState);
}

MotorState MotorStateManager::getState() {
    MotorState outputState = { .driveMotorState = driveStateManager->getState(),
                               .armMotorState = armStateManager->getState() };

    return outputState;
}

std::shared_ptr<DriveMotorStateManager>
MotorStateManager::getDriveStateManager() {
    return driveStateManager;
}

std::shared_ptr<ArmMotorStateManager> MotorStateManager::getArmStateManager() {
    return armStateManager;
}
