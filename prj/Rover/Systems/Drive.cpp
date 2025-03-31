#include "Drive.h"

Drive::m_length = 1;
Drive::m_width = 0.5;

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
