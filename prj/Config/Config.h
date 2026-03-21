
#pragma once
#include <string>

struct MqttConfig {
    std::string serverUrl = "";
    std::string clientId = "";
    std::string topic = "";
};

struct WebsocketConfig {
    std::string address = "";
    std::string port = "";
};

class Config {
public:
    Config() = default;
    explicit Config(const std::string& configFile);
    ~Config() = default;

    MqttConfig mqttConfig;
    WebsocketConfig websocketConfig;
};
