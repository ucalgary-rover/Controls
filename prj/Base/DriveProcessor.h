/**
 * @file DriveProcessor.h
 * @brief Drive Processor handles thread synchronized modifications to the DriveState including manual to auto transitions.
 *
 * @details Similar to the ArmProcessor this class keeps the only absolute copy of the DriveState in which controller threads
 * can only modify via explicit calls to DriveProcessor methods which will maintain threadsafe state management.
 */

#pragma once

#include <chrono>
#include <memory>
#include <mutex>

#include "DriveMotorState.h"
#include "DriveMotorStateManager.h"
#include "DriveState.h"

struct DriveProcessorState {
    DriveState driverState;
    DriveMotorState motorState;
};

enum class DriveProcessorMode {
    DriverSpace,
    MotorSpace,
};

class DriveProcessor {
public:
    DriveProcessor(const DriveState& defaultDriveState);

    DriveProcessor(const DriveMotorState& defaultDriveMotorState);

    void setSpeed(int speed);
    void setHeading(int heading);
    void setAngularVelocity(int angularVelocity);

    void setSteerPosition(WheelID wheel, float position);
    void incrementSteerPosition(WheelID wheel, float increment);
    void setSteerVelocity(WheelID wheel, float velocity);

    DriveState getDriverState();
    DriveMotorState getDriveMotorState();
    DriveProcessorState getState();

    void setRoverState(const DriveMotorState& driveMotorState);
    DriveMotorState getRoverState();

    static DriveMotorState
    processDriveState(const DriveState& driveState,
                      const DriveMotorState& currentMotorState);

private:
    std::mutex mtx;
    DriveProcessorMode mode;
    bool changesMade = false;

    DriveProcessorState state = {};
    DriveMotorState motorVelocity = {};

    DriveMotorState roverMotorState = {};

    std::chrono::steady_clock::time_point update_timestamp
        = std::chrono::steady_clock::now();

    void setMode(DriveProcessorMode newMode);

    void handleChanges();
};