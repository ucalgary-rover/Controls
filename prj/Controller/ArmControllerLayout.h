#pragma once

#include <memory>
#include <string>

#include "ArmIKControllerLayout.h"
#include "ArmManualControllerLayout.h"
#include "ArmProcessor.h"

enum ArmLayout {
    ARM_IK,
    ARM_MANUAL,
};

class ArmControllerLayout : public ControllerLayout {
public:
    ArmControllerLayout(std::shared_ptr<ArmProcessor> armProcessor) :
        ControllerLayout("ArmController") {

        layouts[ARM_IK] = std::make_shared<ArmIKControllerLayout>(armProcessor);
        layouts[ARM_MANUAL]
            = std::make_shared<ArmManualControllerLayout>(armProcessor);

        // Initialize button callbacks
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, switchLayoutIK);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, switchLayoutManual);
        // clang-format on
    }

    std::string getName() override { return layouts[currentLayout]->getName(); }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    ArmLayout currentLayout = ArmLayout::ARM_IK;
    std::shared_ptr<ControllerLayout> layouts[3];

    // helper function
    void switchLayout(ArmLayout layout);

    // button callbacks
    void switchLayoutManual(uint8_t buttonID) { switchLayout(ARM_MANUAL); }
    void switchLayoutIK(uint8_t buttonID) { switchLayout(ARM_IK); }
};
