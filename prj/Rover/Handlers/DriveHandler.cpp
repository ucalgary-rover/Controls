// Schulich Space Program Controls Software Division (2025)
// Written by Johan Esperida

#include "DriveHandler.h"

const char* file = "DriveHandler";

// Various hardcoded values subject to change for fine tuning some movement
// functions

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
-----------------> waiting for the wheels to finish turning still needs to be
programmed
- Probably make some function "awaitWheelTargets" that waits until targets hit

*/
static void CCONV onButtonPressedHandler(PhidgetDigitalInputHandle pdih,
                                         void* ctx, int state);

const float PI = 3.14;

// two decimal places
#define TO_RADIANS(value) round(value* PI / 180 * 100) / 100

// two decimal places
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

// Constructor
DriveHandler::DriveHandler(Drive* drive, MessageQueue* driveQueue) {

    // Reference to the Arm object in Rover.cpp
    m_drive = drive;

    // Create a driveQueue object in this file that references the driveQueue in
    // Rover.cpp (NOT A COPY!!!) This allows us to use the same driveQueue in
    // this file without shenanigans
    m_driveQueue = driveQueue;

    // Open the file to store angles
    std::ifstream angleFile(BASE_LAST_KNOWN_POS_FILE, std::ifstream::binary);
    if (!angleFile.is_open()) {
        Logging::logE(file, "Failed to open angle storage file.");
    }
    Json::CharReaderBuilder readerBuilder;
    std::string errs;
    Json::parseFromStream(readerBuilder, angleFile, &m_lastKnownPos, &errs);
    angleFile.close();
}

void DriveHandler::turnWheel(DriveIndex wheel, int angle) {

    // Update the last known position of the wheel in the JSON file
    m_lastKnownPos[driveIndexToString(wheel)] = angle;
    std::ofstream angleFile(BASE_LAST_KNOWN_POS_FILE);
    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> jsonWriter(
        writerBuilder.newStreamWriter());
    jsonWriter->write(m_lastKnownPos, &angleFile);
    angleFile.close();

    // check if the turn is larger than 10 degrees
    int64_t currentPos;
    double currentStepperPos;
    MotorHandlerReturn motorStuct;

    // get the encoder handler
    m_drive->getDriveEncoderHandle(&motorStuct, wheel);

    PhidgetEncoder_getPosition(*motorStuct.handler.encoder, &currentPos);

    double currentAngle = (currentPos / MAX_ENCODER_POSITIONS) * 360;

    if (abs(currentPos - angle) > 10) {
        stopWheels();
    }

    // get the stepper handler
    m_drive->getDriveStepperHandle(&motorStuct, wheel);

    PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                               &currentStepperPos);
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     currentStepperPos - currentAngle);

    // turn the wheel
    // can make this async with PhidgetStepper_setTargetPosition_async
    PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor, angle);
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
    speedAdjust = reportedAngle / MAX_SPEED_ANGLE;

    // prevents value from going above maximum by rounding down to 1
    speedAdjust = (std::abs(speedAdjust) > 1) ? (int)speedAdjust : speedAdjust;

    // calculates speed based on the adjustment with the sign determining the
    // direction
    float speed = speedAdjust * SPOT_SPEED_MAX;

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
        = TO_DEGREES(atan(m_drive->getWidth() / m_drive->getLength()));

    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {

        // checking for angle to turn each wheel so they rotate properly

        if (stepper == 0 || stepper == 3) {
            turnWheel((DriveIndex)stepper, -wheelAngle);

        } else {
            turnWheel((DriveIndex)stepper, wheelAngle);
        }
    }

    // spin wheels
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        // checking for direction to spin each wheel

        // if index odd the wheel is on the right of chassis
        if (dc % 2 == 1) {
            PhidgetDCMotor_setTargetVelocity(*motorStuct.handler.dcMotor,
                                             rightSpeed);

            // if index is even the wheel is on the left of chassis
        } else {
            PhidgetDCMotor_setTargetVelocity(*motorStuct.handler.dcMotor,
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
            m_drive->getDriveStepperHandle(&motorStuct, stepper);

            PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
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
    float speed = stickMagnitude / 100 * direction * STRAFE_SPEED_MAX;

    // Turn
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveStepperHandle(&motorStuct, stepper);

        turnWheel((DriveIndex)stepper, wheelAngle);
    }

    // Spin
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.handler.dcMotor, speed);
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
        reportedAngle = (stickAngle / 90) * RADIAL_ANGLE_MAX;

        // corresponding to a right turn (note the negative reported angle
        // value)
    } else if (stickAngle > 90) {
        reportedAngle = ((stickAngle - 180) / 90) * RADIAL_ANGLE_MAX;

        // max positions
    } else if (stickAngle = 90) {
        reportedAngle = RADIAL_ANGLE_MAX * m_lastRadialOutput;

        // neutral positions
    } else if (stickAngle == 0) {
        reportedAngle = 0;
    }

    // set new lastRadialOutput
    // also use this to determine direction later (negative is right turn)
    m_lastRadialOutput = (reportedAngle > 0) ? 1 : -1;

    // must convert to radians for trig functions
    float headingAngle = TO_RADIANS(std::abs(reportedAngle));

    return headingAngle;
}

