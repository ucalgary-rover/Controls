#pragma once
#include "DriveMotorState.h"
#include "DriveState.h"

#define ANGULAR_VELOCITY_FACTOR                                                \
    45 // TODO: Find mapping for angular speed (experimentally)

// Allow inaccuracy in sensor data to still be parsed correctly
#define DRIVE_THRESHOLD 0.1 // TODO: configure
#define STEER_THRESHOLD 1.0 // TODO: configure

// At which angle to desired the rover can start moving
#define ACCEPTABLE_ANGLE_FOR_MOTION 5.0 // TODO: configure

enum DriveStateType : int {
    DRIVE_STATE_TYPE_RADIAL_TURN,
    DRIVE_STATE_TYPE_SPOT_TURN,
    DRIVE_STATE_TYPE_STRAFE,
    DRIVE_STATE_TYPE_STOP,
    DRIVE_STATE_TYPE_UNDEFINED
};

class DriveModel {
public:
    static void initialize();
    static DriveMotorState process(const DriveState& state,
                                   const DriveMotorState& currentState);
    static DriveStateType getDriveStateType(DriveMotorState state);

private:
    static double roverLength;
    static double roverWidth;

    // Helper functions for back calculating state
    static bool allWheelsDrivingInSameDirection(DriveMotorState state);
    static bool stateIsStopped(DriveMotorState state);
    static bool stateIsRadialTurn(DriveMotorState state);
    static bool stateIsStrafe(DriveMotorState state);
    static bool stateIsSpotTurn(DriveMotorState state);

    static DriveMotorState
    calculateMotorState(const DriveState& state,
                        const DriveMotorState& currentMotorState);

    static float spotTurnSpeed(int angularVelocity);
    // Assuming heading is 0, 90, 180, 270 or 360
    static int strafeAngleAdjust(int heading, float currentHeading);
    static float radialTurnHeadingAngle(int angularVelocity);
    static float radialTurnWheelAngle(float headingAngle, bool inner);
};
