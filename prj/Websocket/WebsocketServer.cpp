#include "WebsocketServer.h"

static const char* file = "WebsocketServer";

using namespace boost;
using tcp = asio::ip::tcp;
namespace websocket = beast::websocket;

// Constructor
WebSocketServer::WebSocketServer(unsigned short port) :
    acceptor(ioc, tcp::endpoint(tcp::v4(), port)) { }

// Run the WebSocket server, sending messages from the queue
void WebSocketServer::run(MessageQueue& queue) {
    accept_connections(queue);
    ioc.run();
}

// Accept client WebSocket connections, passing the MessageQueue reference to
// each session
void WebSocketServer::accept_connections(MessageQueue& queue) {
    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);
        std::thread(&WebSocketServer::handle_session, this, std::move(socket),
                    std::ref(queue))
            .detach();
    }
}

// Handle a single WebSocket session with a connected client
void WebSocketServer::handle_session(tcp::socket socket, MessageQueue& queue) {
    try {
        // Create a WebSocket stream from the socket and accept the connection
        websocket::stream<tcp::socket> ws(std::move(socket));
        ws.accept();

        while (true) {
            // Pop the next message from the queue (blocks if empty)
            Message msg = queue.pop();

            // Serialize Message object to a string
            std::string serializedMsg = msg.serialize();

            // Set the message type to text and send the serialized message to
            // the client
            ws.text(true);
            ws.write(asio::buffer(serializedMsg));
        }
    } catch (const std::exception& e) {
        std::cerr << "Session disconnect: " << e.what() << "\n";
    }
}