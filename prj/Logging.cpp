#include "Logging.h"
#include "pub_general.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

namespace Logging{
    static std::ofstream logFile;

    namespace {
        void logMsg(const std::string& levelStr, const std::string& msg, const std::string& msg_prefix) {
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm* tm_ptr = std::localtime(&now_c);
            std::ostringstream oss;
            oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") << " " 
                << levelStr << ": \t" << msg_prefix << " - " << msg << "\n";
            std::string out = oss.str();
            std::cout << out;
            if (logFile.is_open()){ 
                logFile << out;
                logFile.flush(); // Ensure the log is written immediately
            }
        }
    }

    void logInit(){
        // Create a file to store logs
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* tm_ptr = std::localtime(&now_c);
        std::ostringstream oss;
        oss << std::put_time(tm_ptr, "%Y-%m-%d_%H-%M-%S");
        std::string timestamp = oss.str();
        std::string filename;
        #if SIDE_TO_BUILD == BUILD_SIDE_BASE
        filename = "logs/base_" + timestamp + ".log";
        logFile.open(filename, std::ios::app);
        std::cout << "Logging initialized for Base side. File: " << filename << std::endl;
        #elif SIDE_TO_BUILD == BUILD_SIDE_ROVER
        filename = "logs/rover_" + timestamp + ".log";
        logFile.open(filename, std::ios::app);
        std::cout << "Logging initialized for Rover side. File: " << filename << std::endl;
        #endif

        if (!logFile.is_open()) {
            std::cout << "Failed to open log file: " << filename << std::endl;
        }
    }

    void logDeinit(){
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void logE(const std::string& msg, const std::string& msg_prefix){
    #if LOGGING_LEVEL >= LOG_LEVEL_ERROR
        logMsg("ERROR", msg, msg_prefix);
    #endif
    }

    void logW(const std::string& msg, const std::string& msg_prefix){
    #if LOGGING_LEVEL >= LOG_LEVEL_WARNING
        logMsg("WARNING", msg, msg_prefix);
    #endif
    }

    void logI(const std::string& msg, const std::string& msg_prefix){
    #if LOGGING_LEVEL >= LOG_LEVEL_INFO
        logMsg("INFO", msg, msg_prefix);
    #endif
    }

    void logD(const std::string& msg, const std::string& msg_prefix){
    #if LOGGING_LEVEL >= LOG_LEVEL_DEBUG
        logMsg("DEBUG", msg, msg_prefix);
    #endif
    }

    void logV(const std::string& msg, const std::string& msg_prefix){
        std::cout << LOGGING_LEVEL << std::endl;
        std::cout << LOG_LEVEL_VERBOSE << std::endl;
    #if LOGGING_LEVEL >= LOG_LEVEL_VERBOSE
        logMsg("VERBOSE", msg, msg_prefix);
    #endif
    }
}