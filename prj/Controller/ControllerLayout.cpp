#include "ControllerLayout.h"

#include <cmath>

// Constant variables for calculations
static constexpr float PI = 3.1415926;

int ControllerLayout::stickMagnitude(int axisX, int axisY) {
    // just use pythagorean to find the magnitude
    // this value influences speed of radial turning and strafing
    return (int)(sqrt(pow(axisX, 2) + pow(axisY, 2)) / 255 * 100);
}

int ControllerLayout::stickAngle(int axisX, int axisY) {
    int angle = (int)radianToDegree(atan2(-(double)axisX, -(double)axisY));
    if (angle < 0) {
        angle += 360;
    }
    return angle;
}

// Converts degrees (0 - 360) to radian (0-2pi)
float ControllerLayout::degreeToRadian(int n) { return n * PI / 180; }

// does the opposite
int ControllerLayout::radianToDegree(float n) { return n * 180 / PI; }

void ControllerLayout::unusedButton(uint8_t buttonID) {
    Logging::logV(filename.c_str(), "Button Unused\n");
}

void ControllerLayout::unusedStick(int xValue, int yValue) {
    Logging::logV(filename.c_str(), "Unused Stick X: %d, Y: %d", xValue,
                  yValue);
}

void ControllerLayout::unusedTrigger(int axisValue) {
    Logging::logV(filename.c_str(), "Unused Trigger X: %d", axisValue);
}

void ControllerLayout::stickChangeAxis(int axisX, int axisY, float* varX,
                                       float* varY, float maxChangeX,
                                       float maxChangeY, float rangeX,
                                       float rangeY, const char* nameX,
                                       const char* nameY) {
    float changeX = axisX * (maxChangeX / 255.0);
    float changeY = axisY * (maxChangeY / -255.0);

    if (varX != nullptr) {
        Logging::logI(filename.c_str(), "change %f = %d * (%f / -255.0)",
                      changeX, axisX, maxChangeX);
        incrementVal(varX, changeX, -rangeX, rangeX, nameX);
    }
    if (varY != nullptr) {
        Logging::logI(filename.c_str(), "change %f = %d * (%f / -255.0)",
                      changeY, axisY, maxChangeY);
        incrementVal(varY, changeY, -rangeY, rangeY, nameY);
    }
}
