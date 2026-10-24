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
    DriveMotorState driveMotorState = driveStateManager->getState();
    WheelID index;
    for (int wheel = 0; wheel < DRIVE_INDEX_WHEEL_COUNT; wheel++) {
        index = static_cast<WheelID>(wheel);
        float angle = std::fmod(driveMotorState.steer[index], 360.0f);
        if (angle < 0)
            driveMotorState.steer[index] += 360.0f;
    }
    MotorState outputState = { .driveMotorState = driveMotorState,
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
