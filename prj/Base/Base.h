#ifndef BASE_H
#define BASE_H

#include "ArmControllerLayout.h"
#include "Base/ControllerHandler.h"
#include "Controller/ControllerLayout.h"
#include "DriveControllerLayout.h"
#include "MotorStateManager.h"
#include "UDPHandler.h"

#pragma once

class Base {
public:
    static void initialize();

    // Start the loops to have Base working
    static void start();

    // Exit the loops that base runs through
    static void quit();

private:
    // Chassis state management
    static MotorStateManager desiredMotorStateManager;
    static MotorStateManager currentMotorStateManager;

    // Variables for state of rover arm
    static ArmMessageType armControlType;

    static bool exitLoop;

    static std::shared_ptr<DriveControllerLayout> driveController;
    static std::shared_ptr<ArmControllerLayout> armController;

    // Desired Motor State Update Methods
    static DriveMotorState
    processDesiredDriveState(const DriveState& desiredDriveState);
    static ArmMotorState
    processDesiredArmState(const ArmState& desiredArmState);
    static ArmState armForwardsKinematics(const ArmMotorState& motorState);
    static void updateDesiredRoverState(uint64_t elapsed_ms);

    // UDP receiving
    static void receive(UDPHandler& receiver);
};
#endif