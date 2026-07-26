#include "SciToolHandler.h"

SciToolHandler::SciToolHandler(std::shared_ptr<SciToolQueue> sciToolQueue) {
    m_sciToolQueue = sciToolQueue;
    // TODO: Initialize Science Tool Motors
    // Please follow the same style as with Drive systems and start with MockHardware for testing
}

void SciToolHandler::start() {
    // TODO: Start SciToolHandling
    while (true) {
        Message ScienceToolMessage = m_sciToolQueue->pop();
    }
}