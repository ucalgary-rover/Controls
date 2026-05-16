#include "ArmVariableIKControllerLayout.h"
static const char* file = "ArmVariableIKControllerLayout";

void ArmVariableIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.x, &armState.y, 0.0001, 0.0001, 1,
                    1, "wristX", "wristY");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmVariableIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    if (stateManager)
        armState = stateManager->getState();
    stickChangeAxis(xValue, yValue, &armState.z, nullptr, 0.0001, 0, 1, 0,
                    "wristZ", "");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmVariableIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmVariableIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    if (stateManager)
        armState = stateManager->getState();
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmVariableIKControllerLayout::buttonResponse(uint8_t buttonID) {
    buttonCallbacks[buttonID](buttonID);
}

void ArmVariableIKControllerLayout::incrementPitch(int value) {
    if (stateManager)
        armState = stateManager->getState();
    incrementVal(&armState.pitch, value, -maxPitch, maxPitch, "clawPitch");
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmVariableIKControllerLayout::incrementRoll(int value) {
    if (stateManager)
        armState = stateManager->getState();
    incrementVal(&armState.roll, value, -maxRoll, maxRoll, "clawPitch");
    if (stateManager)
        stateManager->updateState(armState);
}