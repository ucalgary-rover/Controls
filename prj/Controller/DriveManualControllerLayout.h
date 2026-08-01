#pragma once

#include <memory>

#include "ControllerLayout.h"
#include "DriveProcessor.h"

class DriveManualControllerLayout : public ControllerLayout {
public:
    DriveManualControllerLayout(std::shared_ptr<DriveProcessor> driveProcessor,
                                messageFunc sendZeroMessage) :
        ControllerLayout("DriveManualController") {

        this->driveProcessor = driveProcessor;
        this->sendZeroMessage = sendZeroMessage;

        // Initialize Layout API
        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementWheelAngleOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementWheelOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementWheelOnce);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, buttonZeroMessage);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    std::shared_ptr<DriveProcessor> driveProcessor;
    messageFunc sendZeroMessage;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    WheelID wheel = WHEEL_FR;

    // helper functions
    void triggerTosetWheelAngleVelocity(int triggerVal, float velocity,
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
    void buttonZeroMessage(uint8_t buttonID) { sendZeroMessage(1); }
};
