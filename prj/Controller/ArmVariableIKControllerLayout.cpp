#include "ArmVariableIKControllerLayout.h"
static const char* file = "ArmVariableIKControllerLayout";

void ArmVariableIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    ArmState armState = stateManager.getAndLock();
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    ArmState armState = stateManager.getAndLock();
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    ArmState armState = stateManager.getAndLock();
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    ArmState armState = stateManager.getAndLock();
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::incrementPitch(int value) {
    ArmState armState = stateManager.getAndLock();
    incrementVal(&armState.pitch, value, -maxPitch, maxPitch, "clawPitch");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::incrementRoll(int value) {
    ArmState armState = stateManager.getAndLock();
    incrementVal(&armState.roll, value, -maxRoll, maxRoll, "clawPitch");
    stateManager.updateAndUnlock(armState);
}

void ArmVariableIKControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}