#ifndef ARMHANDLER_H
#define ARMHANDLER_H

#pragma once

#include "HandlerInterface.h"
#include "Rover/Rover.h"       // This also needs to be fixed
#include "Rover/Systems/Arm.h" // This needs to be fixed
#include <mutex>

class ArmHandler : public HandlerInterface {
public:
    ArmHandler(Arm& arm, MessageQueue& armQueue);
    ~ArmHandler();

    void start() override;

    /**
     * @brief Calibrates steppers so they start in the zero positions
     * @param stepper the index of the stepper to be calibrated
     */
    void calibrateStepper(int stepper);

private:
    Arm& m_arm;

    // Arm Queue
    MessageQueue& m_armQueue; // References the armQueue in Rover.cpp

    Json::Value m_lastKnownPos; // Last known position of the arm

    // Threads
    std::thread m_armQueueThread;
    std::thread m_startThread;
    // std::thread m_stopThread;

    // NOT IMPLEMENTED YET
    // InverseKinematics m_ik;
    // PIDController m_pid;

    void handleManualArmMessage(ArmManualMessage message);
    void handleFixedIKMessage(ArmFixedIKMessage message);
    void handleVariableIKMessage(ArmVariableIKMessage message);

    template <int T> void updateMotorAngles(std::array<double, T> new_angles);

    void loop();
};

#endif