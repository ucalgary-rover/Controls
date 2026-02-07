#include "ArmFixedIKControllerLayout.h"

void ArmFixedIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}