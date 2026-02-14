#include "UDPHandler.h"

using namespace boost;
using udp = asio::ip::udp;

// Constructor
UDPHandler::UDPHandler(unsigned short myPort, unsigned short theirPort) :
    mySocket(ioc, udp::endpoint(udp::v4(), myPort)),
    theirEndpoint(udp::v4(), theirPort) {
    received.resize(RECEIVE_BUFFER_SIZE);
}

// Prepare the UDP sender
void UDPHandler::run(MessageQueue& queue) {
    ;
    std::thread(&UDPHandler::handle_session, this, std::ref(queue)).detach();

    // Create some fake asio tasks - prevent the context from finishing
    asio::io_context::work idleWork(ioc);
    ioc.run();
}

// Receive a single serialized Message
Message UDPHandler::receive() {

    int bytesReceived;

    asio::mutable_buffer receiveBuffer
        = asio::buffer(received.data(), received.size());

    bytesReceived = mySocket.receive_from(receiveBuffer, theirEndpoint);

    // Deserialize the buffer into a Message object
    Message msg = Message::deserialize(received, bytesReceived);
    // msg.printMessage(); // Print message received
    return msg;
}

// Continuously send messages from the queue over UDP
void UDPHandler::handle_session(MessageQueue& queue) {

    while (true) {
        if (!queue.empty()) {
            // Pop the next message from the queue (blocks if empty)
            Message msg = queue.pop();

            // Serialize Message object to a byte vector
            std::vector<std::byte> serializedMsg = msg.serialize();
            asio::mutable_buffer msgBuffer = asio::buffer(serializedMsg);

            // Send the serialized message to the client
            size_t returned = mySocket.send_to(msgBuffer, theirEndpoint);

            // Print the sent message
            Message out = Message::deserialize(serializedMsg, returned);
        }
    }
}