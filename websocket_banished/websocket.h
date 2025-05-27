#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "Message.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <mutex>
#include <set>
#include <thread>

#pragma once

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;

// WebSocket Server Class
class WebSocketServer {
public:
    WebSocketServer(asio::io_context& context, int port);
    void run();

private:
    asio::io_context& ioContext;
    tcp::acceptor acceptor;
    void accept();
    void handleSession(tcp::socket socket);
};

// WebSocket Client Class
class WebSocketClient {
public:
    WebSocketClient(asio::io_context& context, const std::string& host,
                    int port);
    void sendMessage(const Message& msg);
    void run();

private:
    asio::io_context& ioContext;
    websocket::stream<tcp::socket> ws;
    std::thread receiveThread;
    void receiveMessages();
};

#endif // WEBSOCKET_H