#include "ArmManualControllerLayout.h"

#include <string>

void ArmManualControllerLayout::leftTriggerResponse(int16_t axisValue) {
    triggerToDeltaJointValue(axisValue);
}

void ArmManualControllerLayout::rightTriggerResponse(int16_t axisValue) {
    triggerToDeltaJointValue(axisValue);
}

void ArmManualControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX
        || !buttonCallbacks[buttonID]) {
        return;
    }

    buttonCallbacks[buttonID](buttonID);
}

void ArmManualControllerLayout::triggerToDeltaJointValue(int triggerVal) {
    ArmMotorState armVelocity = {};
    std::string logMessage = "motor: " + std::to_string(joint);
    setVal(&armVelocity.motorValues[joint], triggerVal, -20, 20,
           logMessage.c_str());
    armProcessor->setJointSpaceVelocity(armVelocity);
}

void ArmManualControllerLayout::incrementJoint(int change) {
    joint
        = static_cast<MotorID>((joint + change + MOTOR_ID_END) % MOTOR_ID_END);
    Logging::logI(filename.c_str(), "Changing to joint: %d", joint);
    manualAngleChange = 0; // Reset manual angle change when changing joint
}

void ArmManualControllerLayout::incrementJointValue(int increment) {
    ArmMotorState armIncrement = {};
    std::string logMessage = "motor: " + std::to_string(joint);
    incrementVal(&armIncrement.motorValues[joint], increment, -20, 20,
                 logMessage.c_str());
    armProcessor->incrementJointSpaceState(armIncrement);
}