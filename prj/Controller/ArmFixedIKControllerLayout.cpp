#include "ArmFixedIKControllerLayout.h"

void ArmFixedIKControllerLayout::handleLeftJoyStick(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::handleRightJoyStick(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::handleLeftTrigger(int xValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(xValue, &lastleftTriggerValue, &armState.clawOpen, -5, 0,
                       100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmFixedIKControllerLayout::handleRightTrigger(int xValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(xValue, &lastrightTriggerValue, &armState.clawOpen, 5, 0,
                       100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}