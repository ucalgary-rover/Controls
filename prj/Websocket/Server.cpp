#include "WebsocketServer.h"

int main() {
    MessageQueue queue;

    // Push messages into the queue
    queue.push(Message(0, Generic { 42 }));
    queue.push(Message(1, WheelMessage { 120, 45, 10 }));
    queue.push(Message(1, ArmMessage { 100, 200, 300, 50, 60, 1, 90, 180 }));

    WebSocketServer server(8080);
    server.run(queue);
}
