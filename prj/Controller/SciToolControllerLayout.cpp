#include "SciToolControllerLayout.h"
#include <string>

void SciToolControllerLayout::leftTriggerResponse(int16_t axisValue) {
    if (INT16_MAX == axisValue) {
        void lowerSciTool();
    } else {
        void stopSciTool();
    }
}

void SciToolControllerLayout::rightTriggerResponse(int16_t axisValue) {
    if (INT16_MAX == axisValue) {
        void raiseSciTool();
    } else {
        void stopSciTool();
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