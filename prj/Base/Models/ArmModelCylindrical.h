#ifndef ARM_MODEL_CYLINDRICAL_H
#define ARM_MODEL_CYLINDRICAL_H

#pragma once

#include "ArmMotorState.h"
#include "ArmStateCylindrical.h"

class ArmModelCylindrical {
public:
    static void initialize();

    static bool forwardsKinematics(const ArmMotorState& motorState,
                                   ArmStateCylindrical& out);
    static bool inverseKinematics(const ArmStateCylindrical& armState,
                                  ArmMotorState& out);

    static bool setMin(MotorID joint, int value);
    static bool setMax(MotorID joint, int value);

    static void printLimits();

private:
    static inline bool is_initialized = false;
};

#endif