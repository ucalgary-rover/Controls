#include "Message.h"

// Constructor
Message::Message(int prty, MessagePayload payload) :
    m_priority(prty), m_payload(std::move(payload)) { }

// Default Constructor
Message::Message() : m_priority(0), m_payload(Generic { 0 }) { }

// Copy Constructor
Message::Message(Message const& src) :
    m_priority(src.m_priority), m_payload(src.m_payload) { }

// Destructor
Message::~Message() { }

// Assignment Operator
Message& Message::operator=(const Message& src) {
    if (this != &src) {
        m_priority = src.m_priority;
        m_payload = src.m_payload;
    }
    return *this;
}

// Check if the message is a priority
bool Message::isPriority() const { return m_priority; }

// Print Message details
void Message::printMessage() const {
    std::cout << "Priority: " << m_priority << ", Payload: ";
    std::visit(
        [](auto&& payload) {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<T, Generic>) {
                std::cout << "Generic - Value: " << payload.value;
            } else if constexpr (std::is_same_v<T, WheelMessage>) {
                std::cout << "WheelMessage - Velocity: " << payload.velocity
                          << ", Theta: " << payload.theta
                          << ", Angle Velocity: " << payload.angle_velocity;
            } else if constexpr (std::is_same_v<T, ArmMessage>) {
                std::cout << "ArmMessage - X: " << payload.armXPos
                          << ", Y: " << payload.armYPos
                          << ", Z: " << payload.armZPos
                          << ", Wrist Rotation: " << payload.wristRotation
                          << "ClawMessage - X: " << payload.clawXPos
                          << ", Y: " << payload.clawYPos
                          << ", Open: " << payload.clawOpen
                          << ", Rotation: " << payload.clawRotation;
            } else if constexpr (std::is_same_v<T, ScienceToolMessage>) {
                std::cout << "ScienceToolMessage - Move Up/Down: "
                          << payload.moveUpDown
                          << ", Move Left/Right: " << payload.moveLeftRight;
            }
        },
        m_payload);
    std::cout << std::endl;
}
