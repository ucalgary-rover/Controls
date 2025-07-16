// Schulich Space Program Controls Software Division (2025)
// Written by Johan Esperida

#include "DriveHandler.h"
#include "Rover/Systems/pub_systems.h"
#include "Rover/pub_rover.h"
#include "phidget22.h"
#include <cmath>

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
-----------------> waiting for the wheels to finish turning still needs to be
programmed
- Probably make some function "awaitWheelTargets" that waits until targets hit

*/

const float PI = 3.14;

// two decimal places
#define TO_RADIANS(value) round(value* PI / 180 * 100) / 100

// two decimal places
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

// Default constructor
DriveHandler::DriveHandler() { }

// Constructor
DriveHandler::DriveHandler(Drive& drive, MessageQueue& driveQueue) {

    // Reference to the Arm object in Rover.cpp
    m_drive = drive;

    // Create a driveQueue object in this file that references the driveQueue in
    // Rover.cpp (NOT A COPY!!!) This allows us to use the same driveQueue in
    // this file without shenanigans
    m_driveQueue = driveQueue;

    instantiateThreads();
}

void DriveHandler::turnWheel(PhidgetStepperHandle stepper, int angle) {
    // check if the turn is larger than 10 degrees
    double currentPos;

    PhidgetStepper_getPosition(stepper, &currentPos);

    if (abs(currentPos - angle) > 10) {
        stopWheels();
    }

    // turn the wheel
    PhidgetStepper_setTargetPosition(stepper, angle);
}

float DriveHandler::spotTurnSpeed(int stickAngle) {
    // determine direction of wheel spin and speed based on stickAngle
    float speedAdjust;
    int reportedAngle;

    // right turn
    if (stickAngle > 180) {
        // note the negative values
        reportedAngle = stickAngle - 360;

        // left turn
    } else if (stickAngle < 180) {
        reportedAngle = stickAngle;

        // deadzone
    } else if (stickAngle == 180) {
        reportedAngle = 0;
    }

    // adjusts value up until a maximum (1 = 100%)
    speedAdjust = reportedAngle / m_maxSpeedAngle;

    // prevents value from going above maximum by rounding down to 1
    speedAdjust = (std::abs(speedAdjust) > 1) ? (int)speedAdjust : speedAdjust;

    // calculates speed based on the adjustment with the sign determining the
    // direction
    float speed = speedAdjust * m_radialSpeedMax;

    // negative value results in a right turn, otherwise left turn
    return speed;
}

void DriveHandler::spotTurn(int stickAngle) {

    // deriving speed based on angle of right stick
    float speed = spotTurnSpeed(stickAngle);

    // default directions(those of a left turn at full speed)
    // if speedAdjust is negative, right turn takes place
    float leftSpeed = -speed;
    float rightSpeed = speed;

    // turn wheels

    // if moving into spot turn configuration, prevents wheels from spinning
    if (!m_spotTurnFlag) {

        stopWheels(); // Ensure the wheels wait to finish turning
                      //  <---------------------
    }

    // Angle of wheel direction based on length and width
    float wheelAngle
        = TO_DEGREES(atan(m_drive.getWidth() / m_drive.getLength()));

    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {

        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, stepper);

        // checking for angle to turn each wheel so they rotate properly

        if (stepper == 0 || stepper == 3) {
            turnWheel(*motorStuct.motorHandle.stepperMotor, -wheelAngle);

        } else {
            turnWheel(*motorStuct.motorHandle.stepperMotor, wheelAngle);
        }
    }

    // spin wheels
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveDCHandle(&motorStuct, dc);

        // checking for direction to spin each wheel

        // if index odd the wheel is on the right of chassis
        if (dc % 2 == 1) {
            PhidgetDCMotor_setTargetVelocity(*motorStuct.motorHandle.dcMotor,
                                             rightSpeed);

            // if index is even the wheel is on the left of chassis
        } else {
            PhidgetDCMotor_setTargetVelocity(*motorStuct.motorHandle.dcMotor,
                                             leftSpeed);
        }
    }
}

int DriveHandler::strafeAngleAdjust(int stickTheta) {
    // adjusting angle for wheels based on stick angle (keeping in mind 180
    // restriction)

    int reportedAngle;

    // corresponding to forward wheel spin
    if (stickTheta < 90 || stickTheta > 270) {
        reportedAngle = stickTheta;

        // corresponding to backward wheel spin
    } else if (stickTheta > 90 && stickTheta < 270) {
        // 180 is subtracted to put the angle in the range of motion

        reportedAngle = stickTheta - 180;

        // edge positions ----> (massive headache ahead) <----
        // since 90 and 270 are equivalent positions, the following code tries
        // to find which is easiest to move to
    } else if (reportedAngle == 90 || reportedAngle == 270) {
        // used to hold the current position
        double currentPos;

        // these are used to find the average wheel orientation
        float positionSum;
        int positionAverage;

        // sum the wheel positions and get the average
        for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
            // get the handler
            MotorHandlerReturn motorStuct;
            m_drive.getDriveStepperHandle(&motorStuct, stepper);

            PhidgetStepper_getPosition(*motorStuct.motorHandle.stepperMotor,
                                       &currentPos);
            positionSum += currentPos;
        }

        positionAverage = (int)(positionSum / DRIVE_INDEX_WHEEL_COUNT);

        // finding differences between current position and possible
        // satisfactory end points
        int diff90 = 90 - positionAverage;

        // corresponding to the -90 position
        int diffAlt90 = -90 - positionAverage;

        // set the reported angle to whichever has the lowest difference
        if (diff90 < diffAlt90) {
            reportedAngle = 90;

        } else {
            reportedAngle = -90;
        }
    }

    return reportedAngle;
}

