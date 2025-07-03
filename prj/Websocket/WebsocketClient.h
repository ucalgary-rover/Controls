#pragma once
#include "Message.h"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <string>

class WebSocketClient {
public:
    /** Constructor for WebSocketClient
     *
     * @param
     *  host: const std::string& - The server host address (e.g., "127.0.0.1")
     *  port: const std::string& - The server port (e.g., "8080")
     *
     * Example Usage:
     *   WebSocketClient client("127.0.0.1", "8080");
     */
    WebSocketClient(const std::string& host, const std::string& port);

    /** Connects to the WebSocket server
     *
     * @param
     *  none
     *
     * @return
     *  none
     */
    void connect();

    /** Sends a string message to the server
     *
     * @param
     *  message: const std::string& - The message to send to the server
     *
     * @return
     *  none
     */
    void send(const std::string& message);

    /** Receives a message from the server
     *
     * @param
     *  none
     *
     * @return
     *  Message - The received message as a Message object
     */
    Message receive();

    /** Closes the WebSocket connection
     *
     * @param
     *  none
     *
     * @return
     *  none
     */
    void close();

private:
    std::string host;
    std::string port;
    boost::asio::io_context ioc; // Boost ASIO IO context
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket>
        ws; // WebSocket stream
};
