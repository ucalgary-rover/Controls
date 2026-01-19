#include "DriveModel.h"
#include <cmath>

static const int RADIAL_ANGLE_MAX = 45;
static const int MAX_SPEED_ANGLE = 90;
static const float RADIAL_SPEED_MAX = 1;
static const float SPOT_SPEED_MAX = 1;
static const float STRAFE_SPEED_MAX = 1;

const float PI = 3.14;

// two decimal places
#define TO_RADIANS(value) round(value* PI / 180 * 100) / 100

// two decimal places
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

Drive* DriveModel::m_drive = nullptr;
int DriveModel::m_lastRadialOutput = 1;

void DriveModel::initialize(Drive* drive) { m_drive = drive; }

float DriveModel::spotTurnSpeed(int stickAngle) {
    // determine direction of wheel spin and speed based on stickAngle

    // right turn
    if (stickAngle < 30 || stickAngle > 330) {
        return 0;
    } else if (stickAngle > 180) {
        // note the negative values
        return -1 + (abs(270 - (float)stickAngle) / 60);
    } else {
        // left turn
        return 1 - (abs(90 - (float)stickAngle) / 60);
    }
}

int DriveModel::strafeAngleAdjust(int stickTheta) {
    // adjusting angle for wheels based on stick angle (keeping in mind 180
    // restriction)

    int reportedAngle;

    // corresponding to forward wheel spin
    if (stickTheta < 90) {
        reportedAngle = stickTheta;

    } else if (stickTheta > 270) {
        reportedAngle = stickTheta - 360;

        // corresponding to backward wheel spin
    } else if (stickTheta > 90 && stickTheta < 270) {
        // 180 is subtracted to put the angle in the range of motion

        reportedAngle = stickTheta - 180;

        // edge positions ----> (massive headache ahead) <----
        // since 90 and 270 are equivalent positions, the following code tries
        // to find which is easiest to move to
    } else if (stickTheta == 90 || stickTheta == 270) {
        // used to hold the current position
        double currentPos;

        // these are used to find the average wheel orientation
        float positionSum = 0;
        int positionAverage;

        // sum the wheel positions and get the average
        for (int stepper = 0; stepper < 4; stepper++) {
            // get the handler
            MotorHandlerReturn motorStuct;
            m_drive->getDriveStepperHandle(&motorStuct, (DriveIndex)stepper);

            PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                                       &currentPos);
            positionSum += currentPos;
        }

        positionAverage = (int)(positionSum / 4);

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

float DriveModel::radialTurnHeadingAngle(int stickAngle) {

    // derive a percentage for the amount turned (0 - 100% for each quadrant)
    // and multiply by a maximum turning angle
    // angle is from the right joystick
    // makes more sense with a diagram

    float reportedAngle = 0;

    // corresponding to a left turn
    if (stickAngle < 90) {
        reportedAngle = (((float)stickAngle * RADIAL_ANGLE_MAX) / 90);

        // corresponding to a right turn (note the negative reported angle
        // value)
    } else if (stickAngle > 180) {
        reportedAngle = ((((float)stickAngle - 360) * RADIAL_ANGLE_MAX) / 90);
    }

    // set new lastRadialOutput
    // also use this to determine direction later (negative is right turn)
    m_lastRadialOutput = (reportedAngle > 0) ? 1 : -1;

    // must convert to radians for trig functions
    float headingAngle = TO_RADIANS(std::abs(reportedAngle));

    return headingAngle;
}

float DriveModel::radialTurnWheelAngles(float headingAngle, bool innerFlag) {
    // adjusts the calculation based on if inner wheel or outer wheel
    int wheelInt = (innerFlag) ? 1 : -1;

    // Finding the angles of the leading wheels
    float wheelAngle
        = atan(2 * m_drive->getLength() * sin(headingAngle)
               / (2 * m_drive->getLength() * cos(headingAngle)
                  - wheelInt * m_drive->getWidth() * sin(headingAngle)));

    return TO_DEGREES(wheelAngle);
}

int DriveModel::getLastRadialOutput() { return m_lastRadialOutput; }

void DriveModel::setLastRadialOutput(int value) { m_lastRadialOutput = value; }
