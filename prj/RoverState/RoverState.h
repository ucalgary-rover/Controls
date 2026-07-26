#pragma once

#include "ArmState.h"
#include "DriveState.h"

struct RoverState {
    DriveState driveState;
    ArmState armState;

    bool operator==(const RoverState& other) const {
        return (this->driveState == other.driveState)
               && (this->armState == other.armState);
    }

    bool operator!=(const RoverState& other) const { return !(*this == other); }
};
