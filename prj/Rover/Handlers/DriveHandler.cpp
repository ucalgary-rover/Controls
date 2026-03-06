// Schulich Space Program Controls Software Division (2025)
// Written by Johan Esperida

#include "DriveHandler.h"
#include "Message.h"
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
static void CCONV onButtonPressedHandler(PhidgetDigitalInputHandle pdih,
                                         void* ctx, int state);
static void CCONV onAngleReached(PhidgetHandle pdih, void* ctx,
                                 PhidgetReturnCode returnCode);

static void CCONV setTargetVelocityDone(PhidgetHandle phid, void* ctx,
                                        PhidgetReturnCode res);

const float PI = 3.14;

// two decimal places
#define TO_RADIANS(value) round(value* PI / 180 * 100) / 100

// two decimal places
#define TO_DEGREES(value) round(value * 180 / PI * 100) / 100

// Constructor
DriveHandler::DriveHandler(Drive* drive,
                           DriveMotorStateManager* driveMotorStateManager) {

    // Reference to the Arm object in Rover.cpp
    m_drive = drive;

    // Create a driveMotorStateManager object in this file that references the
    // driveMotorStateManager in Rover.cpp (NOT A COPY!!!) This allows us to use
    // the same driveMotorStateManager in this file without shenanigans
    m_driveMotorStateManager = driveMotorStateManager;

    // Open the file to store angles
    std::ifstream angleFile(BASE_LAST_KNOWN_POS_FILE, std::ifstream::binary);
    if (!angleFile.is_open()) {
        Logging::logE(file, "Failed to open angle storage file.");
    }
    Json::CharReaderBuilder readerBuilder;
    std::string errs;
    Json::parseFromStream(readerBuilder, angleFile, &m_lastKnownPos, &errs);
    angleFile.close();

    // Calibrate all wheels to face forward
    // for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
    //     calibrateWheel((DriveIndex)i);
    // }
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

    MotorHandlerReturn motorStuct;
    m_drive->getDriveStepperHandle(&motorStuct, wheel);

    double currentStepperPos;

    PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                               &currentStepperPos);

    if (abs(currentStepperPos - angle) > WHEEL_TURN_THRESH) {
        m_wheelTargetStatuses[wheel] = false;
        Logging::logD(file,
                      "Stopping wheels because turning threshold is too large");
        stopWheels();
    }

    Logging::logD(file, "angle: %d, wheel: %d", angle, wheel);

    // turn the wheel
    // can make this async with PhidgetStepper_setTargetPosition_async
    PhidgetStepper_setTargetPosition_async(*motorStuct.handler.stepperMotor,
                                           angle, onAngleReached,
                                           &m_wheelTargetStatuses[wheel]);
}

float DriveHandler::spotTurnSpeed(int stickAngle) {
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

void DriveHandler::spotTurn(int stickAngle) {

    // deriving speed based on angle of right stick
    float speed = spotTurnSpeed(stickAngle);

    // turn wheels

    // if moving into spot turn configuration, prevents wheels from spinning
    // if (!m_spotTurnFlag) {

    //     stopWheels(); // Ensure the wheels wait to finish turning
    //                   //  <---------------------
    // }

    // Angle of wheel direction based on length and width
    float wheelAngle
        = TO_DEGREES(atan(m_drive->getLength() / m_drive->getWidth()));

    // Turn the wheels
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {

        // checking for angle to turn each wheel so they rotate properly

        if (stepper == DRIVE_INDEX_FRONT_LEFT
            || stepper == DRIVE_INDEX_BACK_RIGHT) {
            turnWheel((DriveIndex)stepper, -wheelAngle);

        } else if (stepper == DRIVE_INDEX_FRONT_RIGHT
                   || stepper == DRIVE_INDEX_BACK_LEFT) {
            turnWheel((DriveIndex)stepper, wheelAngle);
        }
    }

    awaitWheelTargets();

    // spin wheels
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);
        PhidgetReturnCode res;

        PhidgetBLDCMotor_setTargetVelocity_async(
            *motorStuct.handler.bldcMotor, -speed, setTargetVelocityDone, NULL);
        Logging::logD(file, "Setting DC motor %d to speed %f", dc, -speed);
    }
}

