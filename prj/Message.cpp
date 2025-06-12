#include "Message.h"

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
    std::cout << "Priority: " << m_isHighPriority << ", Payload: ";
    std::visit(
        [this](auto&& payload) {
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
                          << ", Claw X: " << payload.clawXPos
                          << ", Claw Y: " << payload.clawYPos
                          << ", Claw Open: " << payload.clawOpen
                          << ", Claw Rotation: " << payload.clawRotation
                          << ", Wrist Rotation: " << payload.wristRotation;
            } else if constexpr (std::is_same_v<T, ScienceToolMessage>) {
                std::cout << "ScienceToolMessage - Move Up/Down: "
                          << payload.moveUpDown
                          << ", Move Left/Right: " << payload.moveLeftRight
                          << ", X Pos: " << payload.xPos
                          << ", Y Pos: " << payload.yPos;
            }
        },
        m_payload);
    std::cout << std::endl;
}

// Serialize the Message object to a string
std::string Message::serialize() const {
    //
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
                    << payload.angle_velocity;
            }
            // Arm message
            else if constexpr (std::is_same_v<T, ArmMessage>) {
                oss << payload.armXPos << " " << payload.armYPos << " "
                    << payload.armZPos << " " << payload.clawXPos << " "
                    << payload.clawYPos << " " << payload.clawOpen << " "
                    << payload.clawRotation << " " << payload.wristRotation;
            }
            // Science tool message (Don't think we need this anymore)
            else if constexpr (std::is_same_v<T, ScienceToolMessage>) {
                oss << payload.moveUpDown << " " << payload.moveLeftRight << " "
                    << payload.xPos << " " << payload.yPos;
            }
        },
        m_payload);

    return oss.str();
}

// Deserialize a string to a Message object
Message Message::deserialize(const std::string& data) {
    std::istringstream iss(data);
    bool isHighPriority;
    int formatInt;
    iss >> isHighPriority >> formatInt;

    MessageFormat format = static_cast<MessageFormat>(formatInt);

    // Deserialize payload based on format
    MessagePayload payload;
    switch (format) {
    case MESSAGE_FORMAT_WHEEL: {
        WheelMessage wm;
        iss >> wm.velocity >> wm.theta >> wm.angle_velocity;
        payload = wm;
        break;
    }
    case MESSAGE_FORMAT_ARM: {
        ArmMessage am;
        iss >> am.armXPos >> am.armYPos >> am.armZPos >> am.clawXPos
            >> am.clawYPos >> am.clawOpen >> am.clawRotation
            >> am.wristRotation;
        payload = am;
        break;
    }
    case MESSAGE_FORMAT_SCIENCE_TOOL: {
        ScienceToolMessage stm;
        iss >> stm.moveUpDown >> stm.moveLeftRight >> stm.xPos >> stm.yPos;
        payload = stm;
        break;
    }
    default: { // Generic or unknown
        Generic g;
        iss >> g.value;
        payload = g;
        break;
    }
    }

    Message msg;
    msg.m_isHighPriority = isHighPriority;
    msg.m_format = format;
    msg.m_payload = payload;
    return msg;
}
