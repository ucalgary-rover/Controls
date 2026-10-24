#include "DriveModel.h"
#include "DriveMotorState.h"
#include "pub_general.h"
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

double DriveModel::roverLength = 0;
double DriveModel::roverWidth = 0;

void DriveModel::initialize() {
    roverLength = 1; // TODO: get actual length & width of rover from config
    roverWidth = 1;
}

static int diffBelowThreshold(float angle1, float angle2, float threshold) {
    return abs(angle1 - angle2) < threshold;
}

bool DriveModel::allWheelsDrivingInSameDirection(DriveMotorState state) {
    bool allWheelsDrivingInSameDirection = true;
    if (!stateIsStopped(state)) {
        if (state.drive[0]
            < 0.0) { // Should be jitter safe due to stateIsStopped check
            for (int i = 1; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
                if (state.drive[i] < 0.0) {
                    allWheelsDrivingInSameDirection = false;
                    break;
                }
            }
        } else if (state.drive[0] > 0.0) {
            for (int i = 1; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
                if (state.drive[i] > 0.0) {
                    allWheelsDrivingInSameDirection = false;
                    break;
                }
            }
        } else {
            return false;
        }
        return allWheelsDrivingInSameDirection;
    }
    return false;
}

bool DriveModel::stateIsStopped(DriveMotorState state) {
    bool stopped = true;
    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        if (abs(state.drive[i]) > DRIVE_THRESHOLD) {
            stopped = false;
            break;
        }
    }
    return stopped;
}

bool DriveModel::stateIsRadialTurn(DriveMotorState state) {

    if (!diffBelowThreshold(state.steer[DRIVE_INDEX_BACK_LEFT], 0,
                            STEER_THRESHOLD)
        && !diffBelowThreshold(state.steer[DRIVE_INDEX_BACK_LEFT], 360,
                               STEER_THRESHOLD)) {
        return false; //Badness angle
    }

    if (!diffBelowThreshold(state.steer[DRIVE_INDEX_BACK_RIGHT], 0,
                            STEER_THRESHOLD)
        && !diffBelowThreshold(state.steer[DRIVE_INDEX_BACK_RIGHT], 360,
                               STEER_THRESHOLD)) {
        return false; //Badness angle
    }

    bool rightWheelForwards = (state.steer[DRIVE_INDEX_FRONT_LEFT] < 45
                               && state.steer[DRIVE_INDEX_FRONT_LEFT] > 315);
    bool leftWheelForwards = (state.steer[DRIVE_INDEX_FRONT_RIGHT] < 45
                              && state.steer[DRIVE_INDEX_FRONT_RIGHT] > 315);
    if (rightWheelForwards && leftWheelForwards) {
        return true;
    }

    bool bothLessThan70 = (state.steer[DRIVE_INDEX_FRONT_LEFT] < 70
                           && state.steer[DRIVE_INDEX_FRONT_RIGHT] < 70);
    bool bothGreaterThan290 = (state.steer[DRIVE_INDEX_FRONT_LEFT] > 290
                               && state.steer[DRIVE_INDEX_FRONT_RIGHT] > 290);

    if (!bothLessThan70 && !bothGreaterThan290) {
        return false;
    }

    return true;
}

bool DriveModel::stateIsStrafe(DriveMotorState state) {
    // Check angles
    if (!diffBelowThreshold(state.steer[0], 90.0, STEER_THRESHOLD)
        && !diffBelowThreshold(state.steer[0], 270.0, STEER_THRESHOLD)) {
        return false; //Badness angle
    }

    for (int i = 1; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        if (!diffBelowThreshold(state.steer[i], state.steer[0],
                                STEER_THRESHOLD)) {
            return false;
        }
    }

    return true;
}

bool DriveModel::stateIsSpotTurn(DriveMotorState state) {
    float angles[] = { 315, 45, 45, 315 };

    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        if (!diffBelowThreshold(state.steer[i], angles[i], STEER_THRESHOLD)) {
            return false;
        }
    }

    return true;
}

