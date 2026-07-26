#pragma once

#include "DriveAutoControllerLayout.h"
#include "DriveManualControllerLayout.h"
#include "DriveProcessor.h"
#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include <string>

enum DriveLayout {
    DRIVE_AUTO,
    DRIVE_MANUAL
};

struct DriveControlState {
    DriveState driveState;
    DriveMotorState driveMotorState;
};

class DriveControllerLayout : public ControllerLayout {
public:
    DriveControllerLayout(std::shared_ptr<DriveProcessor> driveProcessor) {
        drivelayouts[DRIVE_AUTO]
            = std::make_shared<DriveAutoControllerLayout>(driveProcessor);
        drivelayouts[DRIVE_MANUAL]
            = std::make_shared<DriveManualControllerLayout>(driveProcessor);

        // Initialize button callbacks
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, swtichToDefault);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, swtichToManual);

        // clang-format on
    }

    std::string getName() { return drivelayouts[currentLayout]->getName(); }

    // button layout here (do the drivelayout switching)
    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

    //helper function
    void switchLayout(DriveLayout layout);

    //button callbacks
    void swtichToManual(uint8_t buttonID) { switchLayout(DRIVE_MANUAL); }
    void swtichToDefault(uint8_t buttonID) { switchLayout(DRIVE_AUTO); }

private:
    DriveLayout currentLayout = DriveLayout::DRIVE_AUTO;
    std::shared_ptr<ControllerLayout> drivelayouts[3];
};
