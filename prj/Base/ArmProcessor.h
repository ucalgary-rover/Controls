/**
 * @file ArmProcessor.h
 * @brief Arm Processor handles thread synchronized modifications to the ArmState as well as inverse and forwards kinematics.
 *
 * @details This class keeps to only absolute value of the ArmState on Base computing inverse and forwards kinematics whenever changes are made.
 * The ArmProcessor supports both incremental and velocity based changes to the Arm state. When switching between task and joint space control, 
 * the accumulated changes will be applied to synchronize task and joint space truths prior to swapping accumulation modes. This design assumes that
 * switching between inverse and forwards kinematics control will be infrequent as each time the mode is swapped the full IK algorithm must be executed.
 */

#pragma once

#include <chrono>
#include <memory>
#include <mutex>

#include "ArmMotorState.h"
#include "ArmState.h"

struct ArmProcessorState {
    ArmState taskSpaceState;
    ArmMotorState jointSpaceState;
};

enum class ArmProcessorMode {
    TaskSpace,
    JointSpace,
};

class ArmProcessor {
public:
    ArmProcessor(const ArmState& defaultArmState);

    ArmProcessor(const ArmMotorState& defaultArmMotorState);

    /**
     * @brief Sets the Task space state computing joint space via Inverse Kinematics.
     */
    void setTaskSpaceState(const ArmState& armState);

    /**
     * @brief Increments the Task space state computing joint space via Inverse Kinematics.
     */
    void incrementTaskSpaceState(const ArmState& delta);

    /**
     * @brief Sets the Task space velocity used for calculating accumulated changes upon getting state.
     */
    void setTaskSpaceVelocity(const ArmState& taskSpaceVelocity);

    /**
     * @brief Sets the Joint space state computing joint space via Inverse Kinematics.
     */
    void setJointSpaceState(const ArmMotorState& armMotorState);

    /**
     * @brief Increments the Joint space state computing joint space via Inverse Kinematics.
     */
    void incrementJointSpaceState(const ArmMotorState& delta);

    /**
     * @brief Sets the Joint space velocity used for calculating accumulated changes upon getting state.
     */
    void setJointSpaceVelocity(const ArmMotorState& jointSpaceVelocity);

    /**
     * @brief Gets task space state synchronzing changes with joint space state.
     */
    ArmState getTaskSpaceState();

    /**
     * @brief Gets joint space state synchronzing changes with task space state.
     */
    ArmMotorState getJointSpaceState();

    /**
     * @brief Gets the synchronized task and joint space in one instant.
     */
    ArmProcessorState getState();

    void setRoverState(const ArmMotorState& armMotorState);
    ArmMotorState getRoverState();

    static ArmMotorState armInverseKinematics(const ArmState& armState);

    static ArmState armForwardsKinematics(const ArmMotorState& armMotorState);

private:
    std::mutex mtx;
    ArmProcessorMode mode;
    bool changesMade = false;

    ArmProcessorState state = {};
    ArmProcessorState velocity = {};

    ArmMotorState roverMotorState = {};

    std::chrono::steady_clock::time_point update_timestamp
        = std::chrono::steady_clock::now();

    void setMode(ArmProcessorMode newMode);

    void handleChanges();
};