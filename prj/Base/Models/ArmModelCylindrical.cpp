#include "ArmModelCylindrical.h"

#include "Logging.h"

struct Boundaries {
    ArmMotorState min;
    ArmMotorState max;
};

static const char* file = "ArmModel";

constexpr double pi = 3.14159265358979323846;

static constexpr double L1 = 0.275771164; // Shoulder to Elbow
static constexpr double L2 = 0.1206754;   // Elbow to Wrist

static Boundaries boundaries = {};

static bool inBounds(MotorID joint, int value) {
    return boundaries.min.motorValues[joint] <= value
           && boundaries.max.motorValues[joint] >= value;
}

void ArmModelCylindrical::initialize() {

    // Set Boundaries to max and min possible values at boot i.e. no limit
    for (int i = 0; i < MotorID::MOTOR_ID_END; i++) {
        boundaries.min.motorValues[i] = INT32_MIN;
        boundaries.max.motorValues[i] = INT32_MAX;
    }

    Logging::logI(file, "Initialized, Removing Joint Limits.");

    is_initialized = true;
}

bool ArmModelCylindrical::forwardsKinematics(const ArmMotorState& motorState,
                                             ArmStateCylindrical& out) {
    if (!is_initialized) {
        Logging::logI(file, "inverseKinematics: Uninitialized.");
        return false;
    }

    double theta2 = motorState.motorValues[MOTOR_ID_SHOULDER];
    double theta3 = motorState.motorValues[MOTOR_ID_ELBOW];

    out.theta = motorState.motorValues[MOTOR_ID_BASE];
    out.r = L1 * cos(theta2) + L2 * cos(theta3);
    out.z = L1 * sin(theta2) + L2 * sin(theta3);
    out.pitch = motorState.motorValues[MOTOR_ID_CLAW_PITCH];
    out.roll = motorState.motorValues[MOTOR_ID_CLAW_ROLL];
    out.clawOpen = motorState.motorValues[MOTOR_ID_CLAW_OPEN];

    return true;
}

bool ArmModelCylindrical::inverseKinematics(const ArmStateCylindrical& armState,
                                            ArmMotorState& out) {
    if (!is_initialized) {
        Logging::logI(file, "inverseKinematics: Uninitialized.");
        return false;
    }

    // Check Base & Claw Limits
    if (!inBounds(MOTOR_ID_BASE, armState.theta)) {
        Logging::logI(file, "inverseKinematics: Base out of bounds.");
        return false;
    }

    if (!inBounds(MOTOR_ID_CLAW_PITCH, armState.pitch)) {
        Logging::logI(file, "inverseKinematics: Claw Pitch Out of Bounds.");
        return false;
    }

    const double r = armState.r;
    const double z = armState.z;

    double cosTheta3 = (r * r + z * z - L1 * L1 - L2 * L2) / (2.0 * L1 * L2);

    if (cosTheta3 < -1.0 || cosTheta3 > 1.0) {
        Logging::logI(file, "inverseKinematics: Target Position out of Reach.");
        return false;
    }

    double theta3 = -acos(cosTheta3);
    double theta2
        = atan2(z, r) - atan2(L1 + L2 * cos(theta3), -L2 * sin(theta3));

    // Check Arm Limits
    if (!inBounds(MOTOR_ID_SHOULDER, theta2)) {
        Logging::logI(file, "inverseKinematics: Shoulder Angle Out of Bounds.");
        return false;
    }

    if (!inBounds(MOTOR_ID_ELBOW, theta3)) {
        Logging::logI(file, "inverseKinematics: Elbow Angle Out of Bounds.");
        return false;
    }

    out.motorValues[MOTOR_ID_BASE] = armState.theta;
    out.motorValues[MOTOR_ID_SHOULDER] = theta2 * 180.f / pi;
    out.motorValues[MOTOR_ID_ELBOW] = theta3 * 180.f / pi;
    out.motorValues[MOTOR_ID_WRIST] = 0; // Unused
    out.motorValues[MOTOR_ID_CLAW_PITCH] = armState.pitch;
    out.motorValues[MOTOR_ID_CLAW_ROLL] = armState.roll;
    out.motorValues[MOTOR_ID_CLAW_OPEN] = armState.clawOpen;

    auto& vals = out.motorValues;
    Logging::logI(file, "%d %d %d %d %d %d", vals[0], vals[1], vals[2], vals[3],
                  vals[4], vals[5]);

    return true;
}

bool ArmModelCylindrical::setMin(MotorID joint, int value) {
    int& min = boundaries.min.motorValues[joint];
    int& max = boundaries.max.motorValues[joint];

    if (value >= max) {
        return false;
    }

    min = value;

    Logging::logI(file, "Set Joint %d MIN to %d", joint, value);

    return true;
}

bool ArmModelCylindrical::setMax(MotorID joint, int value) {
    int& min = boundaries.min.motorValues[joint];
    int& max = boundaries.max.motorValues[joint];

    if (value <= min) {
        return false;
    }

    max = value;

    Logging::logI(file, "Set Joint %d MAX to %d", joint, value);

    return true;
}

void ArmModelCylindrical::printLimits() {
    for (int i = 0; i < MOTOR_ID_END; i++) {
        Logging::logI(file, "Joint : %d : Min: %d : Max %d", i,
                      boundaries.min.motorValues[i],
                      boundaries.max.motorValues[i]);
    }
}
