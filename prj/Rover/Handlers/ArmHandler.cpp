#include "ArmHandler.h"
// #include "SSRTArm2025JointLimits.h"

static const char* file = "ArmHandler";

// Constructor
ArmHandler::ArmHandler(Arm& arm, ArmMotorStateManager& armMotorStateManager) :
    m_arm(arm), m_armMotorStateManager(armMotorStateManager) {

    // Open the file to store stepper positions
    std::ifstream angleFile(ARM_LAST_KNOWN_POS_FILE, std::ifstream::binary);
    if (!angleFile.is_open()) {
        Logging::logE(file, "Failed to open angle storage file.");
    }
    Json::CharReaderBuilder readerBuilder;
    std::string errs;
    Json::parseFromStream(readerBuilder, angleFile, &m_lastKnownPos, &errs);
    angleFile.close();
    // Updates for this not set up yet <------------------------------------

    // Initialize static Arm Model Wrapper
    ArmModel::initialize();

    // for (int i = 0; i < m_arm.getDOF(); i++) {
    //     calibrateStepper(i);
    // }
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

static constexpr double b1 = 10.59;
static constexpr double c1 = 24.5;
static constexpr double b2 = 13;
static constexpr double c2 = 29;

static double alpha
    = acos((14 * 14 - 24.5 * 24.5 - 35.5 * 35.5) / (2 * 24.5 * 35.5));
static double beta = atan(3.5 / 10);

int l1ToTheta1(double l1) {
    double a1 = l1;

    double A1 = acos((a1 * a1 - b1 * b1 - c1 * c1) / (2 * b1 * c1));

    int theta1 = A1 - alpha - beta;

    return theta1;
}

int l2ToThetaa2(double l2) {
    double a2 = l2;

    double A2 = acos((a2 * a2 - b2 * b2 - c2 * c2) / (2 * b2 * c2));

    int theta2 = 180 - A2;

    return theta2;
}

double theta1ToL1(int theta1) {
    double A1 = theta1 + alpha + beta;

    double a1 = sqrt(2 * b1 * c1 * cos(A1) + b1 * b1 + c1 * c1);

    return a1;
}

double theta2ToL2(int theta2) {
    double A2 = 180 - theta2;

    double a2 = sqrt(2 * b2 * c2 * cos(A2) + b2 * b2 + c2 * c2);

    return a2;
}

void ArmHandler::updateMotorAngles(std::array<int, 6> new_angles) {

    std::array<double, 6> new_actuator_values = {};

    // Base
    new_actuator_values[0] = new_angles.at(0);

    // Linear Actuator 1
    new_actuator_values[1] = theta1ToL1(new_angles.at(1));

    // Linear Actuator 2
    new_actuator_values[2] = theta2ToL2(new_angles.at(2));

    // Wrist
    new_actuator_values[3] = new_angles.at(3);

    new_actuator_values[4] = new_angles.at(4);

    new_actuator_values[5] = new_angles.at(5);

    for (int i = 0; i < 6; i++) {
        MotorHandlerReturn* handler = nullptr;
        m_arm.getArmMotorHandle(handler, i);
        updateMotorAngle(handler, new_actuator_values.at(i));
    }
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

void ArmHandler::start() {

    while (true) {
        // Get message from armQueue

        // TODO: Handle the message
        /*if (msg.getFormat() != MessageFormat::MESSAGE_FORMAT_ARM) {
            Logging::logE(file, "Received non-arm message in armQueue %d",
                          msg.getFormat());
            continue;
        }

        message = std::get<ArmMessage>(msg.get_payload());

        switch (message.type) {
        case ARM_MESSAGE_TYPE_MANUAL:
            handleManualArmMessage(message.manual_message);
            break;
        // case ARM_MESSAGE_TYPE_FIXED_IK:
        //     handleFixedIKMessage(message.fixed_ik_message);
        //     break;
        // case ARM_MESSAGE_TYPE_VARIABLE_IK:
        //     handleVariableIKMessage(message.variable_ik_message);
        //     break;
        default:
            break;
        }*/
    }
}

void ArmHandler::calibrateStepper(int joint_num) {
    // This function should be called before the arm is used to ensure
    // the steppers are at the correct orientation

    // 1 for clockwise, -1 for counterclockwise
    int direction = -1;
    MotorHandlerReturn motorStuct;
    MotorHandlerReturn encoderStuct;
    int64_t currentIndex; // will change to its actual value when the stepper
                          // finds it
    double currentAngle = 0;
    int64_t initialIndex; // index given by initial index call
    int change = -10;     // starting off negative so first sweep is clockwise

    // get the encoder and stepper
    m_arm.getArmMotorHandle(&motorStuct, joint_num);
    m_arm.getArmEncoderHandle(&encoderStuct, joint_num);

    // Get the initial value of the index
    PhidgetEncoder_getIndexPosition(*encoderStuct.handler.encoder,
                                    &initialIndex);
    double initialAngle = (initialIndex / MAX_ENCODER_POSITIONS) * 360;

    // Get the current position of the wheel
    int lastKnownAngle = m_lastKnownPos[std::to_string(joint_num)].asInt();
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -lastKnownAngle);

    int angleGoal = 0;

    // start sweeping until index changes
    while (1) {
        // Get the current value of the index
        PhidgetEncoder_getIndexPosition(*encoderStuct.handler.encoder,
                                        &currentIndex);
        currentAngle = (currentIndex / MAX_ENCODER_POSITIONS) * 360;

        PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                         angleGoal);

        if (initialAngle == initialAngle) {
            break;
        }

        if (abs(angleGoal) > 100) {
            Logging::logE(file,
                          "Calibration failed: angle goal exceeded limit.");
            exit(1);
        }
        direction *= -1;
        angleGoal = (change + abs(angleGoal)) * direction;
    }

    // Reset the position of the stepper motor to 0 where the index is
    PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
                                     currentIndex);
    PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
                                     -currentIndex);
    PhidgetEncoder_setPosition(*encoderStuct.handler.encoder, 0);
}
