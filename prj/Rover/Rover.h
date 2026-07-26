#pragma once

#include <vector>

#include "MotorStateManager.h"
#include "UDPHandler.h"
#include "mission_control.h"

#if EXTENTION == EXTENTION_TYPE_ARM
#include "Rover/Handlers/ArmHandler.h"
#elif EXTENTION == EXTENTION_TYPE_SCIENCE_TOOL
#include "Rover/Handlers/SciToolHandler.h"
#endif
#include "Rover/Handlers/DriveHandler.h"
#include "Rover/Handlers/HeadlightHandler.h"

class Rover {
public:
    static void initialize();

    static void start();

private:
    /* Static member variables whose value is known at compile time can be declared inline instead of in cpp file. */
    inline static auto currentMotorStateManager
        = std::make_shared<MotorStateManager>();
    inline static auto processes
        = std::vector<std::thread>(); // Track instantiated processes

    /* Configure handler message queue sizes as needed. */
    inline static auto receiveQueue = std::make_shared<UDPQueue>(
        10); // Size should be large enough to handle network burst
    inline static auto sendQueue = std::make_shared<UDPQueue>(
        10); // Size should be large enough for multiple senders

#if EXTENTION == EXTENTION_TYPE_ARM
    inline static auto armQueue
        = std::make_shared<ArmQueue>(1); // Out of date messages are irrelevant
    static std::shared_ptr<ArmHandler> armHandler;
    static void initializeArm();
#elif EXTENTION == EXTENTION_TYPE_SCIENCE_TOOL
    inline static auto sciToolQueue = std::make_shared<SciToolQueue>(
        1); // Not sure what sci tool queue size should be yet
    static std::shared_ptr<SciToolHandler> sciToolHandler;
    static void initializeSciTool();
#endif
    inline static auto driveQueue = std::make_shared<DriveQueue>(
        1); // Out of date messages are irrelevant
    static std::shared_ptr<DriveHandler> driveHandler;
    static void initializeDrive();

    inline static auto headlightQueue = std::make_shared<HeadlightQueue>(
        5); // Headlight commands may come sequentially but should be fast
    static std::shared_ptr<HeadlightHandler> headlightHandler;
    static void initializeHeadlights();
};
