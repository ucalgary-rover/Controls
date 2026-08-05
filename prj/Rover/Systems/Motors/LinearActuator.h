#pragma once

#include "Motor.h"

#include "Rover/Systems/pub_systems.h"
#include "phidget22.h"

#include "Motor.h"
#include "Rover/Systems/JointControlPID.h"

class LinearActuator : public Motor {
public:
    LinearActuator(int DCSerialNumber, int DCChannel, int DCPort,
                   int encoderSerialNumber, int encoderChannel, int encoderPort,
                   float Kp, float Ki, float Kd) :
        controler(
            MotorHandlerReturn { MOTOR_TYPE_DC_MOTOR, { .dcMotor = &dcMotor } },
            &encoder, Kp, Ki, Kd) {
        // Add new DC Motor Handle
        PhidgetDCMotor_create(&dcMotor);
        setAddressProperties<PhidgetDCMotorHandle>(&dcMotor, DCSerialNumber,
                                                   DCChannel, DCPort);

        // Add new encoder
        PhidgetEncoder_create(&encoder);
        setAddressProperties<PhidgetEncoderHandle>(
            &encoder, encoderSerialNumber, encoderChannel, encoderPort);
    }

    ~LinearActuator() {
        Phidget_close((PhidgetHandle)dcMotor);
        PhidgetDCMotor_delete(&dcMotor);
        Phidget_close((PhidgetHandle)encoder);
        PhidgetEncoder_delete(&encoder);
    }

    void setTargetPosition(float target) override {
        controler.setAngle(target);
    }

    float getCurrentPosition() override { return controler.getAngle(); }

private:
    PhidgetDCMotorHandle dcMotor = {};
    PhidgetEncoderHandle encoder = {};
    JointControlPID controler;

    float target = 0; // DO SOMETHING WITH THIS
};
