#include "MockSciTool.h"

static const char* file = "MockSciTool";

MockSciTool::MockSciTool() {
    // initialise servo motors
    Logging::logD(file, "Initialising MockSciTool Servo Motors");

    // initialise DC motors
    Logging::logD(file, "Initialising MockSciTool DC Lift Motor");

    Logging::logD(file, "Initialising MockSciTool DC Brush Motors");
}

MockSciTool::~MockSciTool() {
    // deinitialise servo motors
    Logging::logD(file, "Deinitialising MockSciTool Servo Motors");

    // deinitialise DC motors
    Logging::logD(file, "Deinitialising MockSciTool DC Lift Motor");

    Logging::logD(file, "Deinitialising MockSciTool DC Brush Motors");
}

void MockSciTool::setGateServoAngle(ScienceToolIndex servo, float angle) {
    return;
}

void MockSciTool::setLiftSpeed(float speedPercentage) { return; }

void MockSciTool::setBrushSpeed(float speedPercentage) { return; }
