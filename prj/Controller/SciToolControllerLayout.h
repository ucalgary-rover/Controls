#pragma once

#include "ArmStateManager.h"
#include "ControllerLayout.h"
#include "pub_general.h"
#include <functional>

class SciToolControllerLayout : public ControllerLayout {
public:
    SciToolControllerLayout() : ControllerLayout("SciToolController") {

        // Initialize Layout API
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, toogleDoor1);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, toogleDoor2);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, toogleDoor3);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, toggleBrush);

        // clang-format on
    }

    void buttonResponse(uint8_t buttonID) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

private:
    int lastleftTriggerValue = 0;
    int lastrightTriggerValue = 0;

    //helper functions
    void toggleDoor(int door);

    void lowerSciTool();
    void raiseSciTool();
    void stopSciTool();

    //button callbacks
    void toogleDoor1(uint8_t buttonID) { toggleDoor(1); }
    void toogleDoor2(uint8_t buttonID) { toggleDoor(2); }
    void toogleDoor3(uint8_t buttonID) { toggleDoor(3); }
    void toggleBrush(uint8_t buttonID);
};
