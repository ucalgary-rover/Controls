#ifndef DRIVE_MODEL_H
#define DRIVE_MODEL_H

#include "../RoverState/DriveMotorState.h"
#include "../../pub_general.h"

class DriveModel {
public:
    static DriveMotorState computeMotorState(const WheelMessage& message,
                                             float length, float width);
};

#endif