int DriveHandler::strafeAngleAdjust(int stickTheta) {
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
    } else {
        Logging::logE(file, "AAAHHHHHHHHHH: %d", stickTheta);
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
        stickTheta = (stickTheta == 270) ? -90 : 90;

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
    float speed = (float)stickMagnitude / 100 * direction * STRAFE_SPEED_MAX;

    // Turn
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        turnWheel((DriveIndex)stepper, wheelAngle);
    }

    awaitWheelTargets();

    // Spin
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        if (dc % 2 == 0) {
            PhidgetBLDCMotor_setTargetVelocity_async(
                *motorStuct.handler.bldcMotor, speed, setTargetVelocityDone,
                NULL);
        } else {
            PhidgetBLDCMotor_setTargetVelocity_async(
                *motorStuct.handler.bldcMotor, -speed, setTargetVelocityDone,
                NULL);
        }
    }
}

float DriveHandler::radialTurnHeadingAngle(int stickAngle) {

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

float DriveHandler::radialTurnWheelAngles(float headingAngle, bool innerFlag) {
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
    Logging::logD(file, "Radial turn with angle %d, magnitude %d, theta %d",
                  stickAngle, stickMagnitude, stickTheta);

    // Only valid angles are between 0 and 90 and 270 and 360

    if (stickAngle < 0 || stickAngle > 360) {
        Logging::logE(file, "Invalid stick angle: %d", stickAngle);
        return;
    }

    if (stickAngle >= 90 && stickAngle <= 270) {
        Logging::logE(file, "Invalid stick angle for radial turn: %d",
                      stickAngle);
        return;
    }

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

    for (int stepper = 0; stepper < 2; stepper++) {
        turnWheel((DriveIndex)stepper, wheelAngles[stepper]);
    }

    // make sure back wheels are straight
    for (int stepper = 2; stepper < 4; stepper++) {
        turnWheel((DriveIndex)stepper, 0);
    }

    awaitWheelTargets();

    // Spin wheels

    // direction based on left joystick angle
    int direction = (stickTheta <= 90 || stickTheta >= 270) ? 1 : -1;

    // deriving speed from magnitude (percentage of radius of joystick circle)
    float speed = (float)stickMagnitude / 100 * direction * RADIAL_SPEED_MAX;

    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        if (dc % 2 == 0) {
            PhidgetBLDCMotor_setTargetVelocity_async(
                *motorStuct.handler.bldcMotor, speed, setTargetVelocityDone,
                NULL);
        } else {
            PhidgetBLDCMotor_setTargetVelocity_async(
                *motorStuct.handler.bldcMotor, -speed, setTargetVelocityDone,
                NULL);
        }
    }
}

void DriveHandler::stopWheels() {
    // sets target velocities of all wheels to zero
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        // get the handler
        MotorHandlerReturn motorStuct;
        m_drive->getDriveDCHandle(&motorStuct, dc);

        PhidgetBLDCMotor_setTargetVelocity_async(
            *motorStuct.handler.bldcMotor, 0, setTargetVelocityDone, NULL);
    }

    // wait for an amount of seconds to let the wheels slow down
    usleep(STOP_WHEEL_WAIT * 1000000);
}

void DriveHandler::awaitWheelTargets() {
    // is true only when all wheel targets are true
    bool overallStatus = false;

    while (!overallStatus) {
        // start comparing all the values
        overallStatus = m_wheelTargetStatuses[0];

        for (int i = 1; i < 4; i++) {
            overallStatus = overallStatus && m_wheelTargetStatuses[i];
        }
    }
    Logging::logD(file, "Exiting await wheel targets");
}

