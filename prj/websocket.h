#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// Type aliases for convenience
using WebSocketServer = websocketpp::server<websocketpp::config::asio>;
using ConnectionHandle = websocketpp::connection_hdl;

/* Represents a message exchanged between the WebSocket server and clients */
class WebSocketMessage {
public:
    /* Constructs a WebSocketMessage object
     *
     * args:
     * messageType (int) - The type of the message
     * messagePriority (int) - Priority of the message (0 = low, 1 = high)
     * payload (std::vector<int>) - The data payload of the message
     */
    WebSocketMessage(int messageType, int messagePriority,
                     const std::vector<int>& payload) :
        messageType(messageType), messagePriority(messagePriority),
        payload(payload) { }

    /* Checks if the message is marked as a priority
     *
     * args:
     * none
     *
     * returns:
     * (bool) true if the message is high priority, false otherwise
     */
    bool isPriority() const { return messagePriority != 0; }

    /* Prints the message details for debugging
     *
     * args:
     * none
     *
     * returns:
     * none
     */
    void printMessage() const {
        std::cout << "Type: " << messageType
                  << ", Priority: " << messagePriority << ", Data: ";
        for (const auto& dataElement : payload) {
            std::cout << dataElement << " ";
        }
        std::cout << std::endl;
    }

private:
    int messageType;          // The type of the message
    int messagePriority;      // Priority flag (0 = low, 1 = high)
    std::vector<int> payload; // Payload or data of the message
};

/* Represents a WebSocket server for communication with clients */
class WebSocket {
public:
    /* Constructs a WebSocket server instance */
    WebSocket();

    /* Destructs the WebSocket server instance */
    ~WebSocket();

    /* Starts the WebSocket server on the specified port
     *
     * args:
     * port (int) - The port on which to run the server
     *
     * returns:
     * none
     */
    void runServer(int port);

    /* Sends a message to the connected client
     *
     * args:
     * message (const WebSocketMessage&) - The message to send
     *
     * returns:
     * none
     */
    void sendMessageToClient(const WebSocketMessage& message);

    /* Sends a message to the server (placeholder for client-side functionality)
     *
     * args:
     * message (const WebSocketMessage&) - The message to send to the server
     *
     * returns:
     * none
     */
    void sendMessageToServer(const WebSocketMessage& message);

private:
    /* Handles incoming messages from clients
     *
     * args:
     * serverInstance (WebSocketServer*) - Pointer to the WebSocket server
     * instance connection (ConnectionHandle) - The connection handle for the
     * client message (WebSocketServer::message_ptr) - The message received from
     * the client
     *
     * returns:
     * none
     */
    void onMessage(WebSocketServer* serverInstance, ConnectionHandle connection,
                   WebSocketServer::message_ptr message);

    WebSocketServer webSocketServer;       // WebSocket server instance
    mutable ConnectionHandle activeClient; // Current active client connection

    std::queue<WebSocketMessage>
        messageQueue;      // Queue for storing received messages
    std::mutex queueMutex; // Mutex for thread-safe access to the queue
};

#endif // WEB_SOCKET_H