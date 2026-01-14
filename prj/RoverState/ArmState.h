#pragma once

#include <cstdint>

struct ArmState {
    float x;
    float y;
    float z;
    int pitch;
    int roll;
    int clawOpen;
};
