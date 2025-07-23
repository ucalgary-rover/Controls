#include "Drive.h"

static const char* file = "Drive";

Drive::Drive(float width, float length) : m_width(width), m_length(length) {
    m_handlesDC = new PhidgetDCMotorHandle*[DRIVE_INDEX_WHEEL_COUNT]();
    m_handlesStepper = new PhidgetStepperHandle*[DRIVE_INDEX_WHEEL_COUNT]();
    m_handlesDigitalInput
        = new PhidgetDigitalInputHandle*[DRIVE_INDEX_WHEEL_COUNT]();
    m_handlesEncoder = new PhidgetEncoderHandle*[DRIVE_INDEX_WHEEL_COUNT]();

    // initialise DC motors
    Logging::logD(file, "Initialising Drive DC Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        PhidgetDCMotor_create(m_handlesDC[dc]);
        setAddressProperties<PhidgetDCMotorHandle>(
            m_handlesDC[dc], DRIVE_DC_SERIAL_NUMBER[dc], DRIVE_DC_CHANNEL[dc]);
    }

    // initialise stepper motors
    Logging::logD(file, "Initialising Drive Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        PhidgetStepper_create(m_handlesStepper[stepper]);
        setAddressProperties<PhidgetStepperHandle>(
            m_handlesStepper[stepper], DRIVE_STEPPER_SERIAL_NUMBER[stepper],
            DRIVE_STEPPER_CHANNEL[stepper]);
    }

    // initialise digital inputs
    Logging::logD(file, "Initialising Drive Digital Inputs");
    for (int digitalInput = 0; digitalInput < DRIVE_INDEX_WHEEL_COUNT;
         digitalInput++) {
        PhidgetDigitalInput_create(m_handlesDigitalInput[digitalInput]);
        setAddressProperties<PhidgetDigitalInputHandle>(
            m_handlesDigitalInput[digitalInput],
            DRIVE_DIGITAL_INPUT_SERIAL_NUMBER[digitalInput],
            DRIVE_DIGITAL_INPUT_CHANNEL[digitalInput]);
    }

    // initialise encoders
    Logging::logD(file, "Initialising Drive Encoders");
    for (int encoder = 0; encoder < DRIVE_INDEX_WHEEL_COUNT; encoder++) {
        PhidgetEncoder_create(m_handlesEncoder[encoder]);
        setAddressProperties<PhidgetEncoderHandle>(
            m_handlesEncoder[encoder], DRIVE_ENCODER_SERIAL_NUMBER[encoder],
            DRIVE_ENCODER_CHANNEL[encoder]);
    }
}

Drive::~Drive() {
    // deinitialise DC motors
    Logging::logD(file, "Deinitialising Drive Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        Phidget_close((PhidgetHandle)*m_handlesDC[dc]);
        PhidgetDCMotor_delete(m_handlesDC[dc]);
    }

    // deinitialise stepper motors
    Logging::logD(file, "Deinitialising Drive Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        Phidget_close((PhidgetHandle)*m_handlesStepper[stepper]);
        PhidgetStepper_delete(m_handlesStepper[stepper]);
    }

    // deinitialise digital inputs
    Logging::logD(file, "Deinitialising Drive Digital Inputs");
    for (int digitalInput = 0; digitalInput < DRIVE_INDEX_WHEEL_COUNT;
         digitalInput++) {
        Phidget_close((PhidgetHandle)*m_handlesDigitalInput[digitalInput]);
        PhidgetDigitalInput_delete(m_handlesDigitalInput[digitalInput]);
    }

    // deinitialise encoders
    Logging::logD(file, "Deinitialising Drive Encoders");
    for (int encoder = 0; encoder < DRIVE_INDEX_WHEEL_COUNT; encoder++) {
        Phidget_close((PhidgetHandle)*m_handlesEncoder[encoder]);
        PhidgetEncoder_delete(m_handlesEncoder[encoder]);
    }
}

bool Drive::getDriveDCHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->handler.dcMotor = m_handlesDC[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Drive::getDriveStepperHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->handler.stepperMotor = m_handlesStepper[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Drive::getDriveDigitalInputHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type
            = MOTOR_TYPE_DIGITAL_INPUT; // Assuming digital input is treated as
                                        // servo for this context
        retVal->handler.digitalInput = m_handlesDigitalInput[index];
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Drive::getDriveEncoderHandle(MotorHandlerReturn* retVal, int index) {
    // Assuming encoder is handled similarly to digital input
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_ENCODER; // Assuming encoder is treated as
                                           // servo for this context
        retVal->handler.encoder
            = m_handlesEncoder[index]; // Placeholder for actual encoder handler
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

float Drive::getLength() { return m_length; }

float Drive::getWidth() { return m_width; }
