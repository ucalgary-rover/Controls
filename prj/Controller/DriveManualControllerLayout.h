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
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, setZero);

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

    WheelID wheel = DRIVE_INDEX_FRONT_RIGHT;

    // helper functions
    void triggerTosetWheelAngleVelocity(int triggerVal, float velocity,
                                        int* lastTriggerValue);

    void incrementWheel(int change);

    void incrementWheelAngle(float increment);

    // Button Callbacks
    void setZero(uint8_t buttonID) {
        for (int wheel = 0; wheel < DRIVE_INDEX_WHEEL_COUNT; wheel++) {
            driveProcessor->setSteerPosition(static_cast<WheelID>(wheel), 0.0);
        }
        sendZeroMessage(1);
    }
    void decrementWheelAngleOnce(uint8_t buttonID) { incrementWheelAngle(-1); }
    void incrementWheelAngleOnce(uint8_t buttonID) { incrementWheelAngle(1); }
    void decrementWheelOnce(uint8_t buttonID) { incrementWheel(-1); }
    void incrementWheelOnce(uint8_t buttonID) { incrementWheel(1); }
};
