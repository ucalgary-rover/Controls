
#include "Config.h"
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <iostream>
#include <string>

std::string readConfigFile(const std::string& configFile) {
    std::ifstream file(configFile);

    if (!file) {
        // Handle error: file not found or could not be opened
        std::cerr << "Error opening file: " << configFile << std::endl;
        return ""; // Or throw an exception
    }

    // Use iterators to read the entire file stream into a string
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    // The file stream is automatically closed when 'file' goes out of scope
    // (due to RAII)
    return content;
}

BOOST_DESCRIBE_STRUCT(MqttConfig, (), (serverUrl, clientId, topic));
BOOST_DESCRIBE_STRUCT(WebsocketConfig, (), (address, port));
BOOST_DESCRIBE_STRUCT(Config, (), (mqttConfig, websocketConfig));

Config::Config() {
    this->mqttConfig = { .serverUrl = "", .clientId = "", .topic = "" };
    this->websocketConfig = { .address = "", .port = "" };
}

Config::Config(std::string configFile) {
    std::string content = readConfigFile(configFile);

    boost::json::value jv = boost::json::parse(content);

    *this = boost::json::value_to<Config>(jv);

    std::cout << "address: " << this->mqttConfig.serverUrl << std::endl;
    std::cout << "client id: " << this->mqttConfig.clientId << std::endl;
    std::cout << "topic: " << this->mqttConfig.topic << std::endl;

    std::cout << std::endl
              << "address: " << this->websocketConfig.address << std::endl;
    std::cout << "port: " << this->websocketConfig.port << std::endl;
}

// int main() { Config cfg = Config("prj/Config/config.json"); }
