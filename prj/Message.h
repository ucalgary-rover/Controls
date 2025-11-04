#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include "pub_general.h"
#include <iostream>
#include <sstream>
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

    /** Serializes the Message object to a string
     *
     * @return
     *  std::string - The serialized message
     */
    std::string serialize() const;

    /** Returns the payload of the message
     *
     * @return
     *  MessagePayload - The payload of the message
     */
    MessagePayload get_payload() { return m_payload; }

    /** Sets isHighPriority of the message
     *
     * @param
     *  isHighPriority: bool - The priority of the message
     *
     * @return
     *  none
     */
    void set_is_high_priority(bool isHighPriority) {
        m_isHighPriority = isHighPriority;
    }

    /** Sets the format of the message
     *
     * @param
     *  format: MessageFormat - The format of the message
     *
     * @return
     *  none
     */
    void set_format(MessageFormat format) { m_format = format; }

    /** Sets the payload of the message
     *
     * @param
     *  payload: MessagePayload - The payload of the message
     *
     * @return
     *  none
     */
    void set_payload(MessagePayload payload) { m_payload = payload; }

private:
    bool m_isHighPriority;    // Priority of message
    MessagePayload m_payload; // One of the struct messages
    MessageFormat m_format;
};

#endif