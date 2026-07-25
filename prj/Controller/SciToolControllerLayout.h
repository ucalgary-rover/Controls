#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

using sciToolMessageFunc = std::function<void(MessageFormat, int)>;

class SciToolControllerLayout : public ControllerLayout {
public:
    SciToolControllerLayout(sciToolMessageFunc sendSciToolMessage) :
        ControllerLayout("SciToolController") {
        this->sendSciToolMessage = sendSciToolMessage;

        // Initialize Layout API
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, toogleDoor1);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, toogleDoor2);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, toogleDoor3);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, toggleBrush);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, decrementServoAngleOneStep);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, incrementServoAngleOneStep);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;
    int servoAngle = 0;
    bool brushEnabled = false;

    //message functions
    sciToolMessageFunc sendSciToolMessage;

    //helper function
    void incrementServoAngle(int increment);

    //button callbacks
    void toogleDoor1(uint8_t buttonID) {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_DOOR, LEFT);
    };
    void toogleDoor2(uint8_t buttonID) {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_DOOR, MIDDLE);
    };
    void toogleDoor3(uint8_t buttonID) {
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_DOOR, RIGHT);
    };
    void toggleBrush(uint8_t buttonID) {
        brushEnabled = !brushEnabled;
        sendSciToolMessage(MESSAGE_FORMAT_SCI_TOOL_BRUSH,
                           brushEnabled ? START_BRUSH : STOP_BRUSH);
    };
    void decrementServoAngleOneStep(uint8_t buttonID) {
        incrementServoAngle(-5);
    };
    void incrementServoAngleOneStep(uint8_t buttonID) {
        incrementServoAngle(5);
    };
};
