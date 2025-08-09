#include "Drive.h"

static const char* file = "Drive";

Drive::Drive(float width, float length) : m_width(width), m_length(length) {

    // Create handles for DC motors
    PhidgetReturnCode res;

    // initialise DC motors
    Logging::logD(file, "Initialising Drive DC Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        m_handlesDC.push_back(PhidgetBLDCMotorHandle());
        res = PhidgetBLDCMotor_create(&m_handlesDC.at(dc));
        Logging::logD(file, "PhidgetBLDCMotor_create returned %d", res);
        setAddressProperties<PhidgetBLDCMotorHandle>(
            &m_handlesDC.at(dc), DRIVE_DC_SERIAL_NUMBER[dc],
            DRIVE_DC_CHANNEL[dc], DRIVE_DC_PORT[dc]);
    }

    // initialise stepper motors
    Logging::logD(file, "Initialising Drive Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        m_handlesStepper.push_back(PhidgetStepperHandle());
        PhidgetStepper_create(&m_handlesStepper.at(stepper));
        setAddressProperties<PhidgetStepperHandle>(
            &m_handlesStepper.at(stepper), DRIVE_STEPPER_SERIAL_NUMBER[stepper],
            DRIVE_STEPPER_CHANNEL[stepper], DRIVE_STEPPER_PORT[stepper]);
        PhidgetStepper_setRescaleFactor(m_handlesStepper.at(stepper),
                                        1.8 / (16 * 77));
        PhidgetStepper_setEngaged(m_handlesStepper.at(stepper), 1);
    }

    // initialise digital inputs
    Logging::logD(file, "Initialising Drive Digital Inputs");
    for (int digitalInput = 0; digitalInput < DRIVE_INDEX_WHEEL_COUNT;
         digitalInput++) {
        m_handlesDigitalInput.push_back(PhidgetDigitalInputHandle());
        PhidgetDigitalInput_create(&m_handlesDigitalInput.at(digitalInput));
        setAddressProperties<PhidgetDigitalInputHandle>(
            &m_handlesDigitalInput.at(digitalInput),
            DRIVE_DIGITAL_INPUT_SERIAL_NUMBER,
            DRIVE_DIGITAL_INPUT_CHANNEL[digitalInput],
            DRIVE_DIGITAL_INPUT_PORT);
    }
}

Drive::~Drive() {
    // deinitialise DC motors
    Logging::logD(file, "Deinitialising Drive Motors");
    for (int dc = 0; dc < DRIVE_INDEX_WHEEL_COUNT; dc++) {
        Phidget_close((PhidgetHandle)m_handlesDC.at(dc));
        PhidgetBLDCMotor_delete(&m_handlesDC.at(dc));
    }

    // deinitialise stepper motors
    Logging::logD(file, "Deinitialising Drive Stepper Motors");
    for (int stepper = 0; stepper < DRIVE_INDEX_WHEEL_COUNT; stepper++) {
        Phidget_close((PhidgetHandle)m_handlesStepper.at(stepper));
        PhidgetStepper_delete(&m_handlesStepper.at(stepper));
    }

    // deinitialise digital inputs
    Logging::logD(file, "Deinitialising Drive Digital Inputs");
    for (int digitalInput = 0; digitalInput < DRIVE_INDEX_WHEEL_COUNT;
         digitalInput++) {
        Phidget_close((PhidgetHandle)m_handlesDigitalInput.at(digitalInput));
        PhidgetDigitalInput_delete(&m_handlesDigitalInput.at(digitalInput));
    }
}

bool Drive::getDriveDCHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_DC_MOTOR;
        retVal->handler.dcMotor = &m_handlesDC.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

bool Drive::getDriveStepperHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < DRIVE_INDEX_WHEEL_COUNT) {
        retVal->type = MOTOR_TYPE_STEPPER_MOTOR;
        retVal->handler.stepperMotor = &m_handlesStepper.at(index);
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
        retVal->handler.digitalInput = &m_handlesDigitalInput.at(index);
        return true;
    }
    retVal->type = MOTOR_TYPE_INVALID;
    return false;
}

float Drive::getLength() { return m_length; }

float Drive::getWidth() { return m_width; }
