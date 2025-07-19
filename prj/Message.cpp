#include "Message.h"

static const char* file = "Message";

// Constructor
Message::Message(int prty, MessagePayload payload) :
    m_isHighPriority(prty), m_payload(std::move(payload)) {
    // Set m_format based on the payload type
    std::visit(
        [this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, WheelMessage>)
                m_format = MESSAGE_FORMAT_WHEEL;
            else if constexpr (std::is_same_v<T, ArmMessage>)
                m_format = MESSAGE_FORMAT_ARM;
            else if constexpr (std::is_same_v<T, ScienceToolMessage>)
                m_format = MESSAGE_FORMAT_SCIENCE_TOOL;
            else
                m_format = static_cast<MessageFormat>(
                    -1); // Or a GENERIC/UNKNOWN value if you have one
        },
        m_payload);
}

// Default Constructor
Message::Message() :
    m_isHighPriority(0), m_payload(Generic { 0 }),
    m_format(static_cast<MessageFormat>(-1)) { }

// Copy Constructor
Message::Message(Message const& src) :
    m_isHighPriority(src.m_isHighPriority), m_payload(src.m_payload),
    m_format(src.m_format) { }

// Destructor
Message::~Message() { }

// Assignment Operator
Message& Message::operator=(const Message& src) {
    if (this != &src) {
        m_isHighPriority = src.m_isHighPriority;
        m_payload = src.m_payload;
        m_format = src.m_format;
    }
    return *this;
}

// Check if the message is a priority
bool Message::isHighPriority() const { return m_isHighPriority; }

// Get the format of the message
MessageFormat Message::getFormat() const { return m_format; }

// Print Message details
void Message::printMessage() const {
    Logging::logV(file, "Priority: %d", m_isHighPriority);
    Logging::logV(file, "Payload:");
    std::visit(
        [this](auto&& payload) {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<T, Generic>) {
                Logging::logV(file, "Generic - Value: %d" + payload.value);
            } else if constexpr (std::is_same_v<T, WheelMessage>) {
                Logging::logV(file,
                              "WheelMessage - Velocity: %d, Theta: %d, Angle "
                              "Velocity%d: ",
                              payload.velocity, payload.theta,
                              payload.angleVelocity);
            } else if constexpr (std::is_same_v<T, ArmMessage>) {
                Logging::logV(file, "ArmMessage - Type: %d", payload.type);
                switch (payload.type) {
                case ARM_MESSAGE_TYPE_MANUAL:
                    Logging::logV(file, "MotorID: %d, AngleChange: %d",
                                  payload.manual_message.motorId,
                                  payload.manual_message.angleChange);
                    break;
                case ARM_MESSAGE_TYPE_FIXED_IK:
                    Logging::logV(
                        file,
                        "WristX: %f, WristY: %f, WristZ: %f, ClawOpen: %d",
                        payload.fixed_ik_message.wristX,
                        payload.fixed_ik_message.wristY,
                        payload.fixed_ik_message.wristZ,
                        payload.fixed_ik_message.clawOpen);
                    break;
                case ARM_MESSAGE_TYPE_VARIABLE_IK:
                    Logging::logV(file,
                                  "WristX: %f, WristY: %f, WristZ: %f, "
                                  "clawPitch: %d, clawRoll: %d, ClawOpen: %d",
                                  payload.variable_ik_message.wristX,
                                  payload.variable_ik_message.wristY,
                                  payload.variable_ik_message.wristZ,
                                  payload.variable_ik_message.clawPitch,
                                  payload.variable_ik_message.clawRoll,
                                  payload.variable_ik_message.clawOpen);
                    break;
                default:
                    Logging::logV(file, "Unknown ArmMessage type");
                }
            } else if constexpr (std::is_same_v<T, ScienceToolMessage>) {
                Logging::logV(file,
                              "ScienceToolMessage - Move Up/Down: %d, Move "
                              "Left/Right: %d, X Pos: %d, Y Pos: %d",
                              payload.moveUpDown, payload.moveLeftRight,
                              payload.xPos, payload.yPos);
            }
        },
        m_payload);
}

// Serialize the Message object to a string
std::string Message::serialize() const {
    std::ostringstream oss;
    oss << m_isHighPriority << " " << static_cast<int>(m_format)
        << " "; // Serialize priority and format

    // Serialize payload based on its type
    std::visit(
        [&oss](auto&& payload) {
            using T = std::decay_t<decltype(payload)>;

            // Generic message
            if constexpr (std::is_same_v<T, Generic>) {
                oss << payload.value;
            }
            // Wheel message
            else if constexpr (std::is_same_v<T, WheelMessage>) {
                oss << payload.velocity << " " << payload.theta << " "
                    << payload.angleVelocity;
            }
            // Arm message
            else if constexpr (std::is_same_v<T, ArmMessage>) {
                oss << static_cast<int>(payload.type) << " ";
                switch (payload.type) {
                case ARM_MESSAGE_TYPE_MANUAL:
                    oss << static_cast<int>(payload.manual_message.motorId)
                        << " " << payload.manual_message.angleChange;
                    break;
                case ARM_MESSAGE_TYPE_FIXED_IK:
                    oss << payload.fixed_ik_message.wristX << " "
                        << payload.fixed_ik_message.wristY << " "
                        << payload.fixed_ik_message.wristZ << " "
                        << payload.fixed_ik_message.clawOpen;
                    break;
                case ARM_MESSAGE_TYPE_VARIABLE_IK:
                    oss << payload.variable_ik_message.wristX << " "
                        << payload.variable_ik_message.wristY << " "
                        << payload.variable_ik_message.wristZ << " "
                        << payload.variable_ik_message.clawPitch << " "
                        << payload.variable_ik_message.clawRoll << " "
                        << payload.variable_ik_message.clawOpen;
                    break;
                default:
                    break;
                }
            } else if constexpr (std::is_same_v<T, ScienceToolMessage>) {
                oss << payload.moveUpDown << " " << payload.moveLeftRight << " "
                    << payload.xPos << " " << payload.yPos;
            }
        },
        m_payload);

    return oss.str();
}