DriveMotorState DriveModel::process(const DriveState& state,
                                    const DriveMotorState& currentMotorState) {

    static DriveStateType currentType;
    DriveStateType desiredDriveStateType;

    DriveMotorState desiredDriveMotorState
        = calculateMotorState(state, currentMotorState, &desiredDriveStateType);

    DriveMotorState stoppedMotorState;
    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        stoppedMotorState.steer[i]
            = desiredDriveMotorState.steer[i]; // Allow angle change
        stoppedMotorState.drive[i] = 0;        // Stop drive
    }

    currentType = DriveModel::getDriveStateType(currentMotorState);
    bool desiredIsSpotTurning
        = desiredDriveStateType == DRIVE_STATE_TYPE_SPOT_TURN;
    bool currentIsSpotTurning = currentType == DRIVE_STATE_TYPE_SPOT_TURN;

    if (currentType == DRIVE_STATE_TYPE_UNDEFINED) {
        return stoppedMotorState;
    }

    if (desiredIsSpotTurning != currentIsSpotTurning) {
        return stoppedMotorState;
    }

    if (desiredDriveStateType == DRIVE_STATE_TYPE_RADIAL_TURN
        && currentType == DRIVE_STATE_TYPE_RADIAL_TURN) {
        return desiredDriveMotorState;
    }

    // Inter-state halting conditions
    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        // Check for drive direction reversal
        if (desiredDriveMotorState.drive[i] * currentMotorState.drive[i]
            < 0.0) { // Opposite signs and therefore negative
            Logging::logE(file, "------------>Opposites");

            return stoppedMotorState;
        }
        // Check for acceptable wheel angle difference
        if (abs(desiredDriveMotorState.steer[i] - currentMotorState.steer[i])
            > ACCEPTABLE_ANGLE_FOR_MOTION) {
            Logging::logE(file, "------------>Angle");

            return stoppedMotorState;
        }
    }
    return desiredDriveMotorState;
}

static bool isMoveForwards(int heading) {
    return (heading <= 90 || heading >= 270);
}

static bool isMoveBackwards(int heading) {
    return (heading > 90 && heading < 270);
}

static bool isStrafeLeft(int heading) {
    return (heading > 45 && heading < 135);
}

static bool isStrafeRight(int heading) {
    return (heading > 225 && heading < 315);
}

static bool inLeftHemisphere(int heading) { return (heading <= 180); }

static int calculateCurrentHeading(const DriveMotorState& currentMotorState) {
    int currentHeading = 0;
    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        currentHeading += currentMotorState.steer[i];
    }

    currentHeading = currentHeading / DRIVE_INDEX_WHEEL_COUNT;

    return currentHeading;
}

