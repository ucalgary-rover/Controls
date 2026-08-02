#pragma once
#include "MqttPublisher.h"

#include <iostream>
#include <stdexcept>
#include <type_traits>

template <typename T>
const char* serialize(const T& pd) {
    static_assert(std::is_trivially_copyable_v<T>,
                  "raw-byte MQTT serialization requires a trivially copyable type");
    return reinterpret_cast<const char*>(&pd);
}

template <typename U>
void MqttPublisher::publish(const std::string& topic, const U& data) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!client_ || !client_->is_connected()) {
        throw std::runtime_error("MQTT client not connected");
    }

    try {
        mqtt::message_ptr msg =
            mqtt::make_message(topic, serialize(data), sizeof(data), 0, false);
        client_->publish(msg)->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT publish failed: " << exc.what() << std::endl;
        throw;
    }
}