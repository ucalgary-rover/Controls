#include "Message.h"
// #include "pub_general.h"

#include <array>
#include <cstring>

#include "Logging.h"

static const char* file = "Message";

// Constructor
Message::Message(MessagePayload payload) : m_payload(std::move(payload)) {
    // Set m_format based on the payload type
    std::visit(
        [this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, MotorState>)
                m_format = MESSAGE_FORMAT_MOTOR_STATE;
            else if constexpr (std::is_same_v<T, SciToolDoorMessage>)
                m_format = MESSAGE_FORMAT_SCI_TOOL_DOOR;
            else if constexpr (std::is_same_v<T, SciToolHeightMessage>)
                m_format = MESSAGE_FORMAT_SCI_TOOL_HEIGHT;
            else if constexpr (std::is_same_v<T, SciToolBrushMessage>)
                m_format = MESSAGE_FORMAT_SCI_TOOL_BRUSH;
            else if constexpr (std::is_same_v<T, CameraServoMessage>)
                m_format = MESSAGE_FORMAT_CAMERA_SERVO;
            else if constexpr (std::is_same_v<T, DriveZeroMessage>)
                m_format = MESSAGE_FORMAT_DRIVE_ZERO;
            else if constexpr (std::is_same_v<T, ArmZeroMessage>)
                m_format = MESSAGE_FORMAT_ARM_ZERO;
            else if constexpr (std::is_same_v<T, HeadlightMessage>)
                m_format = MESSAGE_FORMAT_HEADLIGHTS;
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
                for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
                    std::cout << payload.driveMotorState.steer[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Drive - drive: ";
                for (int i = 0; i < DRIVE_INDEX_WHEEL_COUNT; i++) {
                    std::cout << payload.driveMotorState.drive[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Arm: ";
                for (int i = 0; i < MOTOR_ID_END; i++) {
                    std::cout << payload.armMotorState.motorValues[i] << " ";
                }
                std::cout << std::endl;

            } else if constexpr (std::is_same_v<T, SciToolDoorMessage>) {
                std::cout << "SciToolDoorMessage - Door: " << payload.door;

            } else if constexpr (std::is_same_v<T, SciToolHeightMessage>) {
                std::cout << "SciToolHeightMessage - Control: "
                          << payload.control;

            } else if constexpr (std::is_same_v<T, SciToolBrushMessage>) {
                std::cout << "SciToolBrushMessage - Control: "
                          << payload.control;

            } else if constexpr (std::is_same_v<T, CameraServoMessage>) {
                std::cout << "CameraServoMessage - Control: "
                          << payload.control;

            } else if constexpr (std::is_same_v<T, DriveZeroMessage>) {
                std::cout << "DriveZeroMessage - set";

            } else if constexpr (std::is_same_v<T, ArmZeroMessage>) {
                std::cout << "ArmZeroMessage - set";

            } else if constexpr (std::is_same_v<T, HeadlightMessage>) {
                std::cout << "HeadlightMessage - brightness: "
                          << payload.brightnessPercentage;
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
    case MESSAGE_FORMAT_SCI_TOOL_DOOR: {
        payloadLength += sizeof(SciToolDoorMessage);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_HEIGHT: {
        payloadLength += sizeof(SciToolHeightMessage);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_BRUSH: {
        payloadLength += sizeof(SciToolBrushMessage);
        break;
    }
    case MESSAGE_FORMAT_CAMERA_SERVO: {
        payloadLength += sizeof(CameraServoMessage);
        break;
    }
    case MESSAGE_FORMAT_DRIVE_ZERO: {
        payloadLength += sizeof(DriveZeroMessage);
        break;
    }
    case MESSAGE_FORMAT_ARM_ZERO: {
        payloadLength += sizeof(ArmZeroMessage);
        break;
    }
    case MESSAGE_FORMAT_HEADLIGHTS: {
        payloadLength += sizeof(HeadlightMessage);
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
bool parseMessage(const std::vector<std::byte> data, size_t size,
                  MessagePayload& out) {
    if (size - 4 != sizeof(Payload)) {
        Logging::logI(file,
                      "Serialized MessagePayload size does not match expected. "
                      "Expected: %lu Received: %lu",
                      std::to_string(data.size()),
                      std::to_string(sizeof(Payload)));
        return false;
    }

    Payload payload;
    std::memcpy(&payload, data.data() + 4, size - 4);
    out = payload;
    return true;
}

// Deserialize the received byte array to a Message object
// Does not perform size checks (done in parseMessage)
bool Message::deserialize(const std::vector<std::byte> data, size_t size,
                          Message& out) {

    MessageFormat format;
    std::memcpy(&format, data.data(), sizeof(format));

    bool rtn = false;

    // Deserialize payload based on format
    MessagePayload payload;
    switch (format) {
    case MESSAGE_FORMAT_MOTOR_STATE: {
        rtn = parseMessage<MotorState>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_DOOR: {
        rtn = parseMessage<SciToolDoorMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_HEIGHT: {
        rtn = parseMessage<SciToolHeightMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_SCI_TOOL_BRUSH: {
        rtn = parseMessage<SciToolBrushMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_CAMERA_SERVO: {
        rtn = parseMessage<CameraServoMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_DRIVE_ZERO: {
        rtn = parseMessage<DriveZeroMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_ARM_ZERO: {
        rtn = parseMessage<ArmZeroMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_HEADLIGHTS: {
        rtn = parseMessage<HeadlightMessage>(data, size, payload);
        break;
    }
    case MESSAGE_FORMAT_GENERIC: { // Generic or unknown
        rtn = parseMessage<Generic>(data, size, payload);
        break;
    }
    default:
        rtn = false;
    }

    if (rtn == true) {
        out.m_format = format;
        out.m_payload = payload;
    }

    return rtn;
}
