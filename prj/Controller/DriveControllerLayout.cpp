#include "DriveControllerLayout.h"

static const char* file = "DriveControllerLayout";

void DriveControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void DriveControllerLayout::leftStickResponse(int xValue, int yValue) {
    drivelayouts[currentLayout]->leftStickResponse(xValue, yValue);
}

void DriveControllerLayout::rightStickResponse(int xValue, int yValue) {
    drivelayouts[currentLayout]->rightStickResponse(xValue, yValue);
}

void DriveControllerLayout::leftTriggerResponse(int16_t axisValue) {
    drivelayouts[currentLayout]->leftTriggerResponse(axisValue);
}

void DriveControllerLayout::rightTriggerResponse(int16_t axisValue) {
    drivelayouts[currentLayout]->rightTriggerResponse(axisValue);
}