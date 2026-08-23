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
                   float Kp, float Ki, float Kd) {

        PhidgetReturnCode res;

        // Add new DC Motor Handle
        res = PhidgetDCMotor_create(&dcMotor);
        setAddressProperties<PhidgetDCMotorHandle>(&dcMotor, DCSerialNumber,
                                                   DCChannel, DCPort);
        Logging::logI(file, "dcMotorRes -> %d", res);

        // Add new encoder
        res = PhidgetEncoder_create(&encoder);
        setAddressProperties<PhidgetEncoderHandle>(
            &encoder, encoderSerialNumber, encoderChannel, encoderPort);

        Logging::logI(file, "encoder -> %d", res);

        controler = std::make_unique<JointControlPID>(
            MotorHandlerReturn { MOTOR_TYPE_DC_MOTOR, { .dcMotor = &dcMotor } },
            &encoder, Kp, Ki, Kd);
    }

    ~LinearActuator() {
        Phidget_close((PhidgetHandle)dcMotor);
        PhidgetDCMotor_delete(&dcMotor);
        Phidget_close((PhidgetHandle)encoder);
        PhidgetEncoder_delete(&encoder);
    }

    void setTargetPosition(float target) override {
        controler->setAngle(target);
    }

    float getCurrentPosition() override { return controler->getAngle(); }

    void setZeroPosition() override { PhidgetEncoder_setPosition(encoder, 0); }

private:
    inline static const char* file = "LinearActuator";

    PhidgetDCMotorHandle dcMotor = {};
    PhidgetEncoderHandle encoder = {};
    std::unique_ptr<JointControlPID> controler = {};

    float target = 0; // DO SOMETHING WITH THIS
};
