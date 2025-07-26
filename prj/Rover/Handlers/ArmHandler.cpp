#include "ArmHandler.h"

#include "ArmModel.h"

// Default constructor
ArmHandler::ArmHandler() { }

// Constructor
ArmHandler::ArmHandler(Arm& arm, MessageQueue& armQueue,
                       std::condition_variable& armConVar,
                       bool& armRunningFlag) {

    // Reference to the Arm object in Rover.cpp
    m_arm = arm;

    // Initialize static Arm Model Wrapper
    ArmModel::initialize();

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

void handleManualArmMessage(ArmManualMessage message) {
    auto current_angles = ArmModel::getAngles();

    if (message.motorId > current_angles.max_size()) {
        return;
    }

    current_angles[message.motorId] += message.angleChange;

    ArmModel::updateJointAngles(current_angles);
}

void handleFixedIKMessage(ArmFixedIKMessage message) {
    std::array<double, 3> desired_wrist_position
        = { message.wristX, message.wristY, message.wristZ };

    auto new_angles = ArmModel::generateWristPosition(desired_wrist_position);
    // TODO: Update motors with new_angles
}

void handleVariableIKMessage(ArmVariableIKMessage message) {
    std::array<double, 3> desired_wrist_position
        = { message.wristX, message.wristY, message.wristZ };

    auto new_angles = ArmModel::generateWristPosition(desired_wrist_position);
    new_angles = ArmModel::generateClawOrientation(
        new_angles, message.clawPitch, message.clawRoll);
    // TODO: Update motors with new_angles
}

void ArmHandler::instantiateThreads() {
    m_armQueueThread = std::thread([queue = &m_armQueue]() {
        ArmMessage message;

        while (true) {
            // Get message from armQueue
            message = std::get<ArmMessage>(m_driveQueue->pop().get_payload());

            switch (message.type) {
            case ARM_MESSAGE_TYPE_MANUAL:
                handleManualArmMessage(message.manual_message);
                break;
            case ARM_MESSAGE_TYPE_FIXED_IK:
                handleFixedIKMessage(message.fixed_ik_message);
                break;
            case ARM_MESSAGE_TYPE_VARIABLE_IK:
                handleVariableIKMessage(message.variable_ik_message);
                break;
            default:
                break;
            }
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