#ifndef LOGGING_H
#define LOGGING_H

#pragma once

#include "mission_control.h"
#include <iostream>

namespace Logging{
    /**
     * @brief Initialize the logging system.
     * 
     * This function sets up the logging system, including any necessary configurations.
     */
    void logInit();

    /**
     * @brief Deinitialize the logging system.
     * 
     * This function cleans up the logging system, releasing any resources that were allocated.
     */
    void logDeinit();

    /**
     * @brief Log an error message.
     * 
     * @param msg The message to log.
     * @param msg_prefix The prefix for the message.
     */
    void logE(const char* msg_prefix, const char* msg, ...);

    /**
     * @brief Log an warning message.
     * 
     * @param msg The message to log.
     * @param msg_prefix The prefix for the message.
     */
    void logW(const char* msg_prefix, const char* msg, ...);

    /**
     * @brief Log an info message.
     * 
     * @param msg The message to log.
     * @param msg_prefix The prefix for the message.
     */
    void logI(const char* msg_prefix, const char* msg, ...);

    /**
     * @brief Log a debug message.
     * 
     * @param msg The message to log.
     * @param msg_prefix The prefix for the message.
     */
    void logD(const char* msg_prefix, const char* msg, ...);

    /**
     * @brief Log a verbose message.
     * 
     * @param msg The message to log.
     * @param msg_prefix The prefix for the message.
     */
    void logV(const char* msg_prefix, const char* msg, ...);

};

#endif // LOGGING_H