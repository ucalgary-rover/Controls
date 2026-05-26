#include "DriveManualControllerLayout.h"
#include <string>

void DriveManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelValue(axisValue, -1);
}

void DriveManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelValue(axisValue, 1);
}

void DriveManualControllerLayout::leftStickResponse(int xValue, int yValue) {
    float angle = stickAngle(xValue, yValue);
    if (stateManager)
        driveState = stateManager->getState();
    std::string logMessage = "wheel: " + std::to_string(wheel);
    setVal(&driveState.steer[wheel], angle, (float)0, (float)360,
           logMessage.c_str());
}

void DriveManualControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void DriveManualControllerLayout::triggerToincrementWheelValue(
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