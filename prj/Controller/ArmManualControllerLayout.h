#pragma once

#include "ArmMotorStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

class ArmManualControllerLayout : public ControllerLayout {
public:
    ArmManualControllerLayout(ArmMotorStateManager& armStateManager) :
        ControllerLayout("ArmManualController"), stateManager(armStateManager) {

        // Initialize Layout API
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementJointValueOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementJointValueOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementJointOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementJointOnce);
        // clang-format off
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmMotorStateManager& stateManager;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    int manualAngleChange = 0;
    MotorID joint = MOTOR_ID_BASE;

    // helper functions
    void triggerToincrementJointValue(int triggerVal, int increment);

    void incrementJoint(int change);

    void incrementJointValue(int increment);

    // Button Callbacks
    void decrementJointValueOnce(uint8_t buttonID) { incrementJointValue(-1); }
    void incrementJointValueOnce(uint8_t buttonID) { incrementJointValue(-1); }
    void decrementJointOnce(uint8_t buttonID) { incrementJoint(-1); }
    void incrementJointOnce(uint8_t buttonID) { incrementJoint(1); }
};