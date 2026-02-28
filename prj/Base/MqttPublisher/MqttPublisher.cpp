#include "MqttPublisher.h"
#include <mqtt/async_client.h>

std::unique_ptr<mqtt::async_client> MqttPublisher::client_ = nullptr;
std::mutex MqttPublisher::mutex_;
bool MqttPublisher::initialized_ = false;

// Constructor
MqttPublisher::MqttPublisher(MqttConfig cfg) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        initialize(cfg.serverUrl, cfg.clientId);
        initialized_ = true;
    }
}

void MqttPublisher::initialize(const std::string& serverUrl,
                               const std::string& clientId) {
    try {
        client_ = std::make_unique<mqtt::async_client>(serverUrl, clientId);

        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true);

        std::cout << "Connecting to MQTT Broker..." << std::endl;
        client_->connect(connOpts)->wait();
        std::cout << "Connected." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "MQTT connection failed: " << e.what() << std::endl;
        throw;
    }
}

void MqttPublisher::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (client_ && client_->is_connected()) {
        try {
            client_->disconnect()->wait();
            std::cout << "Disconnected from MQTT broker." << std::endl;
        } catch (const mqtt::exception& exc) {
            std::cerr << "MQTT disconnect failed: " << exc.what() << std::endl;
        }
    }

    client_.reset();
    initialized_ = false;
}
