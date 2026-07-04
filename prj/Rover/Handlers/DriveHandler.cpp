// Schulich Space Program Controls Software Division (2025)
// Written by Johan Esperida

#include "DriveHandler.h"
#include "Message.h"
#include "pub_general.h"
#include <unistd.h>

const char* file = "DriveHandler";

// Various hardcoded values subject to change for fine tuning some movement
// functions

// If a wheel changes angle larger than this threshold all wheel spinning is
// stopped
static const int WHEEL_TURN_THRESH = 20;

// The amount of seconds stopWheels() waits to allow the wheels to lose momentum
static const int STOP_WHEEL_WAIT = 1;

// Might make the following values constants in mission control.h
// is maximum wheel speed for a radial turn
static const float RADIAL_SPEED_MAX = 1;

// is maximum wheel speed for a spot turn
static const float SPOT_SPEED_MAX = 1;

// is the maximum speed for strafing
static const float STRAFE_SPEED_MAX = 1;

// maximum turning angle for radial turning
static const int RADIAL_ANGLE_MAX = 45;

// the angle at which the spot turn reaches max
// speed 90 is the smallest possible angle
static const int MAX_SPEED_ANGLE = 90;

/*
Notes:

Speed determined from magnitude of joystick movement
Large changes in wheel angle will cause the rover to stop to prevent unexpected
movement (> 10 degrees)

Generally, "spinning" is used to refer to the wheels rotating forward or
backwards Generally, "turning" is used to refer to the steppers pointing the
wheels in a different direction

Configured for:
 one joystick on strafing (velocity / direction)
 one joystick on radial / spot turns (angular velocity)
 ***joysticks are to be used in tandem

assumes degrees measured counterclockwise from positive y-axis (on the
joystick)

assumes positive y-axis moves the rover foward when strafing and
radial turning (based on the rover's front)

assumes wheel stepper motors measure angle from positive y-axis increasing
counterclockwise
---> range of motion restricted between 90 and -90

since range of motion limited, all angles given to the wheel must be adjusted to
fit within the range

Stepper motors turn wheels and report when their targets are hit afterwards

*/

const float PI = 3.14;

// two decimal places
#define TO_RADIANS(value) round(value* PI / 180 * 100) / 100

// two decimal places
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

#define DRIVE_UPDATE_INTERVAL_US 50 * 1000 // 50ms

// Constructor
DriveHandler::DriveHandler(
    Drive* drive, DriveMotorStateManager* desiredDriveMotorStateManager,
    DriveMotorStateManager* currentDriveMotorStateManager) {

    // Reference to the m_drive object
    m_drive = drive;

    // Create DriveMotorStateManager objects in this file that reference the
    // DriveMotorStateManager in Rover.cpp (NOT A COPY!!!) This allows us to use
    // the same DriveMotorStateManager in this file without shenanigans
    m_desiredDriveMotorStateManager = desiredDriveMotorStateManager;
    m_currentDriveMotorStateManager = currentDriveMotorStateManager;

    for (int i = 0; i < WHEEL_COUNT; i++) {
        wheelZeroState.drive[i] = 0; // stopped
    }

    float val = m_drive->getWheelAngle(DriveIndex::DRIVE_INDEX_BACK_RIGHT);

    Logging::logI(file, "start_val = %.2f", val);
}

void DriveHandler::updateCurrentState() {
    DriveMotorState currentState; // Construct the current state struct

    // Get values for each wheel
    for (int index = 0; index < WHEEL_COUNT; index++) {
        DriveIndex wheelIndex = static_cast<DriveIndex>(index);
        // Get the current position of the wheel
        currentState.steer[wheelIndex] = m_drive->getWheelAngle(wheelIndex);
        // Truncate since resolution here is
        // not too bad (degrees)

        currentState.drive[wheelIndex] = m_drive->getWheelSpeed(wheelIndex);
    }

    m_currentDriveMotorStateManager->updateState(currentState);
}

