#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <iostream>
#include <stdint.h>
#include <variant>
#include <vector>

// Commented out section below allows multiple message types (structs) while
// letting Message have the same format
/*
struct WheelMessage {
    int velocity;
    int theta;
    int angle_velocity;
};

struct ArmMessage {
    int armXPos;
    int armYPos;
};

struct ClawMessage {
    int clawOpenDistance;
    int rotation;
    int tilt;
};

// Allows for different message formats
using MessagePayload = std::variant<WheelMessage, ArmMessage, ClawMessage>;
*/

class Message {
public:
    Message(int prty, int frmt, const std::vector<int>& instr);
    Message();
    Message(Message const& src);
    ~Message();
    Message& operator=(const Message& src);

    bool isPriority() const;
    void printMessage() const;

private:
    bool m_priority;
    // MessagePayload m_payload;
    int m_format;
    std::vector<int> m_instructions;
};

#endif