void DriveHandler::strafe(int stickTheta, int stickMagnitude) {

    // finding the reported angle
    int wheelAngle = strafeAngleAdjust(stickTheta);

    // forward or backward wheel spin (and the special edge cases) based on
    // joystick angle
    int direction;

    // on the edge cases
    if (stickTheta == 90 || stickTheta == 270) {

        // converts to angles within wheel range to compare to the wheel angle
        stickTheta = (stickTheta = 270) ? -90 : 90;

        // if the angles are the same, move forward, if not move backwards
        direction = (wheelAngle == stickTheta) ? 1 : -1;

        // moving forward
    } else if (stickTheta < 90 || stickTheta > 270) {
        direction = 1;

        // moving backward
    } else {
        direction = -1;
    }

    // deriving speed from magnitude (percentage of radius of joystick circle)
    float speed = stickMagnitude * direction * m_strafeSpeedMax;

    // Turn
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, stepper);

        turnWheel(*motorStuct.motorHandle.stepperMotor, wheelAngle);
    }

    // Spin
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.motorHandle.dcMotor,
                                         speed);
    }
}

float DriveHandler::radialTurnHeadingAngle(int stickAngle) {

    // derive a percentage for the amount turned (0 - 100% for each quadrant)
    // and multiply by a maximum turning angle
    // angle is from the right joystick
    // makes more sense with a diagram

    float reportedAngle = 0;

    // offsets angle if above 180
    if (stickAngle >= 180) {
        stickAngle -= 180;
    }

    // corresponding to a left turn
    if (stickAngle < 90) {
        reportedAngle = (stickAngle / 90) * m_radialAngleMax;

        // corresponding to a right turn (note the negative reported angle
        // value)
    } else if (stickAngle > 90) {
        reportedAngle = ((stickAngle - 180) / 90) * m_radialAngleMax;

        // max positions
    } else if (stickAngle = 90) {
        reportedAngle = m_radialAngleMax * m_lastRadialOutput;

        // neutral positions
    } else if (stickAngle == 0) {
        reportedAngle = 0;
    }

    // set new lastRadialOutput
    // also use this to determine direction later (negative is right turn)
    m_lastRadialOutput = (reportedAngle > 0) ? 1 : -1;

    // must convert to radians for trig functions
    float headingAngle = TO_RADIANS(std::abs(reportedAngle));
}

float DriveHandler::radialTurnWheelAngles(int headingAngle, bool innerFlag) {
    // adjusts the calculation based on if inner wheel or outer wheel
    int wheelInt = (innerFlag) ? 1 : -1;

    // Finding the angles of the leading wheels
    float wheelAngle
        = atan(2 * m_drive.getLength() * sin(headingAngle)
               / (2 * m_drive.getLength() * cos(headingAngle)
                  - wheelInt * m_drive.getWidth() * sin(headingAngle)));

    return TO_DEGREES(wheelAngle);
}

// Applies to the front wheels regardless of direction
void DriveHandler::radialTurn(int stickAngle, int stickMagnitude,
                              int stickTheta) {

    // Finding heading angle
    float headingAngle = radialTurnHeadingAngle(stickAngle);

    // calculating angles of leading wheels
    float innerAngle = radialTurnWheelAngles(headingAngle, true);

    float outerAngle = radialTurnWheelAngles(headingAngle, false);

    // Determining left and right angles based on directions

    float leftAngle;
    float rightAngle;

    // left wheel is inner wheel (left turn)
    if (m_lastRadialOutput > 0) {
        leftAngle = innerAngle;
        rightAngle = outerAngle;

        // right wheel is inner wheel (right turn)
    } else if (m_lastRadialOutput < 0) {
        leftAngle = -outerAngle;
        rightAngle = -innerAngle;
    }

    // Turn wheels (front)
    // list of angles to iterate thru
    float wheelAngles[2] = { leftAngle, rightAngle };

    for (int i = 0; i < 2; ++i) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, i);

        turnWheel(*motorStuct.motorHandle.stepperMotor, wheelAngles[i]);
    }

    // Spin wheels

    // direction based on left joystick angle
    int direction = (stickTheta <= 90 || stickTheta >= 270) ? 1 : -1;

    // deriving speed from magnitude (percentage of radius of joystick circle)
    float speed = stickMagnitude * direction * m_radialSpeedMax;

    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.motorHandle.dcMotor,
                                         speed);
    }
}

void DriveHandler::stopWheels() {
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive.getDriveStepperHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.motorHandle.dcMotor, 0);
    }
}

void DriveHandler::eventLoop() {

    Message message;

    while (true) {
        // Get message from armQueue
        message = m_driveQueue->pop();

        // flags for if the value is non-zero
        bool angleVelocityFlag
            = message.angle_velocity != 0;         // right joystick angle
        bool velocityFlag = message.velocity != 0; // left joystick magnitude

        // for if the movement is forward or backward
        // left joystick angle
        bool thetaFlag = (message.theta < 10 && message.theta > 350)
                         || (message.theta < 190 && message.theta > 170);

        // Scenarios

        // Radial turn (angular velocity and velocity forwards or backwards)
        if (angleVelocityFlag && thetaFlag) {
            m_spotTurnFlag = false;
            radialTurn(message.angle_velocity, message.velocity, message.theta);

            // Spot turning (angular velocity)
        } else if (angleVelocityFlag) {
            spotTurn(message.angle_velocity);

            // Strafing (angle and velocity)
        } else if (velocityFlag) {
            m_spotTurnFlag = false;
            strafe(message.theta, message.velocity);

            // execute stop if no joysticks active
        } else {
            stopWheels();
        }
    }
}
