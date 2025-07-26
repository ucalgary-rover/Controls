#include "ArmModel.h"

#include "../../../tools/IK/include/SSRTArm2025Analytical.h"
#include "../../../tools/IK/include/SSRTArm2025JointLimits.h"

static SSRTArm2025Analytical armModel;
bool ArmModel::is_initialized = false;

std::array<double, 3> vector3d2array(Eigen::Vector3d vector) {
    std::array<double, 3> array = {};
    for (int i = 0; i < vector.SizeAtCompileTime; i++) {
        array[i] = vector(i);
    }

    return array;
}

void ArmModel::initialize() {
    if (is_initialized) {
        return;
    }

    SSRTArm2025::LinkArray link_lengths
        = { 0.6, 0.4, 0.2 }; // TODO: determine actual values

    SSRTArm2025::JointArray joint_defaults = {};
    for (int i = 0; i < joint_defaults.size(); i++) {
        joint_defaults[i] = jointConfigs[i].origin;
    }

    SSRTArm2025Analytical::JointLimits joint_limits = {};
    joint_limits.push_back(checkIndividualJointBoundaries);

    armModel
        = SSRTArm2025Analytical(link_lengths, joint_defaults, joint_limits);

    is_initialized = true;
}

std::vector<std::array<double, 3>> ArmModel::getPositions() {
    if (!is_initialized) {
        return {};
    }

    std::vector<std::array<double, 3>> positions = {};
    for (auto& position : armModel.getPositions()) {
        positions.push_back(vector3d2array(position));
    }

    return positions;
}

std::vector<std::array<double, 3>> ArmModel::getOrientations() {
    if (!is_initialized) {
        return {};
    }

    std::vector<std::array<double, 3>> orientations = {};
    for (auto& orientation : armModel.getOrientations()) {
        orientations.push_back(vector3d2array(orientation));
    }

    return orientations;
}

std::array<double, 6> ArmModel::getAngles() {
    if (!is_initialized) {
        return {};
    }

    return armModel.GetAngles();
}

bool ArmModel::updateJointAngles(std::array<int, 6> new_angles) {
    if (!is_initialized) {
        return false;
    }

    std::array<double, 6> doubleArray = {};
    for (int i = 0; i < new_angles.size(); i++) {
        doubleArray[i] = new_angles[i];
    }

    return armModel.UpdateJointAngles(doubleArray);
}

std::array<double, 6>
ArmModel::generateWristPosition(std::array<double, 3> desired_wrist_position) {
    if (!is_initialized) {
        return {};
    }

    Eigen::Vector3d vector = {};

    for (int i = 0; i < desired_wrist_position.max_size(); i++) {
        vector(i) = desired_wrist_position[i];
    }

    return armModel.GenerateWristPosition(vector);
}

std::array<double, 6>
ArmModel::generateClawOrientation(std::array<int, 6> current_angles,
                                  double claw_pitch, double claw_roll) {
    if (!is_initialized) {
        return {};
    }

    std::array<double, 6> doubleArray = {};
    for (int i = 0; i < current_angles.size(); i++) {
        doubleArray[i] = current_angles[i];
    }

    return armModel.GenerateClawOrientation(doubleArray, claw_pitch, claw_roll);
}