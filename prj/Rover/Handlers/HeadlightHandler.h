#include "HandlerInterface.h"

#include "Message.h"
#include "MessageQueue.h"
#include <memory>

using HeadlightQueue = MessageQueue<Message>; // Change type as needed

class HeadlightHandler : public HandlerInterface {
public:
    HeadlightHandler(std::shared_ptr<HeadlightQueue> headlightQueue);

    void start() override;

private:
    std::shared_ptr<HeadlightQueue> m_headlightQueue;
};