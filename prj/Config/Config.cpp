
#include "Config.h"
#include <boost/describe.hpp>
#include <boost/exception/all.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

static std::string readConfigFile(const std::string& configFile) {
    std::ifstream file(configFile);

    if (!file) {
        // Handle error: file not found or could not be opened
        throw std::runtime_error("Error opening config file: " + configFile);
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

Config::Config(const std::string& configFile) {
    std::string content = readConfigFile(configFile);
    boost::json::value jv;
    try {
        jv = boost::json::parse(content);
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse config file or match file contents to "
                     "config structs"
                  << std::endl;
        std::cerr << "exception: " << boost::diagnostic_information(e, true)
                  << std::endl;

        throw;
    }

    *this = boost::json::value_to<Config>(jv);
}

// int main() { Config cfg = Config("prj/Config/config.json"); }
