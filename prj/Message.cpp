#include "Message.h"
// #include "pub_general.h"

#include <array>
#include <cstring>

// Constructor
Message::Message(MessagePayload payload) : m_payload(std::move(payload)) {
    // Set m_format based on the payload type
    std::visit(
        [this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, MotorState>)
                m_format = MESSAGE_FORMAT_MOTOR_STATE;
            else if constexpr (std::is_same_v<T, ScienceToolMessage>)
                m_format = MESSAGE_FORMAT_SCIENCE_TOOL;
            else
                m_format = static_cast<MessageFormat>(
                    MESSAGE_FORMAT_GENERIC); // Or a GENERIC/UNKNOWN value if
                                             // you have one
        },
        m_payload);
}

// Default Constructor
Message::Message() :
    m_payload(Generic { 0 }),
    m_format(static_cast<MessageFormat>(MESSAGE_FORMAT_GENERIC)) { }

// Copy Constructor
Message::Message(Message const& src) :
    m_payload(src.m_payload), m_format(src.m_format) { }

// Assignment Operator
Message& Message::operator=(const Message& src) {
    if (this != &src) {
        m_format = src.m_format;
        m_payload = src.m_payload;
    }
    return *this;
}

// Get the format of the message
MessageFormat Message::getFormat() const { return m_format; }

// Get the payload of the message
MessagePayload Message::getPayload() const { return m_payload; }

// Print Message details
void Message::printMessage() const {
    std::visit(
        [this](auto&& payload) {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<T, Generic>) {
                std::cout << "Generic - Value: " << payload.value;
            } else if constexpr (std::is_same_v<T, MotorState>) {
                std::cout << "MotorState: Drive - steer: ";
                for (int i = 0; i < WHEEL_COUNT; i++) {
                    std::cout << payload.driveMotorState.steer[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Drive - drive: ";
                for (int i = 0; i < WHEEL_COUNT; i++) {
                    std::cout << payload.driveMotorState.drive[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Arm: ";
                for (int i = 0; i < MOTOR_ID_END; i++) {
                    std::cout << payload.armMotorState.motorValues[i] << " ";
                }
                std::cout << std::endl;

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

// Serialize the Message object to a byte vector
std::vector<std::byte> Message::serialize() const {
    int payloadLength = sizeof(m_format);
    switch (m_format) {
    case MESSAGE_FORMAT_MOTOR_STATE: {
        payloadLength += sizeof(MotorState);
        break;
    }
    case MESSAGE_FORMAT_SCIENCE_TOOL: {
        payloadLength += sizeof(ScienceToolMessage);
        break;
    }
    default: { // Generic or unknown
        payloadLength += sizeof(Generic);
        break;
    }
    }
    return std::vector<std::byte>((std::byte*)this,
                                  (std::byte*)this + payloadLength);
}

// Extracts information received from a byte vector to a Payload
// Performs a size check on received data against a Payload
template <typename Payload>
Payload parseMessage(const std::vector<std::byte> data, size_t size) {
    if (size - 4 != sizeof(Payload)) {
        throw std::runtime_error(
            "Serialized MessagePayload size does not match expected. Expected: "
            + std::to_string(data.size())
            + " Received: " + std::to_string(sizeof(Payload)));
    }

    Payload payload;
    std::memcpy(&payload, data.data() + 4, size - 4);
    return payload;
}

// Deserialize the received byte array to a Message object
// Does not perform size checks (done in parseMessage)
Message Message::deserialize(const std::vector<std::byte> data, size_t size) {

    MessageFormat format;
    std::memcpy(&format, data.data(), sizeof(format));

    // Deserialize payload based on format
    MessagePayload payload;
    switch (format) {
    case MESSAGE_FORMAT_MOTOR_STATE: {
        payload = parseMessage<MotorState>(data, size);
        break;
    }
    case MESSAGE_FORMAT_SCIENCE_TOOL: {
        payload = parseMessage<ScienceToolMessage>(data, size);
        break;
    }
    case MESSAGE_FORMAT_GENERIC: { // Generic or unknown
        payload = parseMessage<Generic>(data, size);
        break;
    }
    default:
        throw std::runtime_error("Unknown data type.");
    }

    Message msg;
    msg.m_format = format;
    msg.m_payload = payload;
    return msg;
}
