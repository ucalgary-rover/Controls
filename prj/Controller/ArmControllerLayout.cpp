#include "ArmControllerLayout.h"

static const char* file = "ArmControllerLayout";
static const char* layoutNames[]
    = { NAMEOF(ARM_FIXED_IK), NAMEOF(ARM_VARIABLE_IK), NAMEOF(ARM_MANUAL) };

ArmControlState ArmControllerLayout::getControlState(uint64_t elapsed_ms) {
    ArmState desiredArm;
    ArmMotorState desiredMotor;

    if (currentLayout
        == ARM_MANUAL) { // Sum motor state and back compute arm state
        desiredMotor = desiredMotorStateManager.getAndLock();
        ArmMotorState increment = armManualStateIncrementManager.getAndLock();
        ArmMotorState delta = armManualStateDeltaManager.getState();

        for (int motor = 0; motor < MOTOR_ID_END; motor++) {
            desiredMotor.motorValues[motor]
                += increment.motorValues[motor]
                   + delta.motorValues[motor] * elapsed_ms;
        }

        // Update and unlock desired motor state
        desiredMotorStateManager.updateAndUnlock(desiredMotor);

        // Clear increment after handling
        armManualStateIncrementManager.updateAndUnlock({});

        // Compute and update arm state
        ArmState currentArmState = armAutomaticStateManager.getAndLock();
        desiredArm = motorToArmState(desiredMotor);
        desiredArm.clawOpen = currentArmState.clawOpen; // Preserve claw
        armAutomaticStateManager.updateAndUnlock(desiredArm);
    } else { // Compute motor state and update
        desiredArm = armAutomaticStateManager.getState();
        desiredMotor = armToMotorState(desiredArm);
        desiredMotorStateManager.updateState(desiredMotor);
    }

    ArmControlState control = {
        .armState = desiredArm,
        .armMotorState = desiredMotor,
    };

    return control;
}

void ArmControllerLayout::buttonResponse(uint8_t buttonID) {
    if (buttonID <= SDL_CONTROLLER_BUTTON_INVALID
        || buttonID >= SDL_CONTROLLER_BUTTON_MAX) {
        return;
    }

    //prioritize top layout if a callback is set
    if (buttonCallbacks[buttonID]) {
        buttonCallbacks[buttonID](buttonID);
        return;
    }

    layouts[currentLayout]->buttonResponse(buttonID);
}

void ArmControllerLayout::leftStickResponse(int xValue, int yValue) {
    layouts[currentLayout]->leftStickResponse(xValue, yValue);
}

void ArmControllerLayout::rightStickResponse(int xValue, int yValue) {
    layouts[currentLayout]->rightStickResponse(xValue, yValue);
}

void ArmControllerLayout::leftTriggerResponse(int16_t axisValue) {
    layouts[currentLayout]->leftTriggerResponse(axisValue);
}

void ArmControllerLayout::rightTriggerResponse(int16_t axisValue) {
    layouts[currentLayout]->rightTriggerResponse(axisValue);
}

void ArmControllerLayout::switchLayout(ArmLayout layout) {
    if (currentLayout == layout) {
        return;
    }

    Logging::logI(file, "Switching to %s", layoutNames[layout]);

    if (currentLayout == ArmLayout::ARM_MANUAL) {
        ArmState currentArmState = armAutomaticStateManager.getAndLock();
        ArmState armState
            = motorToArmState(desiredMotorStateManager.getState());
        armState.clawOpen = currentArmState.clawOpen; // Preserve claw
        armAutomaticStateManager.updateAndUnlock(armState);
    } else {
        ArmMotorState armMotorState
            = armToMotorState(armAutomaticStateManager.getState());
        desiredMotorStateManager.updateState(armMotorState);
    }

    currentLayout = layout;
}