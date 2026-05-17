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
    int angle;         // the reference angle
    int reportedAngle; // the actual angle

    // check if magnitude is zero and return a -1 value if so
    if (axisX == axisY and axisX == 0) {
        return -1;
    }

    if (axisY != 0) {
        // note how the axes have to be switched since we are measuring from the
        // y axis
        angle = (int)radianToDegree(atan((double)axisX / (double)axisY));

        // don't change reference angle (positive angle)
        if (axisX < 0 && axisY < 0) {
            reportedAngle = angle;

            // only y is negative (negative angle or zero angle)
        } else if (axisY < 0) {

            // make sure to report zero instead of 360 when possible
            if (angle == 0) {
                reportedAngle = 0;

            } else {
                reportedAngle = angle + 360;
            }

            // y is positive, x is negative or positive (positive or negative
            // angle)
        } else {
            reportedAngle = angle + 180;
        }

        // either angle of 90 or 270
    } else {
        reportedAngle = (axisX < 0) ? 90 : 270;
    }

    return reportedAngle;
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

void ControllerLayout::triggerToIncrement(int triggerValue, int* compare,
                                          int* var, int n, int min, int max,
                                          const char* name) {
    if (triggerValue > 0 && *compare < 0) {
        incrementVal(var, n, min, max, name);
    }
    *compare
        = triggerValue; // Update the compare value to the current trigger value
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
