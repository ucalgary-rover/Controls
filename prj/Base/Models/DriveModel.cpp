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

bool DriveModel::allWheelsDrivingInSameDirection(DriveMotorState state) {
    bool allWheelsDrivingInSameDirection = true;
    if (!stateIsStopped(state)) {
        if (state.drive[0]
            < 0.0) { // Should be jitter safe due to stateIsStopped check
            for (int i = 1; i < WHEEL_COUNT; i++) {
                if (state.drive[i] < 0.0) {
                    allWheelsDrivingInSameDirection = false;
                    break;
                }
            }
        } else if (state.drive[0] > 0.0) {
            for (int i = 1; i < WHEEL_COUNT; i++) {
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
    for (int i = 0; i < WHEEL_COUNT; i++) {
        if (abs(state.drive[i]) > DRIVE_THRESHOLD) {
            stopped = false;
            break;
        }
    }
    return stopped;
}

bool DriveModel::stateIsRadialTurn(DriveMotorState state) {
    // Flags for readability
    bool backLeftWheelStraight
        = abs(state.steer[WHEEL_BL]) < STEER_THRESHOLD
          || abs(state.steer[WHEEL_BL] - 360.0) < STEER_THRESHOLD;
    bool backRightWheelStraight
        = abs(state.steer[WHEEL_BR]) < STEER_THRESHOLD
          || abs(state.steer[WHEEL_BR] - 360.0) < STEER_THRESHOLD;
    bool frontLeftWheelStraight
        = abs(state.steer[WHEEL_FL]) < STEER_THRESHOLD
          || abs(state.steer[WHEEL_FL] - 360.0) < STEER_THRESHOLD;
    bool frontRightWheelStraight
        = abs(state.steer[WHEEL_FR]) < STEER_THRESHOLD
          || abs(state.steer[WHEEL_FR] - 360.0) < STEER_THRESHOLD;

    bool backWheelsStraight = backLeftWheelStraight && backRightWheelStraight;
    bool frontWheelsStraight
        = frontLeftWheelStraight && frontRightWheelStraight;

    // Use flags to check state
    if (!stateIsStopped(state)) {
        if (frontWheelsStraight && !backWheelsStraight) {
            bool backLeftWheelAngleAcceptable
                = (abs(state.steer[WHEEL_BL]) < (45.0 + STEER_THRESHOLD)
                   || abs(state.steer[WHEEL_BL]) > (315.0 - STEER_THRESHOLD));
            bool backRightWheelAngleAcceptable
                = (abs(state.steer[WHEEL_BR]) < (45.0 + STEER_THRESHOLD)
                   || abs(state.steer[WHEEL_BR]) > (315.0 - STEER_THRESHOLD));
            if (!backLeftWheelAngleAcceptable
                || !backRightWheelAngleAcceptable) {
                return false;
            }
        } else if (backWheelsStraight && !frontWheelsStraight) {
            bool frontLeftWheelAngleAcceptable
                = (abs(state.steer[WHEEL_FL]) < (45.0 + STEER_THRESHOLD)
                   || abs(state.steer[WHEEL_FL]) > (315.0 - STEER_THRESHOLD));
            bool frontRightWheelAngleAcceptable
                = (abs(state.steer[WHEEL_FR]) < (45.0 + STEER_THRESHOLD)
                   || abs(state.steer[WHEEL_FR]) > (315.0 - STEER_THRESHOLD));
            if (!frontLeftWheelAngleAcceptable
                || !frontRightWheelAngleAcceptable) {
                return false;
            }
        }

        // Check that they are all driving in the same direction
        return allWheelsDrivingInSameDirection(state);
    }
    return false;
}

bool DriveModel::stateIsStrafe(DriveMotorState state) {
    if (!stateIsStopped(state)) {
        // Check angles
        if (abs(state.steer[0] - 90.0) > STEER_THRESHOLD
            || abs(state.steer[0] - 270.0) > STEER_THRESHOLD) {
            return false; // Already found an incorrect angle
        }

        bool allSameAngle = true;
        for (int i = 1; i < WHEEL_COUNT; i++) {
            if (abs(state.steer[i] - state.steer[0]) > STEER_THRESHOLD) {
                allSameAngle = false;
                break;
            }
        }

        // Check directions
        return allWheelsDrivingInSameDirection(state) && allSameAngle;
    }
    return false;
}

bool DriveModel::stateIsSpotTurn(DriveMotorState state) {
    if (!stateIsStopped(state)) {
        float spotTurnAngle = TO_DEGREES(atan(roverLength / roverWidth));
        bool allAnglesCorrect = true; // i.e., all angles at 45 degrees
        for (int i = 0; i < WHEEL_COUNT; i++) {
            if (abs(abs(state.steer[i]) - spotTurnAngle) > STEER_THRESHOLD) {
                allAnglesCorrect = false;
            }
        }

        bool allWheelsDrivingInCircle = true;
        if (state.drive[0]
            < 0.0) { // Should be jitter safe due to stateIsStopped check
            for (int i = 1; i < WHEEL_COUNT; i += 2) {
                if (i % 2
                    == 1) { // Odd, should be opposite direction to first wheel
                    if (state.drive[i] > 0.0) {
                        allWheelsDrivingInCircle = false;
                        break;
                    }
                } else { // Even, should be same direction as first wheel
                    if (state.drive[i] < 0.0) {
                        allWheelsDrivingInCircle = false;
                        break;
                    }
                }
            }
        } else {
            for (int i = 1; i < WHEEL_COUNT;
                 i += 2) { // Driving in opposite direction
                           // state.drive[0] > 0.0
                if (i % 2
                    == 1) { // Odd, should be opposite direction to first wheel
                    if (state.drive[i] < 0.0) {
                        allWheelsDrivingInCircle = false;
                        break;
                    }
                } else { // Even, should be same direction as first wheel
                    if (state.drive[i] > 0.0) {
                        allWheelsDrivingInCircle = false;
                        break;
                    }
                }
            }
        }
        return allAnglesCorrect && allWheelsDrivingInCircle;
    }
    return false;
}

DriveMotorState DriveModel::process(const DriveState& state,
                                    const DriveMotorState& currentMotorState) {

    DriveMotorState desiredDriveMotorState
        = calculateMotorState(state, currentMotorState);

    DriveMotorState stoppedMotorState;
    for (int i = 0; i < WHEEL_COUNT; i++) {
        stoppedMotorState.steer[i]
            = desiredDriveMotorState.steer[i]; // Allow angle change
        stoppedMotorState.drive[i] = 0;        // Stop drive
    }

    // Intra-state halting conditions
    DriveStateType desiredDriveStateType
        = DriveModel::getDriveStateType(desiredDriveMotorState);

    if (desiredDriveStateType
        == DriveModel::getDriveStateType(currentMotorState)) {
        if (desiredDriveStateType == DRIVE_STATE_TYPE_STRAFE) {
            if (abs(desiredDriveMotorState.steer[0]
                    - currentMotorState.steer[0])
                > STEER_THRESHOLD) { // getDriveStateType ensures same direction
                return stoppedMotorState;
            }
        }
    } else {
        // Inter-state halting conditions
        for (int i = 0; i < WHEEL_COUNT; i++) {
            // Check for drive direction reversal
            if (desiredDriveMotorState.drive[i] * currentMotorState.drive[i]
                < 0.0) { // Opposite signs and therefore negative
                return stoppedMotorState;
            }
            // Check for acceptable wheel angle difference
            if (abs(desiredDriveMotorState.steer[i]
                    - currentMotorState.steer[i])
                > ACCEPTABLE_ANGLE_FOR_MOTION) {
                return stoppedMotorState;
            }
        }
    }

    return desiredDriveMotorState;
}

DriveMotorState
DriveModel::calculateMotorState(const DriveState& state,
                                const DriveMotorState& currentMotorState) {
    DriveMotorState ms = {};

    bool hasAngularVelocity = state.angularVelocity != 0;
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
    if (longitudinalOnly && hasLinearVelocity) {

        float headingAngle = radialTurnHeadingAngle(state.angularVelocity);

        bool leftInner = state.angularVelocity >= 0;

        ms.steer[WHEEL_FL] = radialTurnWheelAngle(headingAngle, leftInner);
        ms.steer[WHEEL_FR] = radialTurnWheelAngle(headingAngle, !leftInner);
        ms.steer[WHEEL_BL] = 0;
        ms.steer[WHEEL_BR] = 0;

        int direction = (state.heading <= 90 || state.heading >= 270) ? 1 : -1;
        float speed
            = (float)state.speed * direction * RADIAL_SPEED_MAX / 100.0f;

        for (int i = 0; i < WHEEL_COUNT; i++) {
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

        ms.steer[WHEEL_FL] = 360 - wheelAngle;
        ms.steer[WHEEL_FR] = wheelAngle;
        ms.steer[WHEEL_BL] = wheelAngle;
        ms.steer[WHEEL_BR] = 360 - wheelAngle;

        for (int i = 0; i < WHEEL_COUNT; i++) {
            ms.drive[i] = (i % 2 == 0) ? speed : -speed;
        }

        return ms;
    }

    // =====================
    // STRAFE
    // =====================
    if (hasLinearVelocity && lateralOnly && !hasAngularVelocity) {
        float currentHeading = 0;

        for (int i = 0; i < WHEEL_COUNT; i++) {
            currentHeading += currentMotorState.steer[i];
        }

        currentHeading = currentHeading / WHEEL_COUNT;
        float wheelAngle
            = strafeAngleAdjust(state.angularVelocity, currentHeading);

        int direction;
        if (wheelAngle == 0) {
            direction = currentHeading == 180 ? -1 : 1;
        } else {
            direction = currentHeading == wheelAngle ? 1 : -1;
        }

        float speed
            = (float)state.speed / 100.0f * direction * STRAFE_SPEED_MAX;

        for (int i = 0; i < WHEEL_COUNT; i++) {
            ms.steer[i] = wheelAngle;
            ms.drive[i] = speed;
        }

        return ms;
    }

    return ms;
}

DriveStateType DriveModel::getDriveStateType(DriveMotorState state) {
    bool cumulativeAngleError = false;
    for (int i = 0; i < WHEEL_COUNT; i++) {
        if (state.steer[i] > 360.0 || state.steer[i] < 0.0) {
            cumulativeAngleError = true;
        }
    }

    if (cumulativeAngleError) {
        std::cerr << "Disallowed angle (smaller than 0 or larger than 360) "
                     "encountered. Returning undefined drive state.";
        return DRIVE_STATE_TYPE_UNDEFINED;
    } // From this point on, assume acceptable angles

    if (stateIsStopped(state)) {
        return DRIVE_STATE_TYPE_STOP;
    }

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
    if (angularVelocity < 0) {
        reportedAngle += 360;
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