#pragma once
#include "MqttPublisher.h"


#if MQTT_ENABLED

#include <iostream>
#include <stdexcept>
#include <type_traits>

template <typename T>
bool MqttPublisher::publish(const std::string& topic, const T& data) noexcept {
    static_assert(std::is_trivially_copyable_v<T>,
                  "raw-byte MQTT serialization requires a trivially copyable type");
    return publishBytes(topic, &data, sizeof(T));
}
#endif