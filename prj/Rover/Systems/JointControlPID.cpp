#include "JointControlPID.h"

static const char* file = "JointControlPID";

JointControlPID::JointControlPID(MotorHandlerReturn handlerReturn,
                                 PhidgetEncoderHandle* encoderHandle, float Kp,
                                 float Ki, float Kd) :
    m_motorHandle(handlerReturn), m_encoderHandle(encoderHandle), m_Kp(Kp),
    m_Ki(Ki), m_Kd(Kd) {
    PhidgetEncoder_getDataInterval(*m_encoderHandle, &m_dataInterval);

    // Initialize the PID controller with the specified parameters
    switch (m_motorHandle.type) {
    case MOTOR_TYPE_DC_MOTOR:
        // Apply PID output to DC motor
        PhidgetDCMotor_getMaxVelocity(*m_motorHandle.handler.dcMotor,
                                      &m_maxSpeed);

        break;
    case MOTOR_TYPE_BLDC_MOTOR:
        // Apply PID output to BLDC motor
        PhidgetBLDCMotor_getMaxVelocity(*m_motorHandle.handler.bldcMotor,
                                        &m_maxSpeed);

        break;
    }

    m_isInitiated = true;

    Logging::logI(file,
                  "JointControlPID initialized with Kp: %f, Ki: %f, Kd: %f", Kp,
                  Ki, Kd);
}

JointControlPID::JointControlPID() { }

void JointControlPID::setAngle(float angle) {
    m_targetAngle.store(angle);
    Logging::logI(file, "Setting target to %.3f", angle);
    if (!m_hasStarted && m_isInitiated) {
        // If the PID has not started, start it on first angle set.
        processes.push_back(std::thread([&]() { startPID(); }));
        m_hasStarted = true;
    }
}

float JointControlPID::getAngle() { return m_currentAngle.load(); }

void JointControlPID::startPID() {
    while (1) {
        double targetAngle = m_targetAngle.load();

        Logging::logI(file, "target_angle = %.5f", targetAngle);

        int64_t currentPosition;
        PhidgetReturnCode res;
        res = PhidgetEncoder_getPosition(*m_encoderHandle, &currentPosition);
        Logging::logV(file, "encoder read res -> %d \nread value %d", res,
                      currentPosition);

        m_currentAngle.store(encoderPositionToAngle(currentPosition));
        double error = targetAngle - m_currentAngle.load();

        Logging::logI(file, "error = %.5f", error);

        m_errorIntegral += error * m_dataInterval;

        Logging::logI(file, "m_errorIntegral = %.5f", m_errorIntegral);

        double errorDerivative = (error - m_previousError) / m_dataInterval;

        Logging::logI(file, "errorDerivative = %.5f", errorDerivative);

        double motorInput
            = m_Kp * error + m_Ki * m_errorIntegral + m_Kd * errorDerivative;

        Logging::logI(file, "motorInput = %.5f", motorInput);

        double motorInputClamped = 0.0;

        if (motorInput > m_maxSpeed) {
            motorInputClamped = m_maxSpeed;
        } else if (motorInput < -m_maxSpeed) {
            motorInputClamped = -m_maxSpeed;
        } else {
            motorInputClamped = motorInput;
        }

        Logging::logI(file, "motorInputClamped = %.5f", motorInputClamped);

        switch (m_motorHandle.type) {
        case MOTOR_TYPE_DC_MOTOR:
            Logging::logI(file, "Setting DC velocity to %.3f",
                          motorInputClamped);
            // Apply PID output to DC motor
            PhidgetDCMotor_setTargetVelocity_async(
                *m_motorHandle.handler.dcMotor, motorInputClamped, nullptr,
                NULL);

            break;

        case MOTOR_TYPE_BLDC_MOTOR:
            Logging::logI(file, "Setting BLDC velocity to %.3f",
                          motorInputClamped);
            // Apply PID output to BLDC motor
            PhidgetBLDCMotor_setTargetVelocity_async(
                *m_motorHandle.handler.bldcMotor, motorInputClamped, nullptr,
                NULL);

            break;
        }

        m_previousError = error;
        std::this_thread::sleep_for(std::chrono::milliseconds(m_dataInterval));
    }
}