
#pragma once
#include <string>

struct MqttConfig {
    std::string serverUrl;
    std::string clientId;
    std::string topic;
};

struct WebsocketConfig {
    std::string address;
    std::string port;
};

class Config {
private:
public:
    Config();
    Config(std::string configFile);
    ~Config() = default;

    MqttConfig mqttConfig;
    WebsocketConfig websocketConfig;
};
