#pragma once
#include "Message.h"
#include "MessageQueue.h"
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#define RECEIVE_BUFFER_SIZE 100

using UDPQueue = MessageQueue<Message>;

class UDPHandler {
public:
    /** Constructor for UDPReceiver
     *
     * @param
     *  myPort: unsigned short - The port the current instantiation should use
     * (e.g., 8080) theirPort: unsigned short - The port to communicate to
     * (e.g., 8008)
     *
     * Example Usage:
     *   UDPReceiver client(8080, 8008);
     */
    UDPHandler(unsigned short myPort, unsigned short theirPort);

    /** Prepares the UDP handler
     *
     * @param
     *  queue: std::shared_ptr<UDPQueue> - The queue of messages to send
     *
     * @return
     *  none
     */
    void run(std::shared_ptr<UDPQueue> queue);

    /** Continuously sends messages from the queue over UDP
     *
     * @param
     *  queue: std::shared_ptr<UDPQueue> - The queue of messages to send
     *
     * @return
     *  none
     */
    void handle_session(std::shared_ptr<UDPQueue> queue);

    /** Receives a message from the server
     *
     * @param
     *  none
     *
     * @return
     *  Message - The received message as a Message object
     */
    Message receive();

private:
    boost::asio::io_context ioc; // Boost ASIO IO context

    boost::asio::ip::udp::socket mySocket;
    boost::asio::ip::udp::endpoint
        theirEndpoint; // Endpoint includes port and IP address

    std::vector<std::byte> received;
    boost::system::error_code ec;
};
