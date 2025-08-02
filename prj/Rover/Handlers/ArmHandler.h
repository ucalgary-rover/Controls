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

private:
    Arm& m_arm;

    // Arm Queue
    MessageQueue& m_armQueue; // References the armQueue in Rover.cpp

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