#include "WebSocket.h"

std::mutex clientsMutex; // Mutex to synchronize access to the clients set
std::set<std::shared_ptr<websocket::stream<tcp::socket>>>
    clients; // Set to store active WebSocket client connections

// ==================== WebSocket Server Implementation ====================

/**
 * @brief Constructs a WebSocket server that listens on the specified port.
 * @param context ASIO I/O context for managing asynchronous operations.
 * @param port Port number on which the server listens for incoming connections.
 */
WebSocketServer::WebSocketServer(asio::io_context& context, int port) :
    ioContext(context), acceptor(context, tcp::endpoint(tcp::v4(), port)) { }

/**
 * @brief Starts the WebSocket server, initiating the accept loop.
 */
void WebSocketServer::run() { accept(); }

/**
 * @brief Asynchronously accepts incoming WebSocket connections.
 */
void WebSocketServer::accept() {
    acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "[Server] Client connected!\n";
                // Create a new thread to handle the client session
                std::thread(&WebSocketServer::handleSession, this,
                            std::move(socket))
                    .detach();
            }
            accept(); // Continue accepting new connections
        });
}

/**
 * @brief Handles an individual WebSocket client session.
 * @param socket TCP socket for the connected client.
 */
void WebSocketServer::handleSession(tcp::socket socket) {
    try {
        // Create a WebSocket stream from the socket
        auto ws = std::make_shared<websocket::stream<tcp::socket>>(
            std::move(socket));
        ws->accept(); // Perform the WebSocket handshake

        // Add the WebSocket connection to the clients set
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.insert(ws);
        }

        beast::flat_buffer buffer; // Buffer to hold incoming messages
        while (true) {
            ws->read(buffer); // Read a message from the client
            std::string received = beast::buffers_to_string(buffer.data());

            // Deserialize the received message
            Message msg = Message::deserialize(received);
            std::cout << "[Server] Received Message:\n";
            msg.printMessage();

            // Echo the message back to the client
            ws->text(true);
            ws->write(asio::buffer(msg.serialize()));
            buffer.consume(
                buffer.size()); // Clear the buffer for the next message
        }
    } catch (std::exception& e) {
        std::cerr << "[Server] Session error: " << e.what() << std::endl;
    }
}

// ==================== WebSocket Client Implementation ====================

/**
 * @brief Constructs a WebSocket client and connects to the specified server.
 * @param context ASIO I/O context for managing asynchronous operations.
 * @param host The server's hostname or IP address.
 * @param port The port number to connect to.
 */
WebSocketClient::WebSocketClient(asio::io_context& context,
                                 const std::string& host, int port) :
    ioContext(context), ws(context) {

    // Resolve the server address and connect
    tcp::resolver resolver(context);
    auto results = resolver.resolve(host, std::to_string(port));
    asio::connect(ws.next_layer(), results.begin(), results.end());

    // Perform WebSocket handshake
    ws.handshake(host, "/");

    // Start a separate thread to listen for incoming messages
    receiveThread = std::thread(&WebSocketClient::receiveMessages, this);
}

/**
 * @brief Sends a message to the WebSocket server.
 * @param msg The message to send.
 */
void WebSocketClient::sendMessage(const Message& msg) {
    ws.write(asio::buffer(msg.serialize()));
}

/**
 * @brief Runs the client, ensuring the receive thread is properly joined.
 */
void WebSocketClient::run() {
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

/**
 * @brief Continuously receives and processes messages from the server.
 */
void WebSocketClient::receiveMessages() {
    beast::flat_buffer buffer; // Buffer to hold received messages
    try {
        while (true) {
            ws.read(buffer); // Read a message from the server
            std::string received = beast::buffers_to_string(buffer.data());

            // Deserialize the received message
            Message msg = Message::deserialize(received);
            std::cout << "[Client] Server response:\n";
            msg.printMessage();

            buffer.consume(
                buffer.size()); // Clear the buffer for the next message
        }
    } catch (std::exception& e) {
        std::cerr << "[Client] Receive error: " << e.what() << std::endl;
    }
}