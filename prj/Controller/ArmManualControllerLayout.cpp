#include "ArmManualControllerLayout.h"
#include <string>

void ArmManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToincrementJointValue(axisValue, -5);
}

void ArmManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToincrementJointValue(axisValue, 5);
}

void ArmManualControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void ArmManualControllerLayout::triggerToincrementJointValue(int triggerVal,
                                                             int increment) {
    if (stateManager)
        armState = stateManager->getState();
    std::string logMessage = "motor: " + std::to_string(joint);
    triggerToIncrement(triggerVal, &lastleftTriggerValue,
                       &armState.motorValues[joint], increment, -20, 20,
                       logMessage.c_str());
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmManualControllerLayout::incrementJoint(int change) {
    if (stateManager)
        armState = stateManager->getState();
    joint
        = static_cast<MotorID>((joint + change + MOTOR_ID_END) % MOTOR_ID_END);
    Logging::logI(filename.c_str(), "Changing to joint: %d", joint);
    manualAngleChange = 0; // Reset manual angle change when changing joint
    if (stateManager)
        stateManager->updateState(armState);
}

void ArmManualControllerLayout::incrementJointValue(int increment) {
    if (stateManager)
        armState = stateManager->getState();
    std::string logMessage = "motor: " + std::to_string(joint);
    incrementVal(&armState.motorValues[joint], increment, -20, 20,
                 logMessage.c_str());
    if (stateManager)
        stateManager->updateState(armState);
}