#include "ArmHandler.h"

// Default constructor
ArmHandler::ArmHandler() { }

// Constructor
ArmHandler::ArmHandler(Arm& arm, MessageQueue& armQueue,
                       std::condition_variable& armConVar,
                       bool& armRunningFlag) {

    // Reference to the Arm object in Rover.cpp
    m_arm = arm;

    // Create a armQueue object in this file that references the armQueue in
    // Rover.cpp (NOT A COPY!!!) This allows us to use the same armQueue in this
    // file without shenanigans
    m_armQueue = armQueue;

    // Create a condition variable that references the armConVar in Rover.cpp
    // (NOT A COPY!!!) Used to pause the armHandler start() thread
    m_armConVar = armConVar;

    // Create a flag that references the armRunningFlag in Rover.cpp (NOT A
    // COPY!!!) Used as the flag to help armConVar pause the start() thread
    m_armRunningFlag = armRunningFlag;

    instantiateThreads();

    // NOTE: NOT IMPLEMENTED YET
    // m_ik = InverseKinematics(arm.getDOF(), arm.getDimensions());
    // m_pid = PIDController();
}

void ArmHandler::instantiateThreads() {

    m_armQueueThread = std::thread([queue = &m_armQueue]() {
        Message m;

        while (true) {
            // Get message from armQueue
            m = queue->pop();

            // do armHandler shit
            // ex:
            // var1 = m.var1;
            // var2 = m.var2;
            // And then we could use var1 and var2 in the start() infinite loop
        }
    });

    m_startThread = std::thread(&ArmHandler::start, this);
    // m_stopThread = std::thread(&ArmHandler::stop, this);
}

// Destructor
ArmHandler::~ArmHandler() {
    // Shut down arm handler threads
    if (m_startThread.joinable())
        m_startThread.join();
    if (m_armQueueThread.joinable())
        m_armQueueThread.join();
    // if (m_stopThread.joinable())
    //     m_stopThread.join();
}

ArmHandler::start() {
    while (true) {
        std::unique_lock<std::mutex>
            lock; // Required for condition_variable to work

        // Wait to unpause if the thread is paused
        m_armConVar.wait(lock, [this]() { return m_armRunningFlag; });

        // main ArmHandler functionality
    }
}

// ArmHandler::stop() {
//     // Pause the start() thread when given the signal to stop
//     if(true) // temp condition
//         this->m_runningArmHandler = !this->m_runningArmHandler;
// }