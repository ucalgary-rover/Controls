#pragma once

#include <memory>

#include "ArmMotorStateManager.h"
#include "HandlerInterface.h"
#include "MessageQueue.h"
#include "Rover/Systems/Arm.h"

using ArmQueue = MessageQueue<ArmMotorState>;

class ArmHandler : public HandlerInterface {
public:
    ArmHandler(std::shared_ptr<Arm> arm, std::shared_ptr<ArmQueue> armQueue);
    ~ArmHandler() { };

    void start() override;

    // Don't know how to do this yet
    // /**
    //  * @brief Calibrates steppers so they start in the zero positions
    //  * @param joint_num the index of the joint to be calibrated
    //  */
    // void calibrateStepper(int joint_num);

private:
    std::shared_ptr<Arm> m_arm;
    std::shared_ptr<ArmQueue> m_armQueue;
};
