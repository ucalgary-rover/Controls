#include "MockDrive.h"

static const char* file = "MockDrive";

MockDrive::MockDrive() { }

MockDrive::~MockDrive() { }

void MockDrive::setWheelAngle(DriveIndex wheel, float angle) {
    steer[wheel] = angle;
}

double MockDrive::getWheelAngle(DriveIndex wheelIndex) {
    return steer[wheelIndex];
}

void MockDrive::setWheelSpeed(DriveIndex wheel, float speed) {
    drive[wheel] = speed;
}

double MockDrive::getWheelSpeed(DriveIndex wheelIndex) {
    return drive[wheelIndex];
}

float MockDrive::getLength() { return m_length; }

float MockDrive::getWidth() { return m_width; }
