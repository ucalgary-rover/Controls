#pragma once

#include <memory>

#include "ControllerLayout.h"
#include "DriveProcessor.h"

class DriveManualControllerLayout : public ControllerLayout {
public:
    DriveManualControllerLayout(
        std::shared_ptr<DriveProcessor> driveProcessor) :
        ControllerLayout("DriveManualController") {

        this->driveProcessor = driveProcessor;

        // Initialize Layout API
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementWheelOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementWheelOnce);
        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    std::shared_ptr<DriveProcessor> driveProcessor;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    WheelID wheel = WHEEL_FR;

    // helper functions
    void triggerToincrementWheelAngle(int triggerVal, float increment,
                                      int* lastTriggerValue);

    void incrementWheel(int change);

    void incrementWheelAngle(float increment);

    // Button Callbacks
    void decrementWheelAngleOnce(uint8_t buttonID) {
        incrementWheelAngle(-0.1);
    }
    void incrementWheelAngleOnce(uint8_t buttonID) { incrementWheelAngle(0.1); }
    void decrementWheelOnce(uint8_t buttonID) { incrementWheel(-1); }
    void incrementWheelOnce(uint8_t buttonID) { incrementWheel(1); }
};
