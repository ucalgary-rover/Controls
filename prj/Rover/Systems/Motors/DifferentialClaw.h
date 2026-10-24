#pragma once

#include "Stepper.h"

#include <memory>
#include <unistd.h>

class DifferentialClaw {
public:
    DifferentialClaw(std::shared_ptr<Stepper> s1, std::shared_ptr<Stepper> s2) {
        stepper1 = s1;
        stepper2 = s2;
    }

    void setTargetPitch(float target) {
        float diff = target - pitch;

        // Pitch Set both Motors the Same
        stepper1Active = true;
        stepper2Active = true;
        PhidgetStepper_setTargetPosition_async(stepper1->stepperMotor, diff,
                                               motorFinished,
                                               (void*)&stepper1Active);
        PhidgetStepper_setTargetPosition_async(stepper1->stepperMotor, diff,
                                               motorFinished,
                                               (void*)&stepper2Active);

        while (stepper1Active || stepper2Active) {
            usleep(0.0001);
        }
    }

    void setTargetRoll(float target) {
        float diff = target - pitch;

        // Pitch Set both Motors the Same
        stepper1Active = true;
        stepper2Active = true;
        PhidgetStepper_setTargetPosition_async(stepper1->stepperMotor, diff,
                                               motorFinished,
                                               (void*)&stepper1Active);
        PhidgetStepper_setTargetPosition_async(stepper1->stepperMotor, -diff,
                                               motorFinished,
                                               (void*)&stepper2Active);

        while (stepper1Active || stepper2Active) {
            usleep(0.0001);
        }
    }

    float getCurrentPitch() { return pitch; }

    float getCurrentRoll() { return roll; }

    void setZero() {
        stepper1->setZeroPosition();
        stepper2->setZeroPosition();
    };

private:
    std::shared_ptr<Stepper> stepper1;
    std::shared_ptr<Stepper> stepper2;

    volatile bool stepper1Active;
    volatile bool stepper2Active;

    float pitch;
    float roll;

    static void motorFinished(PhidgetHandle ch, void* ctx,
                              PhidgetReturnCode returnCode) {
        (*(bool*)ctx) = false;
    }
};