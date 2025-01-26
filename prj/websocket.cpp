#include "websocket.h"
#include <iostream>
#include <string>
#include <functional> // For std::bind

/* Constructs a WebSocketServer instance */
WebSocket::WebSocket() {}

/* Destructs a WebSocketServer instance */
WebSocket::~WebSocket() {}

/* Starts the WebSocket server on the specified port
 *
 * args:
 * port (int) - The port on which to start the server
 *
 * returns:
 * none
 */
void WebSocket::runServer(int port)
{
    try
    {
        // Initialize ASIO
        webSocketServer.init_asio();

        // Set up open handler
        webSocketServer.set_open_handler([this](ConnectionHandle connectionHandle)
                                         {
            activeClient = connectionHandle;
            std::cout << "Client connected." << std::endl; });

        // Set up close handler
        webSocketServer.set_close_handler([this](ConnectionHandle connectionHandle)
                                          {
            if (connectionHandle.lock() == activeClient.lock())
            {
                activeClient.reset();
                std::cout << "Client disconnected." << std::endl;
            } });

        // Set up message handler
        webSocketServer.set_message_handler(
            [this](ConnectionHandle connectionHandle, WebSocketServer::message_ptr msg)
            {
                this->onMessage(&webSocketServer, connectionHandle, msg);
            });

        // Start the server
        webSocketServer.listen(port);
        webSocketServer.start_accept();
        std::cout << "WebSocket server started on port " << port << std::endl;

        webSocketServer.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "WebSocket server error: " << e.what() << std::endl;
    }
}

/* Handles incoming messages from the client
 *
 * args:
 * serverInstance (WebSocketServer*) - Pointer to the WebSocket server instance
 * connectionHandle (ConnectionHandle) - Connection handle for the client
 * msg (WebSocketServer::message_ptr) - The message received from the client
 *
 * returns:
 * none
 */
void WebSocket::onMessage(WebSocketServer *serverInstance, ConnectionHandle connectionHandle, WebSocketServer::message_ptr msg)
{
    try
    {
        std::string payload = msg->get_payload();
        std::cout << "Message received from client: " << payload << std::endl;

        // Simulate converting payload to a Message object
        WebSocketMessage receivedMessage(0, 0, {1, 2, 3}); // Decoy value please replace
        messageQueue.push(receivedMessage);

        // Create a response message
        WebSocketMessage responseMessage(1, 1, {4, 5, 6}); // Decoy value please replace 
        sendMessageToClient(responseMessage);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing message: " << e.what() << std::endl;
    }
}

/* Sends a message to the connected client
 *
 * args:
 * message (const WebSocketMessage&) - The message to send to the client
 *
 * returns:
 * none
 */
void WebSocket::sendMessageToClient(const WebSocketMessage &message)
{
    try
    {
        if (!activeClient.expired())
        {
            auto connection = activeClient.lock();
            std::string serializedMessage = "Priority:" + std::to_string(message.isPriority());

            webSocketServer.send(connection, serializedMessage, websocketpp::frame::opcode::text);
            std::cout << "Message sent to client: " << serializedMessage << std::endl;
        }
        else
        {
            std::cerr << "No active client connection to send the message." << std::endl;
        }
    }
    catch (const websocketpp::exception &e)
    {
        std::cerr << "WebSocket error while sending message: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "General error while sending message: " << e.what() << std::endl;
    }
}

/* Sends a message to the server (placeholder for client-side functionality)
 *
 * args:
 * message (const WebSocketMessage&) - The message to send to the server
 *
 * returns:
 * none
 */
void WebSocket::sendMessageToServer(const WebSocketMessage &message)
{
    try
    {
        std::string serializedMessage = "Priority:" + std::to_string(message.isPriority());
        // Logic for client-side WebSocket to send messages to the server goes here
        std::cout << "Sending message to server: " << serializedMessage << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error sending message to server: " << e.what() << std::endl;
    }
}