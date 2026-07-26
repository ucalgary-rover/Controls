/**
 * @file ArmProcessor.cpp
 * @brief Arm Processor handles thread synchronized modifications to the ArmState as well as inverse and forwards kinematics.
 *
 * @details This class keeps to only absolute value of the ArmState on Base computing inverse and forwards kinematics whenever changes are made.
 * The ArmProcessor supports both incremental and velocity based changes to the Arm state. When switching between task and joint space control, 
 * the accumulated changes will be applied to synchronize task and joint space truths prior to swapping accumulation modes. This design assumes that
 * switching between inverse and forwards kinematics control will be infrequent as each time the mode is swapped the full IK algorithm must be executed.
 */

#include "ArmProcessor.h"

#include "Models/ArmModel.h"

#define MS_PER_SEC 1000.0

ArmProcessor::ArmProcessor(const ArmState& defaultArmState) {
    ArmModel::initialize();

    mode = ArmProcessorMode::TaskSpace;
    state.taskSpaceState = defaultArmState;
    state.jointSpaceState = armInverseKinematics(defaultArmState);
    update_timestamp = std::chrono::steady_clock::now();
}

ArmProcessor::ArmProcessor(const ArmMotorState& defaultArmMotorState) {
    ArmModel::initialize();

    mode = ArmProcessorMode::JointSpace;
    state.taskSpaceState = armForwardsKinematics(defaultArmMotorState);
    state.jointSpaceState = defaultArmMotorState;
    update_timestamp = std::chrono::steady_clock::now();
}

void ArmProcessor::setTaskSpaceState(const ArmState& armState) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(ArmProcessorMode::TaskSpace);

    // State Set overrides all modifiers
    state.taskSpaceState = armState;
    changesMade = true;
}

void ArmProcessor::incrementTaskSpaceState(const ArmState& delta) {
    std::lock_guard<std::mutex> lock(mtx);

    if (mode != ArmProcessorMode::TaskSpace) {
        handleChanges();
        setMode(ArmProcessorMode::TaskSpace);
    }

    // Accumulate task space changes
    state.taskSpaceState += delta;
    changesMade = true;
}

void ArmProcessor::setTaskSpaceVelocity(const ArmState& taskSpaceVelocity) {
    std::lock_guard<std::mutex> lock(mtx);

    if (mode != ArmProcessorMode::TaskSpace) {
        handleChanges();
        setMode(ArmProcessorMode::TaskSpace);
    }

    velocity.taskSpaceState = taskSpaceVelocity;
    changesMade = true;
}

void ArmProcessor::setJointSpaceState(const ArmMotorState& armMotorState) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(ArmProcessorMode::JointSpace);

    // State Set overrides all modifiers
    state.jointSpaceState = armMotorState;
    changesMade = false;
}

void ArmProcessor::incrementJointSpaceState(const ArmMotorState& delta) {
    std::lock_guard<std::mutex> lock(mtx);

    if (mode != ArmProcessorMode::JointSpace) {
        handleChanges();
        setMode(ArmProcessorMode::JointSpace);
    }

    // Accumulate task space changes
    state.jointSpaceState += delta;
    changesMade = true;
}

void ArmProcessor::setJointSpaceVelocity(
    const ArmMotorState& jointSpaceVelocity) {
    std::lock_guard<std::mutex> lock(mtx);

    if (mode != ArmProcessorMode::JointSpace) {
        handleChanges();
        setMode(ArmProcessorMode::JointSpace);
    }

    velocity.jointSpaceState = jointSpaceVelocity;
    changesMade = true;
}

ArmState ArmProcessor::getTaskSpaceState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state.taskSpaceState;
}

ArmMotorState ArmProcessor::getJointSpaceState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state.jointSpaceState;
}

ArmProcessorState ArmProcessor::getState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state;
}

void ArmProcessor::setRoverState(const ArmMotorState& armMotorState) {
    std::lock_guard<std::mutex> lock(mtx);

    roverMotorState = armMotorState;
}

