#pragma once

#include "Motor.h"

#include "Rover/Systems/pub_systems.h"
#include "phidget22.h"

#include "Motor.h"

class RCServo : public Motor {
public:
    RCServo(int servoSerialNumber, int servoChannel, int servoPort, double min,
            double max) {
        // Add new RC servo Motor Handle
        PhidgetReturnCode res;
        res = PhidgetRCServo_create(&servoMotor);
        Logging::logD(file, "PhidgetRCServo_create %d", res);
        setAddressProperties<PhidgetRCServoHandle>(
            &servoMotor, servoSerialNumber, servoChannel, servoPort);
        PhidgetRCServo_setMinPosition(servoMotor, min);
        PhidgetRCServo_setMinPosition(servoMotor, max);
    }

    ~RCServo() {
        Phidget_close((PhidgetHandle)servoMotor);
        PhidgetRCServo_delete(&servoMotor);
    }

    void setTargetPosition(float target) override {
        this->target = target;
        PhidgetRCServo_setTargetPosition_async(servoMotor, target, nullptr,
                                               nullptr);
    }

    float getCurrentPosition() override {
        double position;
        PhidgetRCServo_getPosition(servoMotor, &(position));
        return static_cast<float>(position);
    }

private:
    PhidgetRCServoHandle servoMotor = {};
    inline static const char* file = "RCServo";

    float target = 0; // DO SOMETHING WITH THIS
};
