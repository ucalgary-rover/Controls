#include "ArmIKControllerLayout.h"

static const char* file = "ArmIKControllerLayout";

static ArmStateCylindrical velocity = {};

static float mapStickX2theta(int xValue) {
    return (((float)xValue) / -255.0) * 5;
}

static float mapStickY2r(int yValue) {
    return (((float)yValue) / -255.0) * 0.05;
}

static float mapStickX2z(int xValue) {
    return (((float)xValue) / -255.0) * 0.05;
}

void ArmIKControllerLayout::leftStickResponse(int xValue, int yValue) {
    velocity.theta = mapStickX2theta(xValue);
    velocity.r = mapStickY2r(yValue);
    armProcessor->setTaskSpaceVelocity(velocity);

    Logging::logI(file, "%d %d", xValue, yValue);
}

void ArmIKControllerLayout::rightStickResponse(int xValue, int yValue) {
    velocity.z = mapStickX2z(xValue);
    armProcessor->setTaskSpaceVelocity(velocity);

    Logging::logI(file, "%d %d", xValue, yValue);
}

void ArmIKControllerLayout::leftTriggerResponse(int16_t axisValue) {
    ArmStateCylindrical armState = {};
    triggerToIncrement(axisValue, &lastleftTriggerValue, &armState.clawOpen, -5,
                       0, 100, "clawOpen");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::rightTriggerResponse(int16_t axisValue) {
    ArmStateCylindrical armState = {};
    triggerToIncrement(axisValue, &lastrightTriggerValue, &armState.clawOpen, 5,
                       0, 100, "clawOpen");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::incrementPitch(int value) {
    ArmStateCylindrical armState = {};
    incrementVal(&armState.pitch, value, -maxPitch, maxPitch, "clawPitch");
    armProcessor->incrementTaskSpaceState(armState);
}

void ArmIKControllerLayout::incrementRoll(int value) {
    ArmStateCylindrical armState = {};
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