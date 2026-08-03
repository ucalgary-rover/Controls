#include "SciToolHandler.h"

SciToolHandler::SciToolHandler(std::shared_ptr<SciTool> sciTool,
                               std::shared_ptr<SciToolQueue> sciToolQueue) :
    HandlerInterface() {
    m_sciTool = sciTool;
    m_sciToolQueue = sciToolQueue;
}

void SciToolHandler::start() {
    // TODO: Start SciToolHandling
    while (true) {
        Message ScienceToolMessage = m_sciToolQueue->pop();

        switch (ScienceToolMessage.getFormat()) {
        MESSAGE_FORMAT_SCI_TOOL_DOOR:
            // Handle door message

            switch (
                std::get<SciToolDoorMessage>(ScienceToolMessage.getPayload())
                    .door) {
            case LEFT:
                m_sciTool->setGateServoAngle(SCI_TOOL_INDEX_SERVO_LEFT,
                                             0); // Example angle for LEFT door
                m_sciTool->setGateServoAngle(SCI_TOOL_INDEX_SERVO_RIGHT,
                                             0); // Example angle for LEFT door
                break;

            case MIDDLE:
                m_sciTool->setGateServoAngle(SCI_TOOL_INDEX_SERVO_LEFT,
                                             0); // Example angle for LEFT door
                m_sciTool->setGateServoAngle(
                    SCI_TOOL_INDEX_SERVO_RIGHT,
                    180); // Example angle for LEFT door
                break;

            case RIGHT:
                m_sciTool->setGateServoAngle(
                    SCI_TOOL_INDEX_SERVO_RIGHT,
                    180); // Example angle for LEFT door
                m_sciTool->setGateServoAngle(
                    SCI_TOOL_INDEX_SERVO_LEFT,
                    180); // Example angle for LEFT door
                break;
            }
            break;
        MESSAGE_FORMAT_SCI_TOOL_HEIGHT:
            // Handle height message
            switch (
                std::get<SciToolHeightMessage>(ScienceToolMessage.getPayload())
                    .control) {
            case RAISE:
                m_sciTool->setLiftSpeed(-SCIENCE_LIFT_SPEED);
                break;

            case LOWER:
                m_sciTool->setLiftSpeed(SCIENCE_LIFT_SPEED);
                break;

            case STOP:
                m_sciTool->setLiftSpeed(0);
                break;
            }
            break;
        MESSAGE_FORMAT_SCI_TOOL_BRUSH:
            // Handle brush message
            switch (
                std::get<SciToolBrushMessage>(ScienceToolMessage.getPayload())
                    .control) {
            case START_BRUSH:
                m_sciTool->setBrushSpeed(SCIENCE_BRUSH_SPEED);
                break;

            case STOP_BRUSH:
                m_sciTool->setBrushSpeed(0);
                break;
            }
            break;
        }
    }
}
