#include "ArmControllerLayout.h"

static const char* file = "ArmControllerLayout";

void ArmControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    //prioritize top layout if a callback is set
    if (buttonCallbacks[buttonID]) {
        buttonCallbacks[buttonID](buttonID);
        return;
    }

    layouts[currentLayout]->buttonResponse(buttonID);
}

void ArmControllerLayout::leftStickResponse(int xValue, int yValue) {
    layouts[currentLayout]->leftStickResponse(xValue, yValue);
}

void ArmControllerLayout::rightStickResponse(int xValue, int yValue) {
    layouts[currentLayout]->rightStickResponse(xValue, yValue);
}

void ArmControllerLayout::leftTriggerResponse(int16_t axisValue) {
    layouts[currentLayout]->leftTriggerResponse(axisValue);
}

void ArmControllerLayout::rightTriggerResponse(int16_t axisValue) {
    layouts[currentLayout]->rightTriggerResponse(axisValue);
}

void ArmControllerLayout::switchLayout(ArmLayout layout) {
    if (currentLayout == layout) {
        return;
    }

    Logging::logI(file, "Switching to %s", layouts[layout]->getName());

    currentLayout = layout;
}