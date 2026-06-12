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

    DriveMotorState getDriveMotorState(uint64_t elapsed_ms) {
        desiredMotorState = process(driveStateManager.getState());
        return desiredMotorState;
    }

    //
    std::string getName() { return drivelayouts[currentLayout]->getName(); }

    // button layout here (do the drivelayout switching)
    void buttonResponse(uint8_t buttonID) override;

    void leftStickResponse(int xValue, int yValue) override;

    void rightStickResponse(int xValue, int yValue) override;

    void leftTriggerResponse(int16_t axisValue) override;

    void rightTriggerResponse(int16_t axisValue) override;

    //buton callbacks
    void swtichToManual(uint8_t buttonID) { currentLayout = DRIVE_MANUAL; }
    void swtichToDefault(uint8_t buttonID) { currentLayout = DRIVE_AUTO; }

private:
    DriveStateManager driveStateManager = {};
    DriveMotorStateManager driveManualStateManager = {};
    DriveMotorState desiredMotorState = {};

    ProcessDriveStateFunc process;

    DriveLayout currentLayout = DriveLayout::DRIVE_AUTO;
    std::shared_ptr<ControllerLayout> drivelayouts[3];
    std::function<void(uint8_t)> buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];
};
