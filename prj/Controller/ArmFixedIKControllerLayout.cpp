#include "ArmFixedIKControllerLayout.h"

void ArmFixedIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    ArmState armState = stateManager.getState();
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    stateManager.updateState(armState);
}

void ArmFixedIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    ArmState armState = stateManager.getState();
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    stateManager.updateState(armState);
}

void ArmFixedIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    ArmState armState = stateManager.getAndLock();
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    stateManager.updateAndUnlock(armState);
}

void ArmFixedIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    ArmState armState = stateManager.getAndLock();
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    stateManager.updateAndUnlock(armState);
}

void ArmFixedIKControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}