#include "DriveManualControllerLayout.h"
#include <string>

void DriveManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelAngle(axisValue, -1.0, &lastleftTriggerValue);
}

void DriveManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToincrementWheelAngle(axisValue, 1.0, &lastrightTriggerValue);
}

void DriveManualControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void DriveManualControllerLayout::incrementWheelAngle(float increment) {
    float val = 0;
    std::string logMessage = "wheel: " + std::to_string(wheel);
    incrementVal(&val, increment, (float)0, (float)360, logMessage.c_str());
    driveProcessor->incrementSteerPosition(wheel, val);
}

void DriveManualControllerLayout::triggerToincrementWheelAngle(
    int triggerVal, float increment, int* lastTriggerValue) {
    float val = 0;
    std::string logMessage = "wheel: " + std::to_string(wheel);
    triggerToIncrement(triggerVal, lastTriggerValue, &val, increment, (float)0,
                       (float)360, logMessage.c_str());
    driveProcessor->incrementSteerPosition(wheel, val);
}

void DriveManualControllerLayout::incrementWheel(int change) {
    wheel = static_cast<WheelID>((wheel + change + WHEEL_COUNT) % WHEEL_COUNT);
    Logging::logI(filename.c_str(), "Changing to joint: %d", wheel);
}