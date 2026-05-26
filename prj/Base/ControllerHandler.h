#ifndef CONTROLLER_HANDLER_H
#define CONTROLLER_HANDLER_H

#pragma once
#include "ArmControllerLayout.h"
#include "Controller.h"
#include "DriveControllerLayout.h"
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

// A class for initializing controllers and running the pollEvent loop
class ControllerHandler {

private:
    // init bool as per UML
    static bool is_initialized;

    static std::vector<Controller>
        controllers; //- vector<const Controller>: controllers (FROM UML)

    static std::vector<std::shared_ptr<ControllerLayout>> layouts;

    const static int controllerAmount = 2;

    // note that index = vector index, value = SDL_JOYSTICK_ID
    static int controllerAssignment[controllerAmount]; // Switched to array
                                                       // because simple is best

    static void buttonResponse(uint8_t buttonID, int controllerID);
    static void stickResponse(int16_t axisValue, int axisID, int controllerID);
    static void triggerResponse(int16_t axisValue, int axisID,
                                int controllerID);
    static void controllerAddedResponse(int controllerID);
    static void controllerRemovedResponse(int controllerID);

public:
    static bool initialize(DriveStateManager* driveStateManager,
                           DriveMotorStateManager* driveMotorStateManager,
                           ArmStateManager* armStateManager,
                           ArmMotorStateManager* armManualChangeManager);
    static void eventLoop();

    // A function for getting the vectors index from controllerAssignemnt
    static int vectorIndexFromID(int controllerID);
};

#endif