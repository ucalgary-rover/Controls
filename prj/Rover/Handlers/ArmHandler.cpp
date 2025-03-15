#include "ArmHandler.h"

// Default constructor
ArmHandler::ArmHandler() { }

// Constructor
ArmHandler::ArmHandler(Arm arm, MessageQueue& armQueue) {
    // Move armThread from armThread argument to ArmHandler
    m_armQueue = armQueue;
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
    if (m_armQueueThread..joinable())
        m_armQueueThread.join();
    // if (m_stopThread.joinable())  m_stopThread.join();
}

ArmHandler::start() {
    while (true) {
        // main ArmHandler functionality
    }
}

// ArmHandler::stop() {
//     this->m_running = !this->m_running;
// }