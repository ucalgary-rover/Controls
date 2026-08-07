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

#include "Models/ArmModelCylindrical.h"

#define MS_PER_SEC 1000.0

static const char* file = "ArmProcessor";

ArmProcessor::ArmProcessor(const ArmStateCylindrical& defaultArmState) {
    ArmModelCylindrical::initialize();

    mode = ArmProcessorMode::TaskSpace;
    state.taskSpaceState = defaultArmState;
    state.jointSpaceState = armInverseKinematics(defaultArmState);
    update_timestamp = std::chrono::steady_clock::now();
}

ArmProcessor::ArmProcessor(const ArmMotorState& defaultArmMotorState) {
    ArmModelCylindrical::initialize();

    mode = ArmProcessorMode::JointSpace;
    state.taskSpaceState = armForwardsKinematics(defaultArmMotorState);
    state.jointSpaceState = defaultArmMotorState;
    update_timestamp = std::chrono::steady_clock::now();
}

void ArmProcessor::setTaskSpaceState(const ArmStateCylindrical& armState) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(ArmProcessorMode::TaskSpace);

    // State Set overrides all modifiers
    state.taskSpaceState = armState;
    changesMade = true;
}

void ArmProcessor::incrementTaskSpaceState(const ArmStateCylindrical& delta) {
    std::lock_guard<std::mutex> lock(mtx);

    if (mode != ArmProcessorMode::TaskSpace) {
        handleChanges();
        setMode(ArmProcessorMode::TaskSpace);
    }

    // Accumulate task space changes
    state.taskSpaceState += delta;
    changesMade = true;
}

void ArmProcessor::setTaskSpaceVelocity(
    const ArmStateCylindrical& taskSpaceVelocity) {
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

ArmStateCylindrical ArmProcessor::getTaskSpaceState() {
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

ArmMotorState
ArmProcessor::armInverseKinematics(const ArmStateCylindrical& armState) const {
    ArmMotorState armMotorState;
    if (!ArmModelCylindrical::inverseKinematics(armState, armMotorState)) {
        Logging::logI(file, "Inverse Kinematics Failed");
        armMotorState = state.jointSpaceState;
    }

    return armMotorState;
}

ArmStateCylindrical
ArmProcessor::armForwardsKinematics(const ArmMotorState& armMotorState) const {
    ArmStateCylindrical armState;
    if (!ArmModelCylindrical::forwardsKinematics(armMotorState, armState)) {
        Logging::logI(file, "Forwards Kinematics Failed");
        armState = state.taskSpaceState;
    }

    return armState;
}

void ArmProcessor::saveCurrentAsMin(MotorID joint) {
    ArmModelCylindrical::setMin(joint,
                                state.jointSpaceState.motorValues[joint]);
}

void ArmProcessor::saveCurrentAsMax(MotorID joint) {
    ArmModelCylindrical::setMax(joint,
                                state.jointSpaceState.motorValues[joint]);
}

void ArmProcessor::printLimits() { ArmModelCylindrical::printLimits(); }

void ArmProcessor::resetLimits() { ArmModelCylindrical::initialize(); }

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

    bool stateChanged = false;

    switch (mode) {
    case ArmProcessorMode::TaskSpace:
        if (!changesMade && velocity.taskSpaceState == ArmStateCylindrical()) {
            return;
        }

        // Accumulate task space velocity
        state.taskSpaceState
            += velocity.taskSpaceState * (elapsed_ms / MS_PER_SEC);

        // Compute inverse kinematics
        state.jointSpaceState = armInverseKinematics(state.taskSpaceState);

        stateChanged = true;
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

        stateChanged = true;
        break;
    default:
        break;
    }

    if (stateChanged) {
        Logging::logI(
            file, "theta: %.2f r: %.2f z: %.2f pitch: %d roll: %d clawOpen: %d",
            state.taskSpaceState.theta, state.taskSpaceState.r,
            state.taskSpaceState.z, state.taskSpaceState.pitch,
            state.taskSpaceState.roll, state.taskSpaceState.clawOpen);

        auto& vals = state.jointSpaceState.motorValues;
        Logging::logI(file, "motors: %d %d %d %d %d %d", vals[0], vals[1],
                      vals[2], vals[3], vals[4], vals[5]);
    }

    changesMade = false;
}
