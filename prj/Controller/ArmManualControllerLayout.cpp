#include "ArmManualControllerLayout.h"
#include <string>

void ArmManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToincrementJointValue(axisValue, -5);
}

void ArmManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToincrementJointValue(axisValue, 5);
}

void ArmManualControllerLayout::buttonResponse(uint8_t buttonID) {
    switch (buttonID) {
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        incrementJoint(-1);
        break;

    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        incrementJoint(1);
        break;
    }
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