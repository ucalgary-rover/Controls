#include "DriveManualControllerLayout.h"
#include <string>

void DriveManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelAngle(axisValue, -1);
}

void DriveManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelAngle(axisValue, 1);
}

void DriveManualControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void DriveManualControllerLayout::incrementWheelAngle(float increment) {
    if (stateManager)
        driveState = stateManager->getState();
    std::string logMessage = "wheel: " + std::to_string(wheel);
    incrementVal(&driveState.steer[wheel], increment, (float)0, (float)360,
                 logMessage.c_str());
    if (stateManager)
        stateManager->updateState(driveState);
}

void DriveManualControllerLayout::triggerToincrementWheelAngle(
    int triggerVal, float increment) {
    if (stateManager)
        driveState = stateManager->getState();
    std::string logMessage = "wheel: " + std::to_string(wheel);
    triggerToIncrement(triggerVal, &lastleftTriggerValue,
                       &driveState.steer[wheel], increment, (float)0,
                       (float)360, logMessage.c_str());
    if (stateManager)
        stateManager->updateState(driveState);
}

void DriveManualControllerLayout::incrementWheel(int change) {
    if (stateManager)
        driveState = stateManager->getState();
    wheel = static_cast<WheelID>((wheel + change + WHEEL_COUNT) % WHEEL_COUNT);
    Logging::logI(filename.c_str(), "Changing to joint: %d", wheel);
    manualAngleChange = 0; // Reset manual angle change when changing joint
    if (stateManager)
        stateManager->updateState(driveState);
}