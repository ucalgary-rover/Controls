#pragma once

#include "ArmMotorState.h"
#include "DriveMotorState.h"

struct MotorState {
    DriveMotorState driveMotorState;
    ArmMotorState armMotorState;
};
