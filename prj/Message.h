#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <iostream>
#include <stdint.h>
#include <variant>
#include <vector>

// Commented out section below allows multiple message types (structs) while
// letting Message have the same format

// Generic message format for default constructor
struct Generic {
    int value;
};

// Message format for wheels
struct WheelMessage {
    int velocity;
    int theta;
    int angle_velocity;
};

// Message format for arm
struct ArmMessage {
    int armXPos;
    int armYPos;
    int armZPos;

    int clawXPos;
    int clawYPos;
    int clawOpen;
    int clawRotation;

    int wristRotation;
};

// Message format for science tool
struct ScienceToolMessage {
    int moveUpDown;
    int moveLeftRight;
};

// Allows for different message formats
using MessagePayload
    = std::variant<Generic, WheelMessage, ArmMessage, ScienceToolMessage>;

class Message {
public:
    /* Constructor for message
     *
     * args:
     *  prty: int - The priority of the message
     *  payload: MessagePayload - The type of struct being put into the message
     *
     * Example Usage:
     *   Message msg1(1, WheelMessage{10, 20, 30});
     *   Message msg2(0, ArmMessage{15, 25});
     */
    Message(int prty, MessagePayload payload);
    Message();
    Message(Message const& src);
    ~Message();
    Message& operator=(const Message& src);

    /* Returns if a message has priority
     *
     * args:
     * none
     *
     * returns:
     * (bool) if the message has priority (True) or not (False)
     */
    bool isPriority() const;

    /* prints contents of Message
     *
     * args:
     * none
     *
     * returns:
     * none
     */
    void printMessage() const;

private:
    bool m_priority;          // Priority of message
    MessagePayload m_payload; // One of the struct messages
    // int m_format;
    // std::vector<int> m_instructions;
};

#endif