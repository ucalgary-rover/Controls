#include "MockDrive.h"

static const char* file = "MockDrive";

MockDrive::MockDrive() { }

MockDrive::~MockDrive() { }

void MockDrive::setWheelAngle(WheelID wheel, float angle) {
    steer[wheel] = angle;
}

double MockDrive::getWheelAngle(WheelID wheelIndex) {
    return steer[wheelIndex];
}

void MockDrive::setWheelSpeed(WheelID wheel, float speed) {
    drive[wheel] = speed;
}

double MockDrive::getWheelSpeed(WheelID wheelIndex) {
    return drive[wheelIndex];
}

float MockDrive::getLength() { return m_length; }

float MockDrive::getWidth() { return m_width; }