DriveMotorState
DriveModel::calculateMotorState(const DriveState& state,
                                const DriveMotorState& currentMotorState,
                                DriveStateType* desiredDriveStateType) {
    static DriveMotorState ms = {};

    bool hasAngularVelocity = state.angularVelocity != 0;
    bool hasLinearVelocity = state.speed != 0;

    bool longitudinalOnly = (state.heading == 0 || state.heading == 180);
    bool lateralOnly = (state.heading == 90 || state.heading == 270);

    // STOP
    if (!hasLinearVelocity && !hasAngularVelocity) {
        // zero speed
        for (int i = 0; i < 4; i++) {
            ms.drive[i] = 0;
        }
        return ms;
    }

    bool strafeLeft = isStrafeLeft(state.heading);
    bool strafeRight = isStrafeRight(state.heading);

    bool strafing = strafeLeft || strafeRight;

    // =====================
    // RADIAL TURN
    // =====================
    if (hasLinearVelocity && (hasAngularVelocity || !strafing)) {

        float headingAngle = radialTurnHeadingAngle(state.angularVelocity);

        bool leftInner = state.angularVelocity >= 0;

        ms.steer[DRIVE_INDEX_FRONT_LEFT]
            = radialTurnWheelAngle(headingAngle, leftInner);
        ms.steer[DRIVE_INDEX_FRONT_RIGHT]
            = radialTurnWheelAngle(headingAngle, !leftInner);
        ms.steer[DRIVE_INDEX_BACK_LEFT] = 0;
        ms.steer[DRIVE_INDEX_BACK_RIGHT] = 0;

        for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
            if (!leftInner) {
                ms.steer[i] = 360 - ms.steer[i];
            }
        }

        *desiredDriveStateType = DRIVE_STATE_TYPE_RADIAL_TURN;

        Logging::logV(file, "FL: %f FR: %f", ms.steer[DRIVE_INDEX_FRONT_LEFT],
                      ms.steer[DRIVE_INDEX_FRONT_RIGHT]);

        int direction = isMoveForwards(state.heading) ? 1 : -1;
        float speed
            = (float)state.speed * direction * RADIAL_SPEED_MAX / 100.0f;

        for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
            ms.drive[i] = speed;
        }

        return ms;
    }

    // =====================
    // SPOT TURN
    // =====================
    if (hasAngularVelocity && !hasLinearVelocity) {

        float speed = spotTurnSpeed(state.angularVelocity);

        float wheelAngle = TO_DEGREES(atan(roverLength / roverWidth));

        ms.steer[DRIVE_INDEX_FRONT_LEFT] = 360 - wheelAngle;
        ms.steer[DRIVE_INDEX_FRONT_RIGHT] = wheelAngle;
        ms.steer[DRIVE_INDEX_BACK_LEFT] = wheelAngle;
        ms.steer[DRIVE_INDEX_BACK_RIGHT] = 360 - wheelAngle;

        for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
            ms.drive[i] = (i % 2 == 0) ? -speed : speed;
        }
        *desiredDriveStateType = DRIVE_STATE_TYPE_SPOT_TURN;

        return ms;
    }

    // =====================
    // STRAFE
    // =====================
    if (!hasAngularVelocity && hasLinearVelocity) {
        float wheelAngle = 0;
        float direction = 1;

        int currentHeading = calculateCurrentHeading(currentMotorState);
        bool closerToLeft = inLeftHemisphere(currentHeading);
        if (strafeLeft) {
            if (closerToLeft) {
                // Set Speeds Positive
                wheelAngle = 90;
                direction = 1;
            } else {
                // Set Speeds Negative
                wheelAngle = 270;
                direction = -1;
            }
        } else {
            if (!closerToLeft) {
                // Set Speeds Positive
                wheelAngle = 270;
                direction = 1;
            } else {
                // Set Speeds Negative
                wheelAngle = 90;
                direction = -1;
            }
        }

        float speed
            = (float)state.speed / 100.0f * direction * STRAFE_SPEED_MAX;

        for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
            ms.steer[i] = wheelAngle;
            ms.drive[i] = speed;
        }

        *desiredDriveStateType = DRIVE_STATE_TYPE_STRAFE;
        return ms;
    }

    return ms;
}

DriveStateType DriveModel::getDriveStateType(DriveMotorState state) {
    bool cumulativeAngleError = false;
    for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
        if (state.steer[i] > 360.0 || state.steer[i] < 0.0) {
            cumulativeAngleError = true;
        }
    }

    if (cumulativeAngleError) {
        std::cerr << "Disallowed angle (smaller than 0 or larger than 360) "
                     "encountered. Returning undefined drive state.";
        return DRIVE_STATE_TYPE_UNDEFINED;
    } // From this point on, assume acceptable angles

    if (stateIsRadialTurn(state)) {
        return DRIVE_STATE_TYPE_RADIAL_TURN;
    }

    if (stateIsStrafe(state)) {
        return DRIVE_STATE_TYPE_STRAFE;
    }

    if (stateIsSpotTurn(state)) {
        return DRIVE_STATE_TYPE_SPOT_TURN;
    }

    return DRIVE_STATE_TYPE_UNDEFINED;
}

float DriveModel::spotTurnSpeed(int angularVelocity) {
    float speed = ((float)angularVelocity) / ANGULAR_VELOCITY_FACTOR;

    if (speed > 1.0) {
        return 1.0;
    } else if (speed < -1.0) {
        return -1.0;
    } else {
        return speed;
    }
}

int DriveModel::strafeAngleAdjust(int heading, float currentHeading) {

    if (heading == 360 || heading == 180 || heading == 0) {
        return 0;
    } else if (currentHeading >= 180) {
        return 270;
    } else {
        return 90;
    }
}

float DriveModel::radialTurnHeadingAngle(int angularVelocity) {
    float reportedAngle = angularVelocity; // Guys wtf how fo you do this

    return TO_RADIANS(std::abs(reportedAngle));
}

float DriveModel::radialTurnWheelAngle(float headingAngle, bool isInnerWheel) {
    int wheelInt = isInnerWheel ? 1 : -1;

    float angle = atan(2 * roverLength * sin(headingAngle)
                       / (2 * roverLength * cos(headingAngle)
                          - wheelInt * roverWidth * sin(headingAngle)));

    if (angle < 0) {
        angle += 2.0f * PI;
    }
    return TO_DEGREES(angle);
}