void DriveHandler::translateSpeedAndAngle(
    DriveMotorState desiredState,
    DriveIndex wheelIndex) { // From software -> hardware
    double angle = desiredState.steer[wheelIndex];
    double speed = desiredState.drive[wheelIndex];

    if (angle <= 360 && angle >= 0) { // Check valid software range
        if (wheelIndex % 2
            == 0) {           // WHEEL_FL/WHEEL_BL (mounted to m_drive fwds)
            if (angle > 90) { // if angle < 90, do nothing
                if (angle <= 180) { // Wheel should be backwards
                    desiredState.steer[wheelIndex] = -180 + angle;
                    desiredState.drive[wheelIndex] = -speed;
                } else if (angle <= 270) { // Wheel should be backwards
                    desiredState.steer[wheelIndex] = -180 + angle;
                    desiredState.drive[wheelIndex] = -speed;
                } else { // Wheel should be forwards
                    desiredState.steer[wheelIndex] = -360 + angle;
                    desiredState.drive[wheelIndex] = speed;
                }
            }
        } else {               // WHEEL_FR/WHEEL_BR (mounted to m_drive bkwds)
            if (angle <= 90) { // Wheel should be backwards
                desiredState.drive[wheelIndex] = -speed;
            } else if (angle <= 180) { // Wheel should be forwards
                desiredState.steer[wheelIndex] = -angle;
            } else if (angle <= 270) { // Wheel should be forwards
                desiredState.steer[wheelIndex] = -180 + angle;
            } else { // Wheel should be backwards
                desiredState.steer[wheelIndex] = -360 + angle;
                desiredState.drive[wheelIndex] = -speed;
            }
        }
    } else {
        throw std::runtime_error(
            "Invalid angle detected: outside of 0-360 degree range.");
    }
}

void DriveHandler::stopWheels() {
    // sets target velocities of all wheels to zero
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        DriveIndex wheelIndex = static_cast<DriveIndex>(dc);
        m_drive->setWheelSpeed(wheelIndex, 0);
    }

    // wait for an amount of seconds to let the wheels slow down
    usleep(STOP_WHEEL_WAIT * 1000000);
}

void DriveHandler::setWheelZeroState() {
    for (int index = 0; index < WHEEL_COUNT; index++) {
        DriveIndex i = static_cast<DriveIndex>(index);
        wheelZeroState.steer[index] = m_drive->getWheelAngle(i);
    }
    zeroAngleSet = true;
}

void DriveHandler::start() {
    while (true) {

        // Read from desired state
        DriveMotorState desiredState
            = m_desiredDriveMotorStateManager->getState();

        try {
            if (!currentlyGettingZeroState) {
                for (int index = 0; index < WHEEL_COUNT; index++) {
                    DriveIndex i = static_cast<DriveIndex>(index);

                    // Translate into hardware specific angles and speed
                    translateSpeedAndAngle(desiredState, i);

                    // Update wheel speed and angle
                    m_drive->setWheelAngle(i, desiredState.steer[i]);
                    m_drive->setWheelSpeed(i, desiredState.drive[i]);
                }
            } else {
                if (zeroAngleSet) {
                    for (int index = 0; index < WHEEL_COUNT; index++) {
                        DriveIndex i = static_cast<DriveIndex>(index);
                        m_drive->setWheelAngle(i, wheelZeroState.steer[i]);
                    }
                }

                // Check if we are done getting the zero state
                bool doneGettingZeroState = true;
                for (int index = 0; index < WHEEL_COUNT; index++) {
                    DriveIndex i = static_cast<DriveIndex>(index);
                    if (abs(m_drive->getWheelAngle(i)
                            - wheelZeroState.steer[index])
                        > STEER_THRESHOLD) {
                        doneGettingZeroState = false;
                    }
                }
                currentlyGettingZeroState = !doneGettingZeroState;
            }
        } catch (const std::runtime_error& e) {
            // Erroneous angle detected
            std::cerr << e.what() << std::endl;

            // Stop all wheels
            for (int index = 0; index < WHEEL_COUNT; index++) {
                DriveIndex i = static_cast<DriveIndex>(index);
                m_drive->setWheelSpeed(i, 0.0);
            }
        }

        // Update current state for sending back to base
        updateCurrentState();

        // Sleep to unblock CPU
        usleep(DRIVE_UPDATE_INTERVAL_US);
    }
}