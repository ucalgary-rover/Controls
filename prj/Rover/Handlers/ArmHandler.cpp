#include "ArmHandler.h"
#include "ArmModel.h"
#include <string>

// Constructor
ArmHandler::ArmHandler(Arm& arm, MessageQueue& armQueue) :
    m_arm(arm), m_armQueue(armQueue) {

    // Open the file to store stepper positions
    std::ifstream positionFile(ARM_LAST_KNOWN_POS_FILE, std::ifstream::binary);
    if (!positionFile.is_open()) {
        Logging::logE(file, "Failed to open angle storage file.");
    }
    Json::CharReaderBuilder readerBuilder;
    std::string errs;
    Json::parseFromStream(readerBuilder, positionFile, &m_lastKnownPos, &errs);
    angleFile.close();
    // Updates for this not set up yet <------------------------------------

    // Initialize static Arm Model Wrapper
    ArmModel::initialize();

    for (int i = 0; i < m_arm->getDOF(); i++) {
        if (motorTypes[motor] == MOTOR_TYPE_STEPPER_MOTOR) {
            calibrateStepper(i);
        }
    }
}

void ArmHandler::handleManualArmMessage(ArmManualMessage message) {
    auto current_angles = ArmModel::getAngles();

    if (message.motorId > current_angles.max_size()) {
        return;
    }

    current_angles[message.motorId] += message.angleChange;

    if (!ArmModel::updateJointAngles(current_angles)) {
        return;
    }
}

void ArmHandler::handleFixedIKMessage(ArmFixedIKMessage message) {
    std::array<double, 3> desired_wrist_position
        = { message.wristX, message.wristY, message.wristZ };

    auto new_angles = ArmModel::generateWristPosition(desired_wrist_position);

    if (!ArmModel::updateJointAngles(new_angles)) {
        return;
    }
}

void ArmHandler::handleVariableIKMessage(ArmVariableIKMessage message) {
    std::array<double, 3> desired_wrist_position
        = { message.wristX, message.wristY, message.wristZ };

    auto new_angles = ArmModel::generateWristPosition(desired_wrist_position);
    new_angles = ArmModel::generateClawOrientation(
        new_angles, message.clawPitch, message.clawRoll);

    if (!ArmModel::updateJointAngles(new_angles)) {
        return;
    }
}

void updateMotorAngle(MotorHandlerReturn* handler, double angle) {
    if (handler == nullptr) {
        return;
    }

    switch (handler->type) {
    case MOTOR_TYPE_DC_MOTOR:
        break;
    case MOTOR_TYPE_STEPPER_MOTOR:
        break;
    case MOTOR_TYPE_SERVO_MOTOR:
        break;
    case MOTOR_TYPE_DIGITAL_INPUT:
        break;
    case MOTOR_TYPE_ENCODER:
        break;
    default:
        break;
    }
}

template <int T>
void ArmHandler::updateMotorAngles(std::array<double, T> new_angles) {
    for (int i = 0; i < T; i++) {
        MotorHandlerReturn* handler = nullptr;
        m_arm.getArmMotorHandle(handler, i);
        updateMotorAngle(handler, new_angles.at(i));
    }
}

void ArmHandler::calibrateStepper(int stepper) {
    // This function should be called before the arm is used to ensure
    // the steppers are at the correct orientation

    // 1 for clockwise, -1 for counterclockwise
    int direction = -1;
    MotorHandlerReturn motorStuct;
    MotorHandlerReturn encoderStuct;
    int currentIndex; // will change to its actual value when the stepper finds
                      // it
    int initialIndex; // index given by initial index call
    int change = -10; // starting off negative so first sweep is clockwise

    // get the encoder and stepper
    m_arm->getArmMotorHandle(&motorStuct, wheel);
    m_arm->getArmEncoderHandle(&encoderStuct, stepper);

    // Get the initial value of the index
    initialIndex = getIndexPosition(*encoderStuct.handler.encoder);

    // Get the current position of the wheel
    int lastKnownAngle = m_lastKnownPos[std::to_string(stepper)].asInt();
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -lastKnownAngle);

    // start sweeping until index changes
    while (currentIndex == initialIndex) {
        angleGoal += change * direction;

        PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                         angleGoal);

        // Get the current value of the index
        currentIndex = getIndexPosition(*encoderStuct.handler.encoder);

        if (abs(angleGoal) > 100) {
            Logging::logE(file,
                          "Calibration failed: angle goal exceeded limit.");
            exit(1);
        }
    }

    // Reset the position of the stepper motor to 0 where the index is
    PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                     currentIndex);
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -currentIndex);
    PhidgetEncoder_setPosition(*encoderStuct.handler.encoder, 0);
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

void ArmHandler::loop() {
    ArmMessage message;

    while (true) {
        // Get message from armQueue
        message = m_armQueue.pop().get_payload();

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
}

void ArmHandler::start() {
    m_startThread = std::thread(&ArmHandler::loop, this);
}
