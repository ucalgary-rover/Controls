#include "Drive.h"
#include <cmath>
#include <stdio.h>

/*
Notes:

Speed determined from magnitude of joystick movement
Not yet adjusted for large changes of direction while wheels are moving

Configured for: (subject to change)
 one joystick on strafing (velocity / direction)
 one joystick on radial / spot turns (velocity / angular velocity)
 ***one joystick is to be used at a time

 assumes degrees measured counterclockwise from positive x-axis (on the
joystick) assumes positive y-axis moves the rover foward when strafing and
radial turning (based on the rover's front)

*/

Drive::Drive(float width, float length) : m_width(width), m_length(length) {
    m_handlesDC = new PhidgetDCMotorHandle*[DRIVE_INDEX_WHEEL_COUNT]();
    m_handlesStepper = new PhidgetStepperHandle*[DRIVE_INDEX_WHEEL_COUNT]();

    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        PhidgetDCMotor_create(m_handlesDC[dc]);
        setAddressProperties<PhidgetDCMotorHandle>(
            m_handlesDC[dc], DRIVE_DC_SERIAL_NUMBER[dc], DRIVE_DC_CHANNEL[dc]);
    }

    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        PhidgetStepper_create(m_handlesStepper[stepper]);
        setAddressProperties<PhidgetStepperHandle>(
            m_handlesStepper[stepper], DRIVE_STEPPER_SERIAL_NUMBER[stepper],
            DRIVE_STEPPER_CHANNEL[stepper]);
    }
}

Drive::~Drive() {
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        Phidget_close((PhidgetHandle)*m_handlesDC[dc]);
        PhidgetDCMotor_delete(m_handlesDC[dc]);
    }

    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        Phidget_close((PhidgetHandle)*m_handlesStepper[stepper]);
        PhidgetStepper_delete(m_handlesStepper[stepper]);
    }
}

bool Drive::getDriveDCHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->motorHandle.dcMotor = m_handlesDC[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Drive::getDriveStepperHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->motorHandle.stepperMotor = m_handlesStepper[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

float Drive::getLength() { return m_length; }

float Drive::getWidth() { return m_width; }

// axis snapping not too helpful here <-------------------------------
void Drive::spotTurn(int stickAngle) {

    // determine direction of wheel spin and speed based on stickAngle

    // starts with an offset of 90 degrees
    stickAngle -= 90;

    // adjusts stickAngle to match control scheme
    int reportedAngle;

    // corresponding to a left turn
    if (stickAngle < 180 && stickAngle > 0) {
        reportedAngle = stickAngle;

        // corresponding to a right turn
    } else if (stickAngle < 0) {
        reportedAngle = stickAngle;

        // also for a right turn
    } else if (stickAngle > 180) {
        reportedAngle = stickAngle - 360;

        // neutral positions
    } else if (stickAngle == 0 || stickAngle == 180) {
        reportedAngle == 0;
    }

    // determining the amount to adjust the max speed
    //  1 is maximum
    float speedAdjust = reportedAngle / 179;

    // default directions and speeds (those of a left turn at full speed)
    float leftSpeed = -1;
    float rightSpeed = 1;

    // multiply by calcuated speed adjustment
    // if the speed adjustment is negative, a right turn happens and directions
    // are switched
    leftSpeed *= speedAdjust;
    rightSpeed *= speedAdjust;

    // turn wheels

    // if moving into spot turn configuration, turn off power to wheels
    if (!m_spotTurnFlag) {

        stopWheels(); // maybe stick some delay in here somewhere?
                      // <---------------------

        // Angle based on length and width
        float angle = atan(m_width / m_length);

        for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {

            // checking for angle to turn each wheel so they rotate properly

            if (stepper == 0 || stepper == 3) {
                turnWheel((PhidgetHandle)*m_handlesStepper[stepper],
                          90 - angle);

            } else {
                turnWheel((PhidgetHandle)*m_handlesStepper[stepper],
                          90 + angle);
            }
        }
    }

    // spin wheels
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {

        // checking for direction to spin each wheel

        // if index odd the wheel is on the right of chassis
        if (dc % 2 == 1) {
            PhidgetDCMotor_setTargetVelocity((PhidgetHandle)*m_handlesDC[dc],
                                             rightSpeed);

            // if index is even the wheel is on the left of chassis
        } else {
            PhidgetDCMotor_setTargetVelocity((PhidgetHandle)*m_handlesDC[dc],
                                             leftSpeed);
        }
    }
}

// Ideally know the difference between current angle and the desired
// <---------------------------
void Drive::strafe(int stickAngle, int stickMagnitude) {

    // adjusting angle for wheels based on stick angle (keeping in mind 180
    // restriction)
    int reportedAngle;
    int direction;

    // corresponding to forward wheel spin
    if (stickAngle <= 180) {
        reportedAngle = stickAngle;
        direction = 1;

        // corresponding to backward wheel spin
    } else if (stickAngle > 180) {
        reportedAngle = stickAngle - 180;
        direction = -1;
    }

    // deriving speed from magnitude (radius of joystick circle is 255)
    float speed = stickMagnitude / 255;

    // Turn
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        turnWheel((PhidgetHandle)*m_handlesDC[stepper], reportedAngle);
    }

    // Spin
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        PhidgetDCMotor_setTargetVelocity((PhidgetHandle)*m_handlesDC[dc],
                                         speed * direction);
    }
}

