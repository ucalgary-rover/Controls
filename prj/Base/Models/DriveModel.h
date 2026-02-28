#pragma once
#include "DriveMotorState.h"
#include "DriveState.h"

class DriveModel {
public:
    static DriveMotorState convert(const DriveState& state, double roverLength,
                                   double roverWidth);

private:
    static float spotTurnSpeed(int stickAngle);
    static int strafeAngleAdjust(int stickTheta);
    static float radialTurnHeadingAngle(int stickAngle);
    static float radialTurnWheelAngle(float headingAngle, double length,
                                      double width, bool inner);
};
