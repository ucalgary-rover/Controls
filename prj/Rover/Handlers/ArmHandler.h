#ifndef ARMHANDLER_H
#define ARMHANDLER_H

#pragma once

#include "Arm.h"
#include "HandlerInterface.h"
#include "Rover.h"
#include <atomic>

class ArmHandler : public HandlerInterface {
public:
    ArmHandler();
    ArmHandler(Arm arm, MesssaqeQueue& armQueue);
    ~ArmHandler();

    void instantiateThreads() override;
    void start() override;
    void stop() override;

private:
    // Arm Queue
    MessageQueue& m_armQueue;

    // Threads
    std::thread m_armQueueThread;
    std::thread m_startThread;
    // std::thread m_stopThread;

    // Flag for running the infinite start() loop
    std::atomic<bool> m_running; // Add an atomic flag to control the loop

    // NOT IMPLEMENTED YET
    // InverseKinematics m_ik;
    // PIDController m_pid;
};

#endif