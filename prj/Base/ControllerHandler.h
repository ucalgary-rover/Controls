#ifndef CONTROLLER_HANDLER_H
#define CONTROLLER_HANDLER_H

#pragma once
#include "ArmControllerLayout.h"
#include "Controller.h"
#include "DriveControllerLayout.h"
#include <cstdint>
#include <map>
#include <vector>

// A class for initializing controllers and running the pollEvent loop
class ControllerHandler {

private:
    // init bool as per UML
    static bool is_initialized;

    static std::vector<Controller>
        controllers; //- vector<const Controller>: controllers (FROM UML)

    // note that key = SDL_JOYSTICK_ID, value = index in controllers vector
    static std::map<int, int>
        controllerAssignment; //- map<int, int>: controllerAssignment (FROM UML)

    static void buttonResponse(uint8_t buttonID, int controllerID);
    static void stickResponse(int16_t axisValue, int axisID, int controllerID);
    static void triggerResponse(int16_t axisValue, int axisID,
                                int controllerID);
    static void controllerAddedResponse(int controllerID);
    static void controllerRemovedResponse(int controllerID);

public:
    static bool initialize(DriveStateManager* driveStateManager,
                           ArmStateManager* armStateManager,
                           ArmMotorStateManager* armManualChangeManager);
    static void eventLoop();
};

#endif