float DriveHandler::radialTurnWheelAngles(int headingAngle, bool innerFlag) {
    // adjusts the calculation based on if inner wheel or outer wheel
    int wheelInt = (innerFlag) ? 1 : -1;

    // Finding the angles of the leading wheels
    float wheelAngle
        = atan(2 * m_drive->getLength() * sin(headingAngle)
               / (2 * m_drive->getLength() * cos(headingAngle)
                  - wheelInt * m_drive->getWidth() * sin(headingAngle)));

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
        turnWheel((DriveIndex)i, wheelAngles[i]);
    }

    // Spin wheels

    // direction based on left joystick angle
    int direction = (stickTheta <= 90 || stickTheta >= 270) ? 1 : -1;

    // deriving speed from magnitude (percentage of radius of joystick circle)
    float speed = stickMagnitude / 100 * direction * RADIAL_SPEED_MAX;

    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.handler.dcMotor, speed);
    }
}

void DriveHandler::stopWheels() {
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        PhidgetDCMotor_setTargetVelocity(*motorStuct.handler.dcMotor, 0);
    }
}

void DriveHandler::start() {

    WheelMessage message;

    while (true) {
        // Get message from driveQueue
        message = std::get<WheelMessage>(m_driveQueue->pop().get_payload());

        // flags for if the value is non-zero
        bool angleVelocityFlag
            = message.angleVelocity != 0;          // right joystick angle
        bool velocityFlag = message.velocity != 0; // left joystick magnitude

        // for if the movement is forward or backward
        // left joystick angle
        bool thetaFlag = (message.theta < 10 && message.theta > 350)
                         || (message.theta < 190 && message.theta > 170);

        // Scenarios

        // Radial turn (angular velocity and velocity forwards or backwards)
        if (angleVelocityFlag && thetaFlag) {
            m_spotTurnFlag = false;
            radialTurn(message.angleVelocity, message.velocity, message.theta);

            // Spot turning (angular velocity)
        } else if (angleVelocityFlag) {
            spotTurn(message.angleVelocity);

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

void DriveHandler::calibrateWheel(DriveIndex wheel) {
    // This function should be called before the rover is used to ensure
    // the wheels are facing the correct direction

    // 1 for clockwise, -1 for counterclockwise
    int direction = 1;
    MotorHandlerReturn motorStuct;
    bool buttonPressed = 0;
    int changes[] = { 10, 4, 1 };

    int lastKnownAngle = m_lastKnownPos[driveIndexToString(wheel)].asInt();

    m_drive->getDriveDigitalInputHandle(&motorStuct, wheel);
    PhidgetDigitalInput_setOnStateChangeHandler(
        *motorStuct.handler.digitalInput, onButtonPressedHandler,
        &buttonPressed);

    // Get the current position of the wheel
    m_drive->getDriveStepperHandle(&motorStuct, wheel);
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -lastKnownAngle);
    for (int i = 0; i < sizeof(changes) / sizeof(changes[0]); i++) {
        int angleGoal = 0;
        while (!buttonPressed) {
            angleGoal += changes[i] * direction;

            PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                             angleGoal);

            if (buttonPressed) {
                buttonPressed = 0;
                break;
            }

            if (abs(angleGoal) > 100) {
                Logging::logE(file,
                              "Calibration failed: angle goal exceeded limit.");
                exit(1);
            }

            if (i = 0) {
                direction * -1;
                angleGoal *= direction;
            }
        }
        direction * -1;
    }

    double currentStepperPos;
    PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                               &currentStepperPos);

    // Reset the position offset to zero
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -currentStepperPos);
}

static void CCONV onButtonPressedHandler(PhidgetDigitalInputHandle pdih,
                                         void* ctx, int state) {

    int* myIntPtr = (int*)ctx;
    *myIntPtr = 1;
}
