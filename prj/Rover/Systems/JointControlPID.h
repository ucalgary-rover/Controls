#ifndef JOINT_CONTROL_PID_H
#define JOINT_CONTROL_PID_H

#pragma once

#include "Rover/pub_rover.h"
#include <atomic>
#include <mutex>
#include <thread>

#define ENCODER_RESOLUTION 512

class JointControlPID {
public:
    JointControlPID(MotorHandlerReturn handlerReturn,
                    PhidgetEncoderHandle* encoderHandle, float Kp, float Ki,
                    float Kd);

    void setAngle(float angle);
    float getAngle();

    void startPID();

private:
    bool m_hasStarted = false;

    MotorHandlerReturn m_motorHandle;
    PhidgetEncoderHandle* m_encoderHandle;
    float m_Kp;
    float m_Ki;
    float m_Kd;

    std::atomic<double> m_targetAngle;
    std::atomic<double> m_currentAngle;

    double m_errorIntegral = 0;
    double m_previousError = 0;

    double m_maxSpeed = 0.0;
    uint32_t m_dataInterval;

    inline static auto processes
        = std::vector<std::thread>(); // Track instantiated processes

    double encoderPositionToAngle(int64_t encoderPosition) {
        return (static_cast<double>(encoderPosition) / ENCODER_RESOLUTION)
               * 360.0;
    }
};

#endif // JOINT_CONTROL_PID_H
