#ifndef CONTROLLER_HANDLER_H
#define CONTROLLER_HANDLER_H

#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include "Controller.h"


// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHandler {

private:
    //// Holding the controller objects
    //Controller m_controllerList[2];  >>>>> OLD

    //// Holding the structs for each controller
    //buttonFunctions m_buttonFuncList[2]; >>>>> OLD

    //init bool as per UML
    static bool is_initialized;

    static std::vector<Controller> controllers; //- vector<const Controller>: controllers (FROM UML)
    static std::map<int, int> controllerAssignment; //- map<int, int>: controllerAssignment (FROM UML)

    static int getAssignedIndex(int controllerID); //index identifier for controllerID

    static void buttonResponse(uint8_t buttonID, int controllerID);
    static void stickResponse(int16_t axisValue, int axisID, int controllerID);
    static void triggerResponse(int16_t axisValue, int axisID, int controllerID);
    static void controllerAddedResponse(int controllerID);
    static void controllerRemovedResponse(int controllerID);

public:
    /**
     *Initializes SDL and maps the appropriate functions to controllers
     *
     *@param values The structs with mappings for each controller, controllers 1
     *and 2 respectively
     *
     *@return None
     *
     */

    //OLD:
    //ControllerHandler(buttonFunctions functionStruct1,
                      //buttonFunctions functionStruct2);
    //void setControllerButtonFuncs(int index, buttonFunctions functionStruct);

    // getter and setter function
    //Controller getControllerFromList(int i) {
        //return (i == 1 || i == 0) ? m_controllerList[i] : m_controllerList[0];
    //};

    //void setControllerFromList(int i, Controller controller) {
        //i = (i == 1 || i == 0) ? i : 0;
        //m_controllerList[i] = controller;
    //};
    static bool initialize(const std::vector<Controller>& controller_functionality);

    static void eventLoop();
    static void testingEventLoop();

    /**22
     *Executes a function when a button is pressed
     *
     *@param buttonID The ID of the button pressed
     *@param controllerIndex The index of the active controller for the event
     *
     *@return None
     *
     */
    //void buttonResponse(Uint8 buttonID, int controllerIndex);

    /**
     *Identifies which stick requires an update and pass in variable for the
     *appropriate stick update function
     *
     //*@param axisValue The value of the axis moved
     //*@param axisID An ID differentiating the x and y axes across both sticks
     //*@param controllerIndex The index of the active controller for the event
     *
     *
     //*@return None. Updates values directly through stick functions
     *
     */
    //void stickResponse(Sint16 axisValue, int axisID, int controllerIndex);

    /**
     *Identifies which trigger requires an update and pass in variable for the
     *appropriate trigger update function
     *
     //*@param axisValue The value of the trigger moved
     //*@param axisID An ID differentiating the left and right triggers
     //*@param controllerIndex The index of the active controller for the event
     *
     //*@return None.
     *
     */
    //void triggerResponse(Sint16 axisValue, int axisID, int controllerIndex);

    /**
     *Executes a function when a button is pressed
     *
     //*@param controllerIndex The index of the active controller for the event
     *
     //*@return None
     *
     */
    //void controllerAddedResponse(int controllerIndex);

    /**
     *Closes the removed controller and sends an update with all zero values
     *
     //*@param controllerIndex The index of the active controller for the event
     *
     //*@return None
     *
     */
    //void controllerRemovedResponse(int controllerIndex);

    /**
     *Runs the event loop to recieve any controller events. Initializes SDL
     *subsystems and closes them once the loop is terminated.
     *
     //*@param None
     *
     //*@return None
     *
     */
    //void eventLoop();

    /**
     *Runs the event loop with a testing window that shows joystick positions.
     *Initializes SDL subsystems and closes them once the loop is terminated.
     *
     //*@param None
     *
     //*@return None
     *
     */
    //void testingEventLoop();
};

#endif