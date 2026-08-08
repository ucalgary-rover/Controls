#pragma once

#include "Motor.h"

#include "Rover/Systems/pub_systems.h"
#include "phidget22.h"

#include "Motor.h"

class Stepper : public Motor {
public:
    Stepper(int stepperSerialNumber, int stepperChannel, int stepperPort) {
        // Add new stepper Handle
        PhidgetStepper_create(&stepperMotor);
        setAddressProperties<PhidgetStepperHandle>(
            &stepperMotor, stepperSerialNumber, stepperChannel, stepperPort);
    }

    ~Stepper() {
        Phidget_close((PhidgetHandle)stepperMotor);
        PhidgetStepper_delete(&stepperMotor);
    }

    void setTargetPosition(float target) override {
        this->target = target;
        PhidgetStepper_setTargetPosition_async(stepperMotor, target, nullptr,
                                               nullptr);
    }

    float getCurrentPosition() override {
        double position;
        PhidgetStepper_getPosition(stepperMotor, &(position));
        return static_cast<float>(position);
    }

    void setZeroPosition() override {
        float currentPos = getCurrentPosition();
        PhidgetStepper_addPositionOffset(stepperMotor, -currentPos);
    }

private:
    PhidgetStepperHandle stepperMotor = {};

    float target = 0; // DO SOMETHING WITH THIS
};
