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
    Message msg = Message::deserialize(received);

    return msg;
}

// Close the WebSocket connection
void WebSocketClient::close() { ws.close(websocket::close_code::normal); }