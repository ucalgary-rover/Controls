#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include "pub_general.h"
#include <iostream>
#include <stdint.h>
#include <variant>
#include <vector>

// Commented out section below allows multiple message types (structs) while
// letting Message have the same format

// Generic message format for default constructor

// Allows for different message formats
using MessagePayload
    = std::variant<Generic, WheelMessage, ArmMessage, ScienceToolMessage>;

class Message {
public:
    /** Constructor for message
     *
     * @param
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

    /** Returns if a message has priority
     *
     * @param
     * none
     *
     * @return
     * (bool) if the message has priority (True) or not (False)
     */
    bool isHighPriority() const;

    /** prints contents of Message
     *
     * @param
     * none
     *
     * @return
     * none
     */
    void printMessage() const;

    MessageFormat getFormat() const;

private:
    bool m_isHighPriority;    // Priority of message
    MessagePayload m_payload; // One of the struct messages
    MessageFormat m_format;
};

#endif