#include "Drive.h"

static void CCONV onAttachHandler(PhidgetHandle ph, void* ctx);
static T setAddressProperties(T* handle, int serialNum, int channel);

Drive::Drive(float width, float length) : m_width(width), m_length(length) {
    m_handlesDC = new PhidgetDCMotorHandle*[WHEEL_COUNT]();
    m_handlesStepper = new PhidgetStepperHandle*[WHEEL_COUNT]();

    for (int dc = 0; dc < WHEEL_COUNT; dc++) {
        PhidgetDCMotor_create(m_handlesDC[dc]);
        setAddressProperties<PhidgetDCMotorHandle>(
            m_handlesDC[dc], DRIVE_DC_SERIAL_NUMBER[motor],
            DRIVE_DC_CHANNEL[dc]);
    }

    for (int stepper = 0; stepper < WHEEL_COUNT; stepper++) {
        PhidgetStepper_create(m_handlesStepper[stepper]);
        setAddressProperties<PhidgetStepperHandle>(
            m_handlesStepper[stepper], DRIVE_STEPPER_SERIAL_NUMBER[stepper],
            DRIVE_STEPPER_CHANNEL[stepper]);
    }
}

Drive::~Drive() {
    for (int dc = 0; dc < WHEEL_COUNT; dc++) {
        Phidget_close((PhidgetHandle)*m_handlesDC[dc]);
        PhidgetDCMotor_delete(m_handlesDC[dc]);
    }

    for (int stepper = 0; stepper < WHEEL_COUNT; stepper++) {
        Phidget_close((PhidgetHandle)*m_handlesStepper[stepper]);
        PhidgetStepper_delete(m_handlesStepper[stepper]);
    }
}

bool Drive::getDriveDCHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < WHEEL_COUNT) {
        retVal->type = DC_MOTOR;
        retVal->dcMoto = m_handlesDC[index];
        return true;
    }
    retVal->type = INVALID_MOTOR_TYPE;
    return false;
}

bool Drive::getDriveStepperHandle(MotorHandlerReturn* retVal, int index) {
    if (index >= 0 && index < WHEEL_COUNT) {
        retVal->type = STEPPER_MOTOR;
        retVal->dcMoto = m_handlesStepper[index];
        return true;
    }
    retVal->type = INVALID_MOTOR_TYPE;
    return false;
}

float Drive::getLength() { return m_length; }

float Drive::getWidth() { return m_width; }

// Declare the event handler
static void CCONV onAttachHandler(PhidgetHandle ph, void* ctx) {
    // You can access the Phidget that fired the event by using the first
    // parameter of the event handler
    int deviceSerialNumber;
    Phidget_getDeviceSerialNumber(ph, &deviceSerialNumber);
    std::cout << "Device: " << deviceSerialNumber << "Has attached";
}

static template <typename T>
void setAddressProperties(T* handle, int serialNum, int channel) {
    Phidget_setDeviceSerialNumber((PhidgetHandle)*handle, serialNum);
    Phidget_setIsHubPortDevice((PhidgetHandle)*handle, 1);
    Phidget_setHubPort((PhidgetHandle)*handle, channel);
    Phidget_setOnAttachHandler((PhidgetHandle)*handle, onAttachHandler, NULL);
    Phidget_openWaitForAttachment((PhidgetHandle)*handle, 5000);
}