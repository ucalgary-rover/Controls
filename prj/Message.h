#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include "RoverState/MotorState.h"
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
using MessagePayload = std::variant<Generic, MotorState, SciToolDoorMessage,
                                    SciToolHeightMessage, SciToolBrushMessage,
                                    HeadlightMessage, DriveZeroMessage>;

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
    Message(MessagePayload payload);
    Message();
    Message(Message const& src);
    Message& operator=(const Message& src);
    ~Message() = default;

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

    MessagePayload getPayload() const;

    /** Serializes the Message object to a string
     *
     * @return
     *  std::vector<std::byte> - The serialized message
     */
    std::vector<std::byte> serialize() const;

    /** Deserializes a string to a Message object
     *
     * @param
     *  data: const std::vector<std::byte> - The serialized message byte vector
     *  size: size_t - The size of the message to deserialize - the message will
     * be deserialized from the start of the byte vector until this number of
     * bytes has been deserialized
     *
     * @return
     *  Message - The deserialized Message object
     */
    static Message deserialize(const std::vector<std::byte> data, size_t size);

private:
    MessageFormat m_format;
    MessagePayload m_payload; // One of the struct messages
};

#endif