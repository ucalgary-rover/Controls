#pragma once

#include "ArmMotorState.h"
#include "DriveMotorState.h"

struct MotorState {
    DriveMotorState driveMotorState;
    ArmMotorState armMotorState;

    bool operator==(const MotorState& other) const {
        return (this->driveMotorState == other.driveMotorState)
               && (this->armMotorState == other.armMotorState);
    }

    bool operator!=(const MotorState& other) const { return !(*this == other); }
};
