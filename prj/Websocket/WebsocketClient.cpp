#include "WebsocketClient.h"

using namespace boost;
using tcp = asio::ip::tcp;
namespace websocket = beast::websocket;

// Constructor
WebSocketClient::WebSocketClient(const std::string& host,
                                 const std::string& port) :
    host(host), port(port), ws(ioc) { }

// Connect to the WebSocket server
void WebSocketClient::connect() {
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);
    asio::connect(ws.next_layer(), results.begin(), results.end());
    ws.handshake(host, "/");
}

// Send a message to the WebSocket server
void WebSocketClient::send(const std::string& message) {
    ws.write(asio::buffer(message));
}

// Receive a serialized Message from the WebSocket server
Message WebSocketClient::receive() {
    beast::flat_buffer buffer;
    ws.read(buffer);
    std::string received = beast::buffers_to_string(buffer.data());

    // Deserialize the Message object
    Message msg = deserialize(received);

    return msg;
}

// Close the WebSocket connection
void WebSocketClient::close() { ws.close(websocket::close_code::normal); }

// Deserialize a string to a Message object
Message WebSocketClient::deserialize(const std::string& data) {
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
        iss >> wm.velocity >> wm.theta >> wm.angleVelocity;
        payload = wm;
        break;
    }
    case MESSAGE_FORMAT_ARM: {
        ArmMessage am;
        int typeInt;
        iss >> typeInt;
        am.type = static_cast<ArmMessageType>(typeInt);
        switch (am.type) {
        case ARM_MESSAGE_TYPE_MANUAL: {
            int motorId;
            float angleChange;
            iss >> motorId >> angleChange;
            am.manual_message.motorId = static_cast<MotorID>(motorId);
            am.manual_message.angleChange = angleChange;
            break;
        }
        case ARM_MESSAGE_TYPE_FIXED_IK: {
            float wristX, wristY, wristZ, clawOpen;
            iss >> wristX >> wristY >> wristZ >> clawOpen;
            am.fixed_ik_message.wristX = wristX;
            am.fixed_ik_message.wristY = wristY;
            am.fixed_ik_message.wristZ = wristZ;
            am.fixed_ik_message.clawOpen = clawOpen;
            break;
        }
        case ARM_MESSAGE_TYPE_VARIABLE_IK: {
            float wristX, wristY, wristZ, clawPitch, clawRoll, clawOpen;
            iss >> wristX >> wristY >> wristZ >> clawPitch >> clawRoll
                >> clawOpen;
            am.variable_ik_message.wristX = wristX;
            am.variable_ik_message.wristY = wristY;
            am.variable_ik_message.wristZ = wristZ;
            am.variable_ik_message.clawPitch = clawPitch;
            am.variable_ik_message.clawRoll = clawRoll;
            am.variable_ik_message.clawOpen = clawOpen;
            break;
        }
        default:
            break;
        }
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
    msg.set_is_high_priority(isHighPriority);
    msg.set_format(format);
    msg.set_payload(payload);
    return msg;
}