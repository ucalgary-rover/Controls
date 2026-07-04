#include "DriveHardware.h"
#define GEAR_RATIO 77

static const char* file = "DriveHardware";

DriveHardware::DriveHardware() {

    // Create handles for DC motors
    PhidgetReturnCode res;

    // initialise DC motors
    Logging::logD(file, "Initialising DriveHardware DC Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        m_handlesDC.push_back(PhidgetBLDCMotorHandle());
        res = PhidgetBLDCMotor_create(&m_handlesDC.at(dc));
        Logging::logD(file, "PhidgetBLDCMotor_create returned %d", res);
        setAddressProperties<PhidgetBLDCMotorHandle>(
            &m_handlesDC.at(dc), DRIVE_DC_SERIAL_NUMBER[dc],
            DRIVE_DC_CHANNEL[dc], DRIVE_DC_PORT[dc]);
    }

    // initialise stepper motors
    Logging::logD(file, "Initialising DriveHardware Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        m_handlesStepper.push_back(PhidgetStepperHandle());
        PhidgetStepper_create(&m_handlesStepper.at(stepper));
        setAddressProperties<PhidgetStepperHandle>(
            &m_handlesStepper.at(stepper), DRIVE_STEPPER_SERIAL_NUMBER[stepper],
            DRIVE_STEPPER_CHANNEL[stepper], DRIVE_STEPPER_PORT[stepper]);
        PhidgetStepper_setRescaleFactor(m_handlesStepper.at(stepper),
                                        1.8 / (16 * GEAR_RATIO));
        PhidgetStepper_setEngaged(m_handlesStepper.at(stepper), 1);
    }

    // initialise encoders for wheel zero tracking
    Logging::logD(file, "Initialising DriveHardware Encoders");
    for (int driveEncoder = 0; driveEncoder < DRIVE_INDEX_WHEEL_COUNT;
         driveEncoder++) {
        m_handlesDriveEncoder.push_back(PhidgetEncoderHandle());
        PhidgetEncoder_create(&m_handlesDriveEncoder.at(driveEncoder));
        setAddressProperties<PhidgetEncoderHandle>(
            &m_handlesDriveEncoder.at(driveEncoder),
            DRIVE_ENCODER_SERIAL_NUMBER[driveEncoder],
            DRIVE_ENCODER_CHANNEL[driveEncoder],
            DRIVE_ENCODER_PORT[driveEncoder]);
    }
}

DriveHardware::~DriveHardware() {
    // deinitialise DC motors
    Logging::logD(file, "Deinitialising DriveHardware Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        Phidget_close((PhidgetHandle)m_handlesDC.at(dc));
        PhidgetBLDCMotor_delete(&m_handlesDC.at(dc));
    }

    // deinitialise stepper motors
    Logging::logD(file, "Deinitialising DriveHardware Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        Phidget_close((PhidgetHandle)m_handlesStepper.at(stepper));
        PhidgetStepper_delete(&m_handlesStepper.at(stepper));
    }

    // deinitialise drive encoders
    Logging::logD(file, "Deinitialising DriveHardware Encoders");
    for (int driveEncoder = 0; driveEncoder < DRIVE_INDEX_WHEEL_COUNT;
         driveEncoder++) {
        Phidget_close((PhidgetHandle)m_handlesDriveEncoder.at(driveEncoder));
        PhidgetEncoder_delete(&m_handlesDriveEncoder.at(driveEncoder));
    }
}

bool DriveHardware::getDriveDCHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_BLDC_MOTOR;
        retVal->handler.bldcMotor = &m_handlesDC.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool DriveHardware::getDriveStepperHandle(MotorHandlerReturn* retVal,
                                          int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->handler.stepperMotor = &m_handlesStepper.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool DriveHardware::getDriveEncoderHandle(MotorHandlerReturn* retVal,
                                          int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_ENCODER;
        retVal->handler.encoder = &m_handlesDriveEncoder.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

static void CCONV onButtonPressedHandler(PhidgetEncoderHandle pdih, void* ctx,
                                         int state) {
    // int* myIntPtr = (int*)ctx;
    // *myIntPtr = 1;
}

static void CCONV onAngleReached(PhidgetHandle pdih, void* ctx,
                                 PhidgetReturnCode returnCode) {
    //*(bool*)ctx = true;
}

static void CCONV setTargetVelocityDone(PhidgetHandle phid, void* ctx,
                                        PhidgetReturnCode res) {
    // Async call completed
}

void DriveHardware::setWheelAngle(DriveIndex wheel, float angle) {
    MotorHandlerReturn motorStuct;
    getDriveStepperHandle(&motorStuct, wheel);

    // can make this async with PhidgetStepper_setTargetPosition_async
    PhidgetStepper_setTargetPosition_async(*motorStuct.handler.stepperMotor,
                                           angle, onAngleReached, nullptr);
}

double DriveHardware::getWheelAngle(DriveIndex wheelIndex) {
    double steer;
    MotorHandlerReturn motorStuctStepper;
    getDriveStepperHandle(&motorStuctStepper, wheelIndex);
    PhidgetStepper_getPosition(*motorStuctStepper.handler.stepperMotor,
                               &(steer));
    return steer;
}

void DriveHardware::setWheelSpeed(DriveIndex wheel, float speed) {
    MotorHandlerReturn motorStuct;
    getDriveDCHandle(&motorStuct, wheel);

    // can make this async with PhidgetBLDCMotor_setTargetVelocity_async
    PhidgetBLDCMotor_setTargetVelocity_async(
        *motorStuct.handler.bldcMotor, speed, setTargetVelocityDone, nullptr);
}

double DriveHardware::getWheelSpeed(DriveIndex wheelIndex) {
    double velocity;
    MotorHandlerReturn motorStuctDC;
    getDriveDCHandle(&motorStuctDC, wheelIndex);
    PhidgetBLDCMotor_getTargetVelocity(*motorStuctDC.handler.bldcMotor,
                                       &(velocity));

    return velocity;
}

float DriveHardware::getLength() { return m_length; }

float DriveHardware::getWidth() { return m_width; }
