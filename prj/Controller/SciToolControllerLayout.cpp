#include "SciToolControllerLayout.h"
#include <string>

void SciToolControllerLayout::leftTriggerResponse(int16_t axisValue) {
    if (INT16_MAX == axisValue) {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_HEIGHT, LOWER);
    } else {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_HEIGHT, STOP);
    }
}

void SciToolControllerLayout::rightTriggerResponse(int16_t axisValue) {
    if (INT16_MAX == axisValue) {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_HEIGHT, RAISE);
    } else {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_HEIGHT, STOP);
    }
}

void SciToolControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void SciToolControllerLayout::incrementServoAngle(int increment) {
    incrementVal(&servoAngle, increment, 0, 180, "servoAngle");
    sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_SERVO, servoAngle);
}
