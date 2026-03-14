#include "DriveModel.h"
#include <cmath>

static const float PI = 3.14;
#define TO_RADIANS(value) round(value * PI / 180 * 100) / 100
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

// Internal helpers ported from DriveHandler logic but pure (no hardware calls)
static float spotTurnSpeedInternal(int stickAngle) {
    if (stickAngle < 30 || stickAngle > 330) {
        return 0;
    } else if (stickAngle > 180) {
        return -1 + (abs(270 - (float)stickAngle) / 60);
    } else {
        return 1 - (abs(90 - (float)stickAngle) / 60);
    }
}

static int strafeAngleAdjustInternal(int stickTheta, float length, float width) {
    int reportedAngle = 0;

    if (stickTheta < 90) {
        reportedAngle = stickTheta;
    } else if (stickTheta > 270) {
        reportedAngle = stickTheta - 360;
    } else if (stickTheta > 90 && stickTheta < 270) {
        reportedAngle = stickTheta - 180;
    } else if (stickTheta == 90 || stickTheta == 270) {
        // edge positions - choose nearest average wheel orientation
        // For model simplicity return 90 or -90 based on stickTheta
        reportedAngle = (stickTheta == 270) ? -90 : 90;
    }

    return reportedAngle;
}

static float radialTurnHeadingAngleInternal(int stickAngle) {
    float reportedAngle = 0;
    const int RADIAL_ANGLE_MAX = 45;

    if (stickAngle < 90) {
        reportedAngle = (((float)stickAngle * RADIAL_ANGLE_MAX) / 90);
    } else if (stickAngle > 180) {
        reportedAngle = ((((float)stickAngle - 360) * RADIAL_ANGLE_MAX) / 90);
    }

    float headingAngle = TO_RADIANS(std::abs(reportedAngle));
    return headingAngle;
}

static float radialTurnWheelAnglesInternal(float headingAngle, bool innerFlag,
                                           float length, float width) {
    int wheelInt = (innerFlag) ? 1 : -1;

    float wheelAngle = atan(2 * length * sin(headingAngle)
                           / (2 * length * cos(headingAngle)
                              - wheelInt * width * sin(headingAngle)));

    return TO_DEGREES(wheelAngle);
}

DriveMotorState DriveModel::computeMotorState(const WheelMessage& message,
                                              float length, float width) {
    DriveMotorState out;
    // default zero
    for (int i = 0; i < WHEEL_COUNT; i++) {
        out.steer[i] = 0;
        out.drive[i] = 0;
    }

    bool angleVelocityFlag = message.angleVelocity > 0;
    bool velocityFlag = message.velocity != 0;

    bool longitudinalVelocityOnly = (message.theta == 0) || (message.theta == 180);
    bool lateralVelocityOnly = (message.theta == 90) || (message.theta == 270);

    if (!velocityFlag && !angleVelocityFlag) {
        return out; // all zeros
    }

    // Spot turn: angular velocity only
    if (angleVelocityFlag && !velocityFlag) {
        float speedF = spotTurnSpeedInternal(message.angleVelocity);
        int speedPercent = (int)round(speedF * 100.0f);

        // Wheel steering angles for spot turn (based on geometry)
        float wheelAngle = TO_DEGREES(atan(length / width));
        // Map to DriveIndex ordering: FL, FR, BL, BR
        out.steer[0] = - (int)round(wheelAngle); // FL
        out.steer[1] = (int)round(wheelAngle);   // FR
        out.steer[2] = (int)round(wheelAngle);   // BL
        out.steer[3] = - (int)round(wheelAngle); // BR

        // drive speeds (percent)
        for (int i = 0; i < WHEEL_COUNT; i++) {
            out.drive[i] = -speedPercent; // match original sign convention
        }

        return out;
    }

    // Radial turn (heading change while moving longitudinally)
    if (angleVelocityFlag && longitudinalVelocityOnly && velocityFlag) {
        float headingAngle = radialTurnHeadingAngleInternal(message.angleVelocity);
        float innerAngle = radialTurnWheelAnglesInternal(headingAngle, true, length, width);
        float outerAngle = radialTurnWheelAnglesInternal(headingAngle, false, length, width);

        float leftAngleF;
        float rightAngleF;

        // Determine direction from angleVelocity sign (positive -> left)
        int lastRadialOutput = (message.angleVelocity > 0) ? 1 : -1;
        if (lastRadialOutput > 0) {
            leftAngleF = innerAngle;
            rightAngleF = outerAngle;
        } else {
            leftAngleF = -outerAngle;
            rightAngleF = -innerAngle;
        }

        out.steer[0] = (int)round(leftAngleF);  // FL
        out.steer[1] = (int)round(rightAngleF); // FR
        out.steer[2] = 0;                       // BL straight
        out.steer[3] = 0;                       // BR straight

        int direction = (message.theta <= 90 || message.theta >= 270) ? 1 : -1;
        int speedPercent = (int)round((float)message.velocity * direction);
        for (int i = 0; i < WHEEL_COUNT; i++) out.drive[i] = (i % 2 == 0) ? speedPercent : -speedPercent;

        return out;
    }

    // Strafing (angle and velocity)
    if (velocityFlag && (longitudinalVelocityOnly || lateralVelocityOnly) && !angleVelocityFlag) {
        int wheelAngle = strafeAngleAdjustInternal(message.theta, length, width);

        int direction;
        if (message.theta == 90 || message.theta == 270) {
            int stickTheta = (message.theta == 270) ? -90 : 90;
            direction = (wheelAngle == stickTheta) ? 1 : -1;
        } else if (message.theta < 90 || message.theta > 270) {
            direction = 1;
        } else {
            direction = -1;
        }

        for (int stepper = 0; stepper < WHEEL_COUNT; stepper++) {
            out.steer[stepper] = wheelAngle;
        }

        int speedPercent = (int)round((float)message.velocity * direction);
        for (int dc = 0; dc < WHEEL_COUNT; dc++) {
            out.drive[dc] = (dc % 2 == 0) ? speedPercent : -speedPercent;
        }

        return out;
    }

    // Fallback: stop
    return out;
}
