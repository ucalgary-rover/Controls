#ifndef ARMHANDLER_H
#define ARMHANDLER_H

#pragma once

#include "./Rover.h"       // This also needs to be fixed
#include "./Systems/Arm.h" // This needs to be fixed
#include "HandlerInterface.h"
#include <mutex>

class ArmHandler : public HandlerInterface {
public:
    ArmHandler();
    ArmHandler(Arm& arm, MessageQueue& armQueue,
               std::condition_variable& armConVar, bool& armRunningFlag);
    ~ArmHandler();

    void instantiateThreads() override;
    void start() override;
    void stop() override;

private:
    Arm& m_arm;

    // Arm Queue
    MessageQueue& m_armQueue; // References the armQueue in Rover.cpp

    // Threads
    std::thread m_armQueueThread;
    std::thread m_startThread;
    // std::thread m_stopThread;

    // Variables for pausing and resuming the start() infinite loop
    std::condition_variable& m_armConVar;
    bool& m_armRunningFlag;

    // NOT IMPLEMENTED YET
    // InverseKinematics m_ik;
    // PIDController m_pid;
};

#endif