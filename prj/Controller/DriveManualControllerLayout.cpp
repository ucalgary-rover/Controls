#include "DriveManualControllerLayout.h"
#include <string>

void DriveManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    if (axisValue <= INT16_MIN) {
        triggerTosetWheelAngleVelocity(axisValue, 0, &lastleftTriggerValue);
    } else {
        triggerTosetWheelAngleVelocity(axisValue, -1.0, &lastleftTriggerValue);
    }
}

void DriveManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    if (axisValue <= INT16_MIN) {
        triggerTosetWheelAngleVelocity(axisValue, 0, &lastrightTriggerValue);

    } else {
        triggerTosetWheelAngleVelocity(axisValue, 1.0, &lastrightTriggerValue);
    }
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

void DriveManualControllerLayout::triggerTosetWheelAngleVelocity(
    int triggerVal, float velocity, int* lastTriggerValue) {
    float val = 0;
    std::string logMessage = "wheel: " + std::to_string(wheel);
    triggerToIncrement(triggerVal, lastTriggerValue, &val, velocity, (float)-5,
                       (float)5, logMessage.c_str());
    driveProcessor->setSteerVelocity(wheel, val);
}

void DriveManualControllerLayout::incrementWheel(int change) {
    wheel = static_cast<WheelID>((wheel + change + DRIVE_INDEX_WHEEL_COUNT)
                                 % DRIVE_INDEX_WHEEL_COUNT);
    Logging::logI(filename.c_str(), "Changing to wheel: %d", wheel);
}