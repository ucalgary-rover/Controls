#include "HandlerInterface.h"

#include "Message.h"
#include "MessageQueue.h"
#include <memory>

using SciToolQueue = MessageQueue<Message>; // Change type as needed

class SciToolHandler : public HandlerInterface {
public:
    SciToolHandler(std::shared_ptr<SciToolQueue> sciToolQueue);

    void start() override;

private:
    std::shared_ptr<SciToolQueue> m_sciToolQueue;
};