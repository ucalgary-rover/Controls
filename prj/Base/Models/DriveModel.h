#pragma once
#include "DriveMotorState.h"
#include "DriveState.h"

class DriveModel {
public:
    static void initialize();
    static DriveMotorState process(const DriveState& state);

private:
    static double roverLength;
    static double roverWidth;
    static float spotTurnSpeed(int angularVelocity);
    static int strafeAngleAdjust(int heading);
    static float radialTurnHeadingAngle(int angularVelocity);
    static float radialTurnWheelAngle(float headingAngle, bool inner);
};
