#pragma once

#include <memory>

#include "ArmMotorStateManager.h"
#include "ArmProcessor.h"
#include "ControllerLayout.h"
#include "pub_general.h"

class ArmManualControllerLayout : public ControllerLayout {
public:
    ArmManualControllerLayout(std::shared_ptr<ArmProcessor> armProcessor,
                              messageFunc sendStepperZeroMessage) :
        ControllerLayout("ArmManualController") {

        this->armProcessor = armProcessor;
        this->sendStepperZeroMessage = sendStepperZeroMessage;

        // Initialize Layout API
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, setMin);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, setMax);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSTICK, printLimits);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSTICK, resetLimits);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementJointValueOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementJointValueOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementJointOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementJointOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, setStepperZero);

        // clang-format off
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    std::shared_ptr<ArmProcessor> armProcessor;
    messageFunc sendStepperZeroMessage;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    int manualAngleChange = 0;
    MotorID joint = MOTOR_ID_BASE;

    // helper functions
    void triggerToDeltaJointValue(int triggerVal);

    void incrementJoint(int change);

    void incrementJointValue(int increment);

    // Button Callbacks
    void setStepperZero(uint8_t buttonID) {
        for (int wheel = 0; wheel < DRIVE_INDEX_WHEEL_COUNT; wheel++) {
            armProcessor->setJointSpaceState(ArmMotorState{});
        }
        sendStepperZeroMessage(1);
    };
    void setMin(uint8_t buttonID);
    void setMax(uint8_t buttonID);
    void printLimits(uint8_t buttonID);
    void resetLimits(uint8_t buttonID);
    void decrementJointValueOnce(uint8_t buttonID) { incrementJointValue(-1); }
    void incrementJointValueOnce(uint8_t buttonID) { incrementJointValue(1); }
    void decrementJointOnce(uint8_t buttonID) { incrementJoint(-1); }
    void incrementJointOnce(uint8_t buttonID) { incrementJoint(1); }
};