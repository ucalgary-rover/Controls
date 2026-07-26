#pragma once

#include <memory>

#include "DriveMotorStateManager.h"
#include "HandlerInterface.h"
#include "MessageQueue.h"
#include "Rover/Systems/Drive.h"
#include "Rover/pub_rover.h"

using DriveQueue = MessageQueue<DriveMotorState>;

class DriveHandler : public HandlerInterface {
public:
    bool currentlyGettingZeroState = false;
    DriveHandler(
        std::shared_ptr<Drive> drive, std::shared_ptr<DriveQueue> driveQueue,
        std::shared_ptr<DriveMotorStateManager> currentDriveMotorStateManager);
    ~DriveHandler() { };

    void updateCurrentState();

    void translateSpeedAndAngle(DriveMotorState desiredState,
                                DriveIndex wheelIndex);

    /** @brief Stops all wheel movement
     * @return None
     */
    void stopWheels();

    void setWheelZeroState();

    /**
     * @brief Pops messages out of the queue and calls other functions
     * @return None
     */
    void start() override;

private:
    bool zeroAngleSet = false;
    std::shared_ptr<Drive> m_drive;

    std::shared_ptr<DriveQueue>
        m_driveQueue; // References the (desired) drive state
                      // Manager in Rover.cpp
    std::shared_ptr<DriveMotorStateManager>
        m_currentDriveMotorStateManager; // References the (current) drive state
                                         // Manager in Rover.cpp

    DriveMotorState
        wheelZeroState; // Direct hardware values, not consistent with software DriveModel
    // Variables corresponding to conditions during operation
};
