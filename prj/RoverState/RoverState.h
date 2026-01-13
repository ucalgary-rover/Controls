#pragma once

#include "ArmState.h"
#include "DriveState.h"

struct RoverState {
    DriveState driveState;
    ArmState armState;
};
