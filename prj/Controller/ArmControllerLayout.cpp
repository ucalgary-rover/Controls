#include "ArmControllerLayout.h"

static const char* file = "ArmControllerLayout";

void ArmControllerLayout::buttonResponse(uint8_t buttonID) {
    switch (buttonID) {
    case SDL_CONTROLLER_BUTTON_X:
        currentLayout = FIXED_IK;
        Logging::logI(file, "Fixed_IK active");
        break;

    case SDL_CONTROLLER_BUTTON_Y:
        currentLayout = MANUAL;
        Logging::logI(file, "Manual active");
        break;

    case SDL_CONTROLLER_BUTTON_B:
        currentLayout = VARIABLE_IK;
        Logging::logI(file, "Variable_IK active");
        break;

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
