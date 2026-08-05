#pragma once

#include "Motor.h"

#include "Rover/Systems/pub_systems.h"
#include "phidget22.h"

#include "Motor.h"

class Stepper : public Motor {
public:
    Stepper(int stepperSerialNumber, int stepperChannel, int stepperPort) {
        // Add new DC Motor Handle
        PhidgetStepper_create(&stepperMotor);
        setAddressProperties<PhidgetStepperHandle>(
            &stepperMotor, stepperSerialNumber, stepperChannel, stepperPort);
    }

    ~Stepper() {
        Phidget_close((PhidgetHandle)stepperMotor);
        PhidgetStepper_delete(&stepperMotor);
    }

    void setTargetPosition(float target) override { this->target = target; }

    float getCurrentPosition() override { return this->target; }

private:
    PhidgetStepperHandle stepperMotor = {};

    float target = 0; // DO SOMETHING WITH THIS
};