void Drive::stopWheels() {
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        PhidgetDCMotor_setTargetVelocity((PhidgetHandle)*m_handlesDC[dc], 0);
    }
}

// definitely reduce the axis snapping on this <-------------------------------
void Drive::radialTurn(int stickAngle, int stickMagnitude) {

    // derive a percentage for the amount turned (0 - 100% for each quadrant)
    // makes more sense with a diagram
    // start with an offset of 90
    float reportedAngle;

    //-1 is backwards, 1 is forwards
    int direction;

    if (stickAngle < 180) {
        reportedAngle = (stickAngle - 90) / 90 * 45;
        direction = 1;

        // Report the angle of the other hemisphere
    } else if (stickAngle >= 180) {
        reportedAngle = (stickAngle - 270) / 90 * 45;
        direction = -1;
    }

    int headingAngle = abs(reportedAngle);

    // Finding the angles of the leading wheels
    float innerAngle = atan(
        2 * m_length * sin(headingAngle)
        / (2 * m_length * cos(headingAngle) - m_width * sin(headingAngle)));

    float outerAngle = atan(
        2 * m_length * sin(headingAngle)
        / (2 * m_length * cos(headingAngle) + m_width * sin(headingAngle)));

    // Determine inner and outer wheel angles based on direction and reported
    // angle signs Keeping absolute direction of front and angle from positive
    // x-axis from here on Logic makes more sense with a diagram

    float leftAngle;
    float rightAngle;

    // left wheel is inner wheel
    if (direction * reportedAngle > 0) {
        leftAngle = (reportedAngle < 0) ? innerAngle : 180 - innerAngle;
        rightAngle = (reportedAngle < 0) ? outerAngle : 180 - outerAngle;

        // right wheel is inner wheel
    } else if (direction * reportedAngle < 0) {
        leftAngle = (reportedAngle < 0) ? outerAngle : 180 - outerAngle;
        rightAngle = (reportedAngle < 0) ? innerAngle : 180 - innerAngle;

        // moving straight
    } else {

        leftAngle = 90;
        rightAngle = 90;
    }

    // Turn wheels

    // value for deciding which wheels must be turned
    int wheelThresh = 1 - direction;

    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {

        // makes sure the proper wheels are angled based on direction of
        // movement

        // angles wheels 0, 1 if forward and 2, 3 if backward
        if (stepper == wheelThresh) {
            turnWheel((PhidgetHandle)*m_handlesDC[stepper], leftAngle);

        } else if (stepper == wheelThresh + 1) {
            turnWheel((PhidgetHandle)*m_handlesDC[stepper], rightAngle);

        } else {
            turnWheel((PhidgetHandle)*m_handlesDC[stepper], 90);
        }
    }

    // Spin wheels

    // deriving speed from magnitude (radius of joystick circle is 255)
    float speed = stickMagnitude / 255;

    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        PhidgetDCMotor_setTargetVelocity((PhidgetHandle)*m_handlesDC[dc],
                                         speed * direction);
    }
}

void Drive::eventLoop() {

    while (true) {

        // recieving the message (make it work properly later)
        // <-------------------------
        WheelMessage message;

        // flags for if the value is non-zero
        bool angleVelocityFlag = message.angle_velocity != 0;
        bool velocityFlag = message.angle_velocity != 0;

        // Scenarios

        // Radial turn (angular velocity and velocity)
        if (angleVelocityFlag && velocityFlag) {
            m_spotTurnFlag = false;
            radialTurn(message.angle_velocity, message.velocity);

            // Spot turning (angular velocity)
        } else if (angleVelocityFlag) {
            spotTurn(message.angle_velocity);

            // Strafing (angle and velocity)
        } else if (velocityFlag) {
            m_spotTurnFlag = false;
            strafe(message.theta, message.velocity);

            // execute stop
        } else {
            stopWheels();
        }
    }
}