void DriveHandler::start() {

    Message message;

    while (true) {
        // Get message from driveQueue
        // Message msg = m_driveQueue->pop();

        // TODO: Handle the message

        /*if (msg.getFormat() != MessageFormat::MESSAGE_FORMAT_WHEEL) {
            Logging::logE(file, "Received non-wheel message in driveQueue %d",
                          msg.getFormat());
            continue;
        }

        msg.printMessage();

        message = std::get<WheelMessage>(msg.get_payload());

        // flags for if the value is non-zero
        bool angleVelocityFlag
            = message.angleVelocity > 0;           // right joystick angle
        bool velocityFlag = message.velocity != 0; // left joystick magnitude

        // for if the movement is forward or backward
        // left joystick angle
        bool longitudinalVelocityOnly
            = (message.theta == 0) || (message.theta == 180);

        bool lateralVelocityOnly
            = (message.theta == 90) || (message.theta == 270);

        // We're going to disable superimposed radial + strafing turning
        // For now radial turning should only work when strafing velocity is 0
        // And strafing should only work when angular velocity is 0

        // Radial turn (angular velocity and velocity forwards or backwards)
        if (!velocityFlag && !angleVelocityFlag) {
            stopWheels();
        } else if (angleVelocityFlag && longitudinalVelocityOnly
                   && velocityFlag) {
            Logging::logD(file, "Radial turn");
            m_spotTurnFlag = false;
            radialTurn(message.angleVelocity, message.velocity, message.theta);

            // Spot turning (angular velocity)
        } else if (angleVelocityFlag && !velocityFlag) {
            Logging::logD(file, "Spot turn");
            spotTurn(message.angleVelocity);

            // Strafing (angle and velocity)
        } else if (velocityFlag
                   && (longitudinalVelocityOnly || lateralVelocityOnly)
                   && !angleVelocityFlag) {
            Logging::logD(file, "Strafing");
            m_spotTurnFlag = false;
            strafe(message.theta, message.velocity);

            // execute stop if no joysticks active
        } else {
            stopWheels();
        }*/
    }
}

void DriveHandler::calibrateWheel(DriveIndex wheel) {
    // This function should be called before the rover is used to ensure
    // the wheels are facing the correct direction

    // 1 for clockwise, -1 for counterclockwise
    int calibrationDirection = 1;
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
        int state;
        PhidgetDigitalInput_getState(*motorStuct.handler.digitalInput, &state);
        if (state == 1) {
            Logging::logI(file,
                          "Button already pressed, skipping calibration.");
            return;
        }

        int angleGoal = 0;
        while (true) {
            angleGoal = (changes[i] + abs(angleGoal)) * calibrationDirection;
            Logging::logI(file,
                          "Calibrating change %d to angle %d direction %d",
                          changes[i], angleGoal, calibrationDirection);

            PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                             angleGoal);

            while (true) {
                double position;
                PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                                           &position);
                if ((position >= angleGoal - 0.5)
                    && (position <= angleGoal + 0.5)) {
                    break;
                }
                usleep(10000); // Sleep for 10ms to avoid busy waiting
            }

            if (buttonPressed) {
                buttonPressed = 0;
                break;
            }

            if (abs(angleGoal) > 100) {
                Logging::logE(file,
                              "Calibration failed: angle goal exceeded limit.");
                exit(1);
            }

            if (i == 0) {
                calibrationDirection *= -1;
            }
        }
        calibrationDirection *= -1;
    }

    // Reset the position of the stepper motor to 0
    double currentStepperPos;
    PhidgetStepper_getPosition(*motorStuct.handler.stepperMotor,
                               &currentStepperPos);
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -currentStepperPos);
}

static void CCONV onButtonPressedHandler(PhidgetDigitalInputHandle pdih,
                                         void* ctx, int state) {
    int* myIntPtr = (int*)ctx;
    *myIntPtr = 1;
}

static void CCONV onAngleReached(PhidgetHandle pdih, void* ctx,
                                 PhidgetReturnCode returnCode) {
    *(bool*)ctx = true;
}

static void CCONV setTargetVelocityDone(PhidgetHandle phid, void* ctx,
                                        PhidgetReturnCode res) {

    // Async call completed
}