#include "ArmHandler.h"
// #include "SSRTArm2025JointLimits.h"

static const char* file = "ArmHandler";

static constexpr double b1 = 10.59;
static constexpr double c1 = 24.5;
static constexpr double b2 = 13;
static constexpr double c2 = 29;

static double alpha
    = acos((14 * 14 - 24.5 * 24.5 - 35.5 * 35.5) / (2 * 24.5 * 35.5));
static double beta = atan(3.5 / 10);

static int l1ToTheta1(double l1) {
    double a1 = l1;

    double A1 = acos((a1 * a1 - b1 * b1 - c1 * c1) / (2 * b1 * c1));

    int theta1 = A1 - alpha - beta;

    return theta1;
}

static int l2ToThetaa2(double l2) {
    double a2 = l2;

    double A2 = acos((a2 * a2 - b2 * b2 - c2 * c2) / (2 * b2 * c2));

    int theta2 = 180 - A2;

    return theta2;
}

static double theta1ToL1(int theta1) {
    double A1 = theta1 + alpha + beta;

    double a1 = sqrt(2 * b1 * c1 * cos(A1) + b1 * b1 + c1 * c1);

    return a1;
}

static double theta2ToL2(int theta2) {
    double A2 = 180 - theta2;

    double a2 = sqrt(2 * b2 * c2 * cos(A2) + b2 * b2 + c2 * c2);

    return a2;
}

// Constructor
ArmHandler::ArmHandler(std::shared_ptr<Arm> arm,
                       std::shared_ptr<ArmQueue> armQueue) :
    HandlerInterface() {
    m_arm = arm;
    m_armQueue = armQueue;
}

void ArmHandler::start() {
    while (true) {
        ArmMotorState armMessage = m_armQueue->pop();

        /* Here we can add the handling logic for actuating the arm. */
        for (int i = 0; i < MotorID::MOTOR_ID_END; i++) {
            m_arm->setJointAngle(static_cast<MotorID>(i),
                                 armMessage.motorValues[i]);
        }
    }
}

// void ArmHandler::calibrateStepper(int joint_num) {
//     // This function should be called before the arm is used to ensure
//     // the steppers are at the correct orientation

//     // 1 for clockwise, -1 for counterclockwise
//     int direction = -1;
//     MotorHandlerReturn motorStuct;
//     MotorHandlerReturn encoderStuct;
//     int64_t currentIndex; // will change to its actual value when the stepper
//                           // finds it
//     double currentAngle = 0;
//     int64_t initialIndex; // index given by initial index call
//     int change = -10;     // starting off negative so first sweep is clockwise

//     // get the encoder and stepper
//     m_arm->getArmMotorHandle(&motorStuct, joint_num);
//     m_arm->getArmEncoderHandle(&encoderStuct, joint_num);

//     // Get the initial value of the index
//     PhidgetEncoder_getIndexPosition(*encoderStuct.handler.encoder,
//                                     &initialIndex);
//     double initialAngle = (initialIndex / MAX_ENCODER_POSITIONS) * 360;

//     // Get the current position of the wheel
//     int lastKnownAngle = 0;
//     PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
//                                      -lastKnownAngle);

//     int angleGoal = 0;

//     // start sweeping until index changes
//     while (1) {
//         // Get the current value of the index
//         PhidgetEncoder_getIndexPosition(*encoderStuct.handler.encoder,
//                                         &currentIndex);
//         currentAngle = (currentIndex / MAX_ENCODER_POSITIONS) * 360;

//         PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
//                                          angleGoal);

//         if (initialAngle == initialAngle) {
//             break;
//         }

//         if (abs(angleGoal) > 100) {
//             Logging::logE(file,
//                           "Calibration failed: angle goal exceeded limit.");
//             exit(1);
//         }
//         direction *= -1;
//         angleGoal = (change + abs(angleGoal)) * direction;
//     }

//     // Reset the position of the stepper motor to 0 where the index is
//     PhidgetStepper_setTargetPosition(*motorStuct.handler.stepperMotor,
//                                      currentIndex);
//     PhidgetStepper_addPositionOffset(*motorStuct.handler.stepperMotor,
//                                      -currentIndex);
//     PhidgetEncoder_setPosition(*encoderStuct.handler.encoder, 0);
// }
