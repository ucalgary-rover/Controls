#pragma once
#include "MqttPublisher.h"

template <typename T>
const char* serialize(T* pd) {
    const char* byteArray = reinterpret_cast<const char*>(pd);

    return byteArray;
}

template <typename U>
void MqttPublisher::publish(const std::string& topic, U data) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!client_ || !client_->is_connected()) {
        throw std::runtime_error("MQTT client not connected");
    }

    try {
        mqtt::message_ptr msg = mqtt::make_message(topic, serialize(&data),
                                                   sizeof(data), 0, false);
        client_->publish(msg)->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT publish failed: " << exc.what() << std::endl;
        throw;
    }
}