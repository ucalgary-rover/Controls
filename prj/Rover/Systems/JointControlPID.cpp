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

    Logging::logI(file,
                  "JointControlPID initialized with Kp: %f, Ki: %f, Kd: %f", Kp,
                  Ki, Kd);
}

void JointControlPID::setAngle(float angle) {
    m_targetAngle.store(angle);
    if (!m_hasStarted) {
        // If the PID has not started, start it on first angle set.
        processes.push_back(std::thread([&]() { startPID(); }));
        m_hasStarted = true;
    }
}

float JointControlPID::getAngle() { return m_currentAngle.load(); }

void JointControlPID::startPID() {
    while (1) {
        double targetAngle = m_targetAngle.load();
        int64_t currentPosition;
        PhidgetEncoder_getPosition(*m_encoderHandle, &currentPosition);
        m_targetAngle.store(encoderPositionToAngle(currentPosition));
        double error = targetAngle - m_currentAngle.load();
        ;
        m_errorIntegral += error * m_dataInterval;
        double errorDerivative = (error - m_previousError) / m_dataInterval;

        double motorInput
            = m_Kp * error + m_Ki * m_errorIntegral + m_Kd * errorDerivative;

        if (motorInput > m_maxSpeed) {
            motorInput = m_maxSpeed;
        } else if (motorInput < -m_maxSpeed) {
            motorInput = -m_maxSpeed;
        }

        switch (m_motorHandle.type) {
        case MOTOR_TYPE_DC_MOTOR:
            // Apply PID output to DC motor
            PhidgetDCMotor_setTargetVelocity_async(
                *m_motorHandle.handler.dcMotor, motorInput, nullptr, NULL);

            break;

        case MOTOR_TYPE_BLDC_MOTOR:
            // Apply PID output to BLDC motor
            PhidgetBLDCMotor_setTargetVelocity_async(
                *m_motorHandle.handler.bldcMotor, motorInput, nullptr, NULL);

            break;
        }

        m_previousError = error;
        std::this_thread::sleep_for(std::chrono::milliseconds(m_dataInterval));
    }
}