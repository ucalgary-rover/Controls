#include "Message.h"

// Constructor
Message::Message(int prty, int frmt, const std::vector<int>& instr) :
    m_priority(prty), m_format(frmt), m_instructions(instr) { }

// Default Constructor
Message::Message() {
    m_priority = 0;
    m_format = 0;
    m_instructions = std::vector<int>(0);
}

// Copy Constructor
Message::Message(Message const& src) :
    m_priority(src.m_priority), m_format(src.m_format),
    m_instructions(src.m_instructions) { }

// Destructor
Message::~Message() { }

// Assignment Operator
Message& Message::operator=(const Message& src) {
    if (this != &src) {
        m_priority = src.m_priority;
        m_format = src.m_format;
        m_instructions = src.m_instructions;
    }
    return *this;
}

bool Message::isPriority() const { return m_priority; }

// Made by ChatGPT
void Message::printMessage() const {
    std::cout << "Priority: " << m_priority << ", Format: " << m_format
              << ", Instructions: ";
    for (const auto& instr : m_instructions) {
        std::cout << instr << " ";
    }
    std::cout << std::endl;
}
