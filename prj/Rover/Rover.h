#pragma once

#include "MotorStateManager.h"

class Rover {
public:
    static void initialize();

    static void start();

private:
    // Chassis state management
    static MotorStateManager desiredMotorStateManager;
    static MotorStateManager currentMotorStateManager;
};
