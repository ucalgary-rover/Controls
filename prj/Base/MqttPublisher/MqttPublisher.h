#pragma once
#include "Config/Config.h"
#include <memory>
#include <mqtt/async_client.h>
#include <mutex>

class MqttPublisher {
private:
    static void initialize(const std::string& serverUrl,
                           const std::string& clientId);

    static std::unique_ptr<mqtt::async_client> client_;
    static std::mutex mutex_;
    static bool initialized_;

public:
    // Constructor
    MqttPublisher(MqttConfig cfg);

    // Destructor
    ~MqttPublisher() = default;

    template <typename T>
    void publish(const std::string& topic, const T& data);

    static void shutdown();
};

#include "MqttPublisher.tpp"