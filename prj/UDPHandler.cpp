#include "UDPHandler.h"

using namespace boost;
using udp = asio::ip::udp;

// Constructor
UDPHandler::UDPHandler(unsigned short myPort, unsigned short theirPort,
                       std::string theirIP) :
    mySocket(ioc, udp::endpoint(udp::v4(), myPort)),
    theirEndpoint(boost::asio::ip::make_address(theirIP), theirPort) {
    received.resize(RECEIVE_BUFFER_SIZE);
}

// Prepare the UDP sender
void UDPHandler::run(std::shared_ptr<UDPQueue> queue) {
    std::thread(&UDPHandler::handle_session, this, std::ref(queue)).detach();

    // Create some fake asio tasks - prevent the context from finishing
    auto work = boost::asio::make_work_guard(ioc);
    ioc.run();
}

// Receive a single serialized Message
Message UDPHandler::receive() {

    int bytesReceived;
    udp::endpoint senderEndpoint;

    asio::mutable_buffer receiveBuffer
        = asio::buffer(received.data(), received.size());

    while (true) {
        bytesReceived = mySocket.receive_from(receiveBuffer, senderEndpoint);

        // Deserialize the buffer into a Message object
        Message msg;
        if (!Message::deserialize(received, bytesReceived, msg)) {
            continue;
        }
        // msg.printMessage(); // Print message received
        return msg;
    }
}

// Continuously send messages from the queue over UDP
void UDPHandler::handle_session(std::shared_ptr<UDPQueue> queue) {

    while (true) {
        // Pop the next message from the queue (blocks if empty)
        Message msg = queue->pop();

        // Serialize Message object to a byte vector
        std::vector<std::byte> serializedMsg = msg.serialize();
        asio::mutable_buffer msgBuffer = asio::buffer(serializedMsg);

        // Send the serialized message to the client
        size_t returned = mySocket.send_to(msgBuffer, theirEndpoint);
    }
}