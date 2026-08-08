#include "MqttPublisher.h"

#if MQTT_ENABLED

#include <iostream>
#include <memory>
#include <mqtt/async_client.h>

std::unique_ptr<mqtt::async_client> MqttPublisher::client_ = nullptr;
std::mutex MqttPublisher::mutex_;
bool MqttPublisher::initialized_ = false;

// Constructor
bool MqttPublisher::initialize(const std::string& serverUrl,
                               const std::string& clientId) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_)
        return true;
    if (serverUrl.empty())
        return false;

    try {
        client_ = std::make_unique<mqtt::async_client>(serverUrl, clientId);

        mqtt::connect_options opts;
        opts.set_automatic_reconnect(true);
        opts.set_clean_session(true);

        client_->connect(opts); // not waited on; see note below
        initialized_ = true;
        return true;
    } catch (...) {
        client_.reset();
        initialized_ = false;
        return false;
    }
}

MqttPublisher::MqttPublisher(const MqttConfig& cfg) noexcept {
    initialize(cfg.serverUrl, cfg.clientId);
}

bool MqttPublisher::isReady() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_ || !client_)
        return false;
    try {
        return client_->is_connected();
    } catch (...) {
        return false;
    }
}

bool MqttPublisher::publishBytes(const std::string& topic, const void* data,
                                 std::size_t size) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_ || !client_)
        return false;

    try {
        if (!client_->is_connected())
            return false; // expected, not exceptional

        mqtt::message_ptr msg = mqtt::make_message(topic, data, size, 0, false);
        client_->publish(msg); // fire and forget
        return true;
    } catch (...) {
        return false;
    }
}

void MqttPublisher::shutdown() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (client_) {
        try {
            if (client_->is_connected()) {
                client_->disconnect()->wait_for(std::chrono::seconds(1));
            }
        } catch (...) {
            // best effort
        }
    }
    client_.reset();
    initialized_ = false;
}
#endif