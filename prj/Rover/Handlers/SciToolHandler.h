#include "HandlerInterface.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Rover/Systems/SciTool.h"
#include "Rover/pub_rover.h"
#include <memory>

using SciToolQueue = MessageQueue<Message>; // Change type as needed

#define SCIENCE_LIFT_SPEED 20
#define SCIENCE_BRUSH_SPEED 100

class SciToolHandler : public HandlerInterface {
public:
    SciToolHandler(std::shared_ptr<SciTool> sciTool,
                   std::shared_ptr<SciToolQueue> sciToolQueue);

    void start() override;

private:
    std::shared_ptr<SciTool> m_sciTool;
    std::shared_ptr<SciToolQueue> m_sciToolQueue;
};