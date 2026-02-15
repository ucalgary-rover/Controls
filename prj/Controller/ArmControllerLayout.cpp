#include "ArmControllerLayout.h"

void ArmControllerLayout::buttonResponse(uint8_t buttonID) {
    switch (buttonID) {
    case SDL_CONTROLLER_BUTTON_BACK:
        currentLayout = FIXED_IK;
        break;

    case SDL_CONTROLLER_BUTTON_GUIDE:
        currentLayout = MANUAL;
        break;

    case SDL_CONTROLLER_BUTTON_START:
        currentLayout = VARIABLE_IK;

    default:
        layouts[currentLayout]->buttonResponse(buttonID);
    }
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
