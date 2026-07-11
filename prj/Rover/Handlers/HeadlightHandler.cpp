#include "HeadlightHandler.h"

HeadlightHandler::HeadlightHandler(
    std::shared_ptr<HeadlightQueue> headlightQueue) : HandlerInterface() {
    m_headlightQueue = headlightQueue;
}

void HeadlightHandler::start() {
    while (true) {
        Message headlightMessage = m_headlightQueue->pop();
    }
}