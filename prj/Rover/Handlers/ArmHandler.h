#ifndef ARMHANDLER_H
#define ARMHANDLER_H

#pragma once

#include "ArmModel.h"
#include "HandlerInterface.h"
#include "Rover/Rover.h"
#include "Rover/Systems/Arm.h"
#include <jsoncpp/json/json.h>
#include <mutex>
#include <string>

class ArmHandler : public HandlerInterface {
public:
    ArmHandler(Arm& arm, MessageQueue& armQueue);
    ~ArmHandler();

    void start() override;

    /**
     * @brief Calibrates steppers so they start in the zero positions
     * @param joint_num the index of the joint to be calibrated
     */
    void calibrateStepper(int joint_num);

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

    void updateMotorAngles(std::array<int, 6> new_angles);
};

#endif