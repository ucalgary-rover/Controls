#include "WebsocketClient.h"

int main() {
    WebSocketClient client("127.0.0.1", "8080"); // Localhost and port 8080
    client.connect();

    while (true) {
        Message reply = client.receive();
        reply.printMessage(); // Print the received message
    }

    client.close();
}
