#include "DriveModel.h"
#include "DriveMotorState.h"
#include <cmath>
#include "pub_general.h"

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

double DriveModel::roverLength = 0;
double DriveModel::roverWidth = 0;

void DriveModel::initialize() {
    roverLength = 1; // TODO: get actual length & width of rover from config
    roverWidth = 1;
}

DriveMotorState DriveModel::process(const DriveState& state) {
    DriveMotorState ms = {};

    bool hasAngularVelocity = state.angularVelocity > 0;
    bool hasLinearVelocity = state.speed != 0;

    bool longitudinalOnly = (state.heading == 0 || state.heading == 180);
    bool lateralOnly = (state.heading == 90 || state.heading == 270);

    // STOP
    if (!hasLinearVelocity && !hasAngularVelocity) {
        return ms;
    }

    // =====================
    // RADIAL TURN
    // =====================
    if (hasAngularVelocity && longitudinalOnly && hasLinearVelocity) {

        float headingAngle = radialTurnHeadingAngle(state.angularVelocity);

        float innerAngle = radialTurnWheelAngle(headingAngle, true);

        float outerAngle = radialTurnWheelAngle(headingAngle, false);

        float leftAngle;
        float rightAngle;

        if (state.angularVelocity < 90) { // left turn
            leftAngle = innerAngle;
            rightAngle = outerAngle;
        } else { // right turn
            leftAngle = -outerAngle;
            rightAngle = -innerAngle;
        }

        ms.steer[WHEEL_FL] = leftAngle;
        ms.steer[WHEEL_FR] = rightAngle;
        ms.steer[WHEEL_RL] = 0;
        ms.steer[WHEEL_RR] = 0;

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
    if (hasAngularVelocity && !hasLinearVelocity) {

        float speed = spotTurnSpeed(state.angularVelocity);

        float wheelAngle = TO_DEGREES(atan(roverLength / roverWidth));

        ms.steer[WHEEL_FL] = -wheelAngle;
        ms.steer[WHEEL_FR] = wheelAngle;
        ms.steer[WHEEL_RL] = wheelAngle;
        ms.steer[WHEEL_RR] = -wheelAngle;

        for (int i = 0; i < 4; i++) {
            ms.drive[i] = -speed;
        }

        return ms;
    }

    // =====================
    // STRAFE
    // =====================
    if (hasLinearVelocity && (longitudinalOnly || lateralOnly)
        && !hasAngularVelocity) {

        int wheelAngle = strafeAngleAdjust(state.heading);

        int direction = (state.heading < 90 || state.heading > 270) ? 1 : -1;

        float speed
            = (float)state.speed / 100.0f * direction * STRAFE_SPEED_MAX;

        for (int i = 0; i < WHEEL_COUNT; i++) {
            ms.steer[i] = wheelAngle;
            ms.drive[i] = (i % 2 == 0) ? speed : -speed;
        }

        return ms;
    }

    return ms;
}

float DriveModel::spotTurnSpeed(int angularVelocity) {
    if (angularVelocity < 30 || angularVelocity > 330)
        return 0;
    if (angularVelocity > 180)
        return -1 + (std::abs(270 - angularVelocity) / 60.0f);
    return 1 - (std::abs(90 - angularVelocity) / 60.0f);
}

int DriveModel::strafeAngleAdjust(int heading) {
    if (heading < 90)
        return heading;
    if (heading > 270)
        return heading - 360;
    if (heading > 90 && heading < 270)
        return heading - 180;
    return 90; // simplified edge case
}

float DriveModel::radialTurnHeadingAngle(int angularVelocity) {
    float reportedAngle = 0.0f;

    if (angularVelocity < 90) {
        reportedAngle = (angularVelocity * RADIAL_ANGLE_MAX) / 90.0f;
    } else if (angularVelocity > 180) {
        reportedAngle = ((angularVelocity - 360.0f) * RADIAL_ANGLE_MAX) / 90.0f;
    }

    return TO_RADIANS(std::abs(reportedAngle));
}

float DriveModel::radialTurnWheelAngle(float headingAngle, bool isInnerWheel) {
    int wheelInt = isInnerWheel ? 1 : -1;

    float angle = atan(2 * roverLength * sin(headingAngle)
                       / (2 * roverLength * cos(headingAngle)
                          - wheelInt * roverWidth * sin(headingAngle)));

    return TO_DEGREES(angle);
}