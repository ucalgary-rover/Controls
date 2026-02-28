#include "DriveModel.h"
#include "DriveMotorState.h"
#include <cmath>

const char* file = "DriveHandler";

constexpr float PI = 3.14159f;

#define TO_RADIANS(value) ((value) * PI / 180.0f)
#define TO_DEGREES(value) ((value) * 180.0f / PI)

// is maximum wheel speed for a spot turn
static constexpr float RADIAL_SPEED_MAX = 1.0f;

// is the maximum speed for strafing
static constexpr float STRAFE_SPEED_MAX = 1.0f;

// maximum turning angle for radial turning
static const int RADIAL_ANGLE_MAX = 45;

DriveMotorState DriveModel::convert(const DriveState& state, double length,
                                    double width) {
    DriveMotorState ms = {};

    bool angleVelocityFlag = state.angularVelocity > 0;
    bool velocityFlag = state.speed != 0;

    bool longitudinalOnly = (state.heading == 0 || state.heading == 180);
    bool lateralOnly = (state.heading == 90 || state.heading == 270);

    // STOP
    if (!velocityFlag && !angleVelocityFlag) {
        return ms;
    }

    // =====================
    // RADIAL TURN
    // =====================
    if (angleVelocityFlag && longitudinalOnly && velocityFlag) {

        float headingAngle = radialTurnHeadingAngle(state.angularVelocity);

        float innerAngle
            = radialTurnWheelAngle(headingAngle, length, width, true);

        float outerAngle
            = radialTurnWheelAngle(headingAngle, length, width, false);

        float leftAngle;
        float rightAngle;

        if (state.angularVelocity < 90) { // left turn
            leftAngle = innerAngle;
            rightAngle = outerAngle;
        } else { // right turn
            leftAngle = -outerAngle;
            rightAngle = -innerAngle;
        }

        ms.steer[0] = leftAngle;
        ms.steer[1] = rightAngle;
        ms.steer[2] = 0;
        ms.steer[3] = 0;

        int direction = (state.heading <= 90 || state.heading >= 270) ? 1 : -1;
        float speed
            = (float)state.speed / 100.0f * direction * RADIAL_SPEED_MAX;

        for (int i = 0; i < 4; i++) {
            ms.drive[i] = (i % 2 == 0) ? speed : -speed;
        }

        return ms;
    }

    // =====================
    // SPOT TURN
    // =====================
    if (angleVelocityFlag && !velocityFlag) {

        float speed = spotTurnSpeed(state.angularVelocity);

        float wheelAngle = TO_DEGREES(atan(length / width));

        ms.steer[0] = -wheelAngle;
        ms.steer[1] = wheelAngle;
        ms.steer[2] = wheelAngle;
        ms.steer[3] = -wheelAngle;

        for (int i = 0; i < 4; i++) {
            ms.drive[i] = -speed;
        }

        return ms;
    }

    // =====================
    // STRAFE
    // =====================
    if (velocityFlag && (longitudinalOnly || lateralOnly)
        && !angleVelocityFlag) {

        int wheelAngle = strafeAngleAdjust(state.heading);

        int direction = (state.heading < 90 || state.heading > 270) ? 1 : -1;

        float speed
            = (float)state.speed / 100.0f * direction * STRAFE_SPEED_MAX;

        for (int i = 0; i < 4; i++) {
            ms.steer[i] = wheelAngle;
            ms.drive[i] = (i % 2 == 0) ? speed : -speed;
        }

        return ms;
    }

    return ms;
}

float DriveModel::spotTurnSpeed(int stickAngle) {
    if (stickAngle < 30 || stickAngle > 330)
        return 0;
    if (stickAngle > 180)
        return -1 + (std::abs(270 - stickAngle) / 60.0f);
    return 1 - (std::abs(90 - stickAngle) / 60.0f);
}

int DriveModel::strafeAngleAdjust(int stickTheta) {
    if (stickTheta < 90)
        return stickTheta;
    if (stickTheta > 270)
        return stickTheta - 360;
    if (stickTheta > 90 && stickTheta < 270)
        return stickTheta - 180;
    return 90; // simplified edge case
}

float DriveModel::radialTurnHeadingAngle(int stickAngle) {
    float reportedAngle = 0.0f;

    if (stickAngle < 90) {
        reportedAngle = (stickAngle * RADIAL_ANGLE_MAX) / 90.0f;
    } else if (stickAngle > 180) {
        reportedAngle = ((stickAngle - 360.0f) * RADIAL_ANGLE_MAX) / 90.0f;
    }

    return TO_RADIANS(std::abs(reportedAngle));
}

float DriveModel::radialTurnWheelAngle(float headingAngle, double length,
                                       double width, bool inner) {
    int wheelInt = inner ? 1 : -1;

    float angle = atan(2 * length * sin(headingAngle)
                       / (2 * length * cos(headingAngle)
                          - wheelInt * width * sin(headingAngle)));

    return TO_DEGREES(angle);
}