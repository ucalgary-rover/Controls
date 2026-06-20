#pragma once

#include "DriveAutoControllerLayout.h"
#include "DriveManualControllerLayout.h"
#include <SDL2/SDL.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

enum DriveLayout {
    DRIVE_AUTO,
    DRIVE_MANUAL
};

using ProcessDriveStateFunc = std::function<DriveMotorState(const DriveState&)>;

struct DriveControlState {
    DriveState driveState;
    DriveMotorState driveMotorState;
};

class DriveControllerLayout : public ControllerLayout {
public:
    DriveControllerLayout(ProcessDriveStateFunc processFunc) {
        drivelayouts[DRIVE_AUTO]
            = std::make_shared<DriveAutoControllerLayout>(driveStateManager);
        drivelayouts[DRIVE_MANUAL]
            = std::make_shared<DriveManualControllerLayout>(
                driveManualStateManager);

        process = processFunc;

        // Initialize button callbacks
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, swtichToDefault);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, swtichToManual);

        // clang-format on
    }

    DriveControlState getControlState(uint64_t elapsed_ms);

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
    DriveStateManager driveStateManager = {};
    DriveMotorStateManager driveManualStateManager = {};
    DriveMotorState desiredMotorState = {};

    ProcessDriveStateFunc process;

    DriveLayout currentLayout = DriveLayout::DRIVE_AUTO;
    std::shared_ptr<ControllerLayout> drivelayouts[3];
};
