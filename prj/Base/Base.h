#pragma once

#include "ArmControllerLayout.h"
#include "ArmProcessor.h"
#include "Base/ControllerHandler.h"
#include "Controller/ControllerLayout.h"
#include "DriveControllerLayout.h"
#include "DriveProcessor.h"
#include "MotorStateManager.h"
#include "SciToolControllerLayout.h"
#include "UDPHandler.h"

class Base {
public:
    static void initialize();

    // Start the loops to have Base working
    static void start();

    // Exit the loops that base runs through
    static void quit();

private:
    // Chassis state management
    static constexpr ArmMotorState defaultArmMotorState = {};
    static std::shared_ptr<ArmProcessor> armProcessor;

    static constexpr DriveMotorState defaultDriveMotorState = {};
    static std::shared_ptr<DriveProcessor> driveProcessor;

    static bool exitLoop;

    // UDP receiving
    static void receive(UDPHandler& receiver);
};
