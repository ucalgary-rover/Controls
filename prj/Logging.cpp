#include "Logging.h"
#include "pub_general.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

static const char* file = "Base";

namespace Logging{
    static std::ofstream logFile;

    namespace {
        void logMsg(const char* levelStr, const char* msg_prefix, const char* msg, va_list args) {
            char buf[512];
            vsnprintf(buf, sizeof(buf), msg, args);

            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm* tm_ptr = std::localtime(&now_c);
            std::ostringstream oss_body;
            std::ostringstream oss_timestamp;
            oss_body << levelStr << ": \t" << msg_prefix << " - " << buf << "\n";
            oss_timestamp << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") << " ";
            std::string out_body = oss_body.str();
            std::string out_timmestamp = oss_timestamp.str();
            std::cout << out_body;
            if (logFile.is_open()){ 
                logFile << out_timmestamp << out_body;
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
        logI(file, "Logging initialized for Base side %s", filename);

        #elif SIDE_TO_BUILD == BUILD_SIDE_ROVER
        filename = "logs/rover_" + timestamp + ".log";
        logFile.open(filename, std::ios::app);
        logI(file, "Logging initialized for Rover side %s", filename);
        #endif
    }

    void logDeinit(){
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void logE(const char* msg_prefix, const char* msg, ...){
    #if LOGGING_LEVEL >= LOG_LEVEL_ERROR
        va_list args;
        va_start(args, msg);
        logMsg("ERROR", msg_prefix, msg, args);
    #endif
    }

    void logW(const char* msg_prefix, const char* msg, ...){
    #if LOGGING_LEVEL >= LOG_LEVEL_WARNING
        va_list args;
        va_start(args, msg);
        logMsg("WARNING", msg_prefix, msg, args);
    #endif
    }

    void logI(const char* msg_prefix, const char* msg, ...){
    #if LOGGING_LEVEL >= LOG_LEVEL_INFO
        va_list args;
        va_start(args, msg);
        logMsg("INFO", msg_prefix, msg, args);
    #endif
    }

    void logD(const char* msg_prefix, const char* msg, ...){
    #if LOGGING_LEVEL >= LOG_LEVEL_DEBUG
        va_list args;
        va_start(args, msg);
        logMsg("DEBUG", msg_prefix, msg, args);
    #endif
    }

    void logV(const char* msg_prefix, const char* msg, ...){
    #if LOGGING_LEVEL >= LOG_LEVEL_VERBOSE
        va_list args;
        va_start(args, msg);
        logMsg("VERBOSE", msg_prefix, msg, args);
    #endif
    }
}