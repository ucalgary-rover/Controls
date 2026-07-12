#include "MockArm.h"

static const char* file = "MockArm";

MockArm::MockArm() { }

MockArm::~MockArm() { }

void MockArm::setJointAngle(MotorID joint, float value) {
    joints[joint] = value;
}

float MockArm::getJointAngle(MotorID joint) { return joints[joint]; }