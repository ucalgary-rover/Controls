#pragma once

#include "mission_control.h"

#if MQTT_ENABLED

#include "Config/Config.h"
#include <cstddef>
#include <memory>
#include <mqtt/async_client.h>
#include <mutex>
#include <string>

class MqttPublisher {
public:
    explicit MqttPublisher(const MqttConfig& cfg) noexcept;
    ~MqttPublisher() = default;

    // Returns false if the payload could not be handed to the broker.
    // Never throws.
    template <typename T>
    bool publish(const std::string& topic, const T& data) noexcept;

    static bool isReady() noexcept;
    static void shutdown() noexcept;

private:
    static bool initialize(const std::string& serverUrl,
                           const std::string& clientId) noexcept;
    static bool publishBytes(const std::string& topic, const void* data,
                             std::size_t size) noexcept;

    static std::unique_ptr<mqtt::async_client> client_;
    static std::mutex mutex_;
    static bool initialized_;
};

#include "MqttPublisher.tpp"

#endif