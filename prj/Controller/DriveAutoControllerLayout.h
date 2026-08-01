#pragma once

#include <memory>

#include "ControllerLayout.h"
#include "DriveProcessor.h"

class DriveAutoControllerLayout : public ControllerLayout {
public:
    DriveAutoControllerLayout(std::shared_ptr<DriveProcessor> driveProcessor,
                              messageFunc sendHeadlightsMessage,
                              messageFunc sendCameraServoMessage) :
        ControllerLayout("DriveController") {

        this->driveProcessor = driveProcessor;
        this->sendHeadlightsMessage = sendHeadlightsMessage;
        this->sendCameraServoMessage = sendCameraServoMessage;

        // clang-format off
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, decrementMaxSpeedOneStep );
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, incrementMaxSpeedOneStep );       
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementLightLevelOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementLightLevelOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, setLightLevelMax);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, setLightLevelMin);
        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    std::shared_ptr<DriveProcessor> driveProcessor;
    messageFunc sendHeadlightsMessage;
    messageFunc sendCameraServoMessage;

    int presentMaxSpeed = 80;  // present maximum speed of chassis
    int absoluteMaxSpeed = 80; // Absolute max speed of the chassis

    const int maxRadialSpeed = 45; // degrees per second

    uint8_t lightLevel = 0;

    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    //helper functions

    void checkState(uint8_t button);

    void setVelocity(int X, int Y);

    void setAngularVelocity(int X, int Y);

    void incrementMaxSpeed(int val);

    void incrementLightLevel(uint8_t val);

    //button callbacks
    void decrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(-2); };
    void incrementMaxSpeedOneStep(uint8_t buttonID) { incrementMaxSpeed(2); };

    void decrementLightLevelOneStep(uint8_t buttonID) {
        incrementLightLevel(-10);
    }
    void incrementLightLevelOneStep(uint8_t buttonID) {
        incrementLightLevel(10);
    }

    void setLightLevelMax(uint8_t buttonID) { incrementLightLevel(100); }

    void setLightLevelMin(uint8_t buttonID) { incrementLightLevel(-100); }
};
