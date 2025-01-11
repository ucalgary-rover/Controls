#include "Message.h"

Message::Message(int prty, int frmt, const std::vector<int> &instr)
    : m_priority(prty), m_format(frmt), m_instructions(instr) {} // I think we would need to create a deep copy of m_instructions

bool Message::isPriority() const
{
    return m_priority;
}

// Made by ChatGPT
void Message::printMessage() const
{
    std::cout << "Priority: " << m_priority
              << ", Format: " << m_format
              << ", Instructions: ";
    for (const auto &instr : m_instructions)
    {
        std::cout << instr << " ";
    }
    std::cout << std::endl;
}