ArmMotorState ArmProcessor::getRoverState() {
    std::lock_guard<std::mutex> lock(mtx);

    return roverMotorState;
}

ArmMotorState ArmProcessor::armInverseKinematics(const ArmState& armState) {
    ArmMotorState armMs = {};

    // Wrist position IK
    std::array<double, 3> desiredPos
        = { (double)armState.x, (double)armState.y, (double)armState.z };
    std::array<int, 6> angles = ArmModel::generateWristPosition(desiredPos);

    // Apply claw orientation
    double pitch = (double)armState.pitch;
    double roll = (double)armState.roll;

    angles = ArmModel::generateClawOrientation(angles, pitch, roll);

    // Map angles to motor IDs
    armMs.motorValues[MOTOR_ID_BASE] = angles[0];
    armMs.motorValues[MOTOR_ID_SHOULDER] = angles[1];
    armMs.motorValues[MOTOR_ID_ELBOW] = angles[2];
    armMs.motorValues[MOTOR_ID_WRIST] = angles[3];

    // ArmState desired orientation
    armMs.motorValues[MOTOR_ID_CLAW_ROLL] = angles[4];
    armMs.motorValues[MOTOR_ID_CLAW_PITCH] = angles[5];

    // Open/close value
    armMs.motorValues[MOTOR_ID_CLAW_OPEN] = armState.clawOpen;

    return armMs;
}

ArmState
ArmProcessor::armForwardsKinematics(const ArmMotorState& armMotorState) {
    std::array<int, 6> motorValues = {};
    for (int i = 0; i <= MOTOR_ID_CLAW_PITCH; i++) {
        motorValues[i] = armMotorState.motorValues[i];
    }

    ArmFKOutput fkOut = ArmModel::forwardsKinematics(motorValues);

    ArmState out = {
        .x = (float)fkOut.wrist_position[0],
        .y = (float)fkOut.wrist_position[1],
        .z = (float)fkOut.wrist_position[2],
        .pitch = (int)fkOut.claw_pitch,
        .roll = (int)fkOut.claw_roll,
        .clawOpen = armMotorState.motorValues[MOTOR_ID_CLAW_OPEN],
    };

    return out;
}

void ArmProcessor::setMode(ArmProcessorMode newMode) {
    switch (newMode) {
    case ArmProcessorMode::TaskSpace:
        // Clear Joint Space State
        velocity.jointSpaceState = {};
        mode = ArmProcessorMode::TaskSpace;
        break;
    case ArmProcessorMode::JointSpace:
        // Clear Task Space State
        velocity.taskSpaceState = {};
        mode = ArmProcessorMode::JointSpace;
        break;
    default:
        break;
    }

    changesMade = false;
}

void ArmProcessor::handleChanges() {
    // Update Timestamp
    auto current_timestamp = std::chrono::steady_clock::now();
    uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              current_timestamp - update_timestamp)
                              .count();
    update_timestamp = current_timestamp;

    switch (mode) {
    case ArmProcessorMode::TaskSpace:
        if (!changesMade && velocity.taskSpaceState == ArmState()) {
            return;
        }

        // Accumulate task space velocity
        state.taskSpaceState
            += velocity.taskSpaceState * (elapsed_ms / MS_PER_SEC);

        // Compute inverse kinematics
        state.jointSpaceState = armInverseKinematics(state.taskSpaceState);
        break;
    case ArmProcessorMode::JointSpace:
        if (!changesMade && velocity.jointSpaceState == ArmMotorState()) {
            return;
        }

        // Accumulate joint space velocity
        state.jointSpaceState
            += velocity.jointSpaceState * (elapsed_ms / MS_PER_SEC);

        // Compute forwards kinematics
        state.taskSpaceState = armForwardsKinematics(state.jointSpaceState);
        break;
    default:
        break;
    }

    changesMade = false;
}
