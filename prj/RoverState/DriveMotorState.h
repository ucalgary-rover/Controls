#pragma once

#include "pub_general.h"

struct DriveMotorState {
    float steer[WHEEL_COUNT];
    float drive[WHEEL_COUNT];
};
