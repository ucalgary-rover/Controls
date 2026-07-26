/**
 * @file DriveProcessor.cpp
 * @brief Drive Processor handles thread synchronized modifications to the DriveState including manual to auto transitions.
 *
 * @details Similar to the ArmProcessor this class keeps the only absolute copy of the DriveState in which controller threads
 * can only modify via explicit calls to DriveProcessor methods which will maintain threadsafe state management.
 */

#include "DriveProcessor.h"

#include "Models/DriveModel.h"

#define MS_PER_SEC 1000.0

DriveProcessor::DriveProcessor(const DriveState& defaultDriveState) {
    DriveModel::initialize();

    mode = DriveProcessorMode::DriverSpace;
    state.driverState = defaultDriveState;
    state.motorState = processDriveState(defaultDriveState, {});
    update_timestamp = std::chrono::steady_clock::now();
}

DriveProcessor::DriveProcessor(const DriveMotorState& defaultDriveMotorState) {
    DriveModel::initialize();

    mode = DriveProcessorMode::MotorSpace;
    state.driverState = {};
    state.motorState = defaultDriveMotorState;
    update_timestamp = std::chrono::steady_clock::now();
}

void DriveProcessor::setSpeed(int speed) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::DriverSpace);

    state.driverState.speed = speed;
    changesMade = true;
}

void DriveProcessor::setHeading(int heading) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::DriverSpace);

    state.driverState.heading = heading;
    changesMade = true;
}

void DriveProcessor::setAngularVelocity(int angularVelocity) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::DriverSpace);

    state.driverState.angularVelocity = angularVelocity;
    changesMade = true;
}

void DriveProcessor::setSteerPosition(WheelID wheel, float position) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::MotorSpace);
    state.motorState.steer[wheel] = position;
    changesMade = true;
}

void DriveProcessor::incrementSteerPosition(WheelID wheel, float increment) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::MotorSpace);
    state.motorState.steer[wheel] += increment;
    changesMade = true;
}

void DriveProcessor::setSteerVelocity(WheelID wheel, float velocity) {
    std::lock_guard<std::mutex> lock(mtx);

    setMode(DriveProcessorMode::MotorSpace);

    motorVelocity.steer[wheel] = velocity;
    changesMade = true;
}

DriveState DriveProcessor::getDriverState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state.driverState;
}

DriveMotorState DriveProcessor::getDriveMotorState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state.motorState;
}

DriveProcessorState DriveProcessor::getState() {
    std::lock_guard<std::mutex> lock(mtx);

    handleChanges();

    return state;
}

void DriveProcessor::setRoverState(const DriveMotorState& driveMotorState) {
    std::lock_guard<std::mutex> lock(mtx);

    roverMotorState = driveMotorState;
}

DriveMotorState DriveProcessor::getRoverState() {
    std::lock_guard<std::mutex> lock(mtx);

    return roverMotorState;
}

DriveMotorState
DriveProcessor::processDriveState(const DriveState& driveState,
                                  const DriveMotorState& currentMotorState) {
    return DriveModel::process(driveState, currentMotorState);
}

void DriveProcessor::setMode(DriveProcessorMode newMode) {
    switch (newMode) {
    case DriveProcessorMode::DriverSpace:
        // Clear Motor Space State
        motorVelocity = {};
        mode = DriveProcessorMode::DriverSpace;
        break;
    case DriveProcessorMode::MotorSpace:
        // Clear Drive Space State
        mode = DriveProcessorMode::MotorSpace;
        break;
    default:
        break;
    }

    changesMade = false;
}

void DriveProcessor::handleChanges() {
    // Update Timestamp
    auto current_timestamp = std::chrono::steady_clock::now();
    uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              current_timestamp - update_timestamp)
                              .count();
    update_timestamp = current_timestamp;

    switch (mode) {
    case DriveProcessorMode::DriverSpace:
        if (!changesMade) {
            return;
        }

        // Compute Motor State
        state.motorState
            = processDriveState(state.driverState, roverMotorState);
        break;
    case DriveProcessorMode::MotorSpace:
        if (!changesMade && motorVelocity == DriveMotorState()) {
            return;
        }

        // Compute Motor State
        state.motorState += motorVelocity * (elapsed_ms / MS_PER_SEC);

        // Invalidate Driver state
        state.driverState = {};

        // Clear Drive Space State
        mode = DriveProcessorMode::MotorSpace;
        break;
    default:
        break;
    }

    changesMade = false;
}
