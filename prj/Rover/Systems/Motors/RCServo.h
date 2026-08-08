#pragma once

#include "Motor.h"

#include "Rover/Systems/pub_systems.h"
#include "phidget22.h"

#include "Motor.h"

class RCServo : public Motor {
public:
    RCServo(int servoSerialNumber, int servoChannel, int servoPort) {
        // Add new DC Motor Handle
        PhidgetRCServo_create(&servoMotor);
        setAddressProperties<PhidgetRCServoHandle>(
            &servoMotor, servoSerialNumber, servoChannel, servoPort);
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

    float target = 0; // DO SOMETHING WITH THIS
};
