#pragma once

#include "pub_general.h"

struct DriveMotorState {
    int steer[WHEEL_COUNT];
    int drive[WHEEL_COUNT];
};
