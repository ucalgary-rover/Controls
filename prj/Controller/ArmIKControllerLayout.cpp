#include "ArmIKControllerLayout.h"

static const char* file = "ArmIKControllerLayout";

void ArmIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    ArmState armState = {};
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    ArmState armState = {};
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    ArmState armState = {};
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    ArmState armState = {};
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::incrementPitch(int value) {
    ArmState armState = {};
    incrementVal(&armState.pitch, value, -maxPitch, maxPitch, "clawPitch");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::incrementRoll(int value) {
    ArmState armState = {};
    incrementVal(&armState.roll, value, -maxRoll, maxRoll, "clawPitch");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}