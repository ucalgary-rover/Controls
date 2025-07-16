#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include "pub_general.h"
#include <SDL2/SDL.h>
#include <iostream>

// Used to streamline the struct (Don't keep in final)<<<<<<<<<<<<<<<<<<<<<<<<<
typedef void (*ControllerFunc)(void* args);

// class for joysticks (generic and can be applied to any controller)
class Stick {
private:
    // identifiers for the axes
    // funny enough, triggers also operate with axes
    GameControllerAxis m_xAxis;
    GameControllerAxis m_yAxis;

    // positions values from -255 - 255 (note up and left are negative)
    int m_posX = 0;
    int m_posY = 0;

    // raw values for the use of moving deadzone comparison
    float m_rawX = 0;
    float m_rawY = 0;

    // a function for sending this stick's data
    void (*m_stickFunc)(int xValue, int yValue);

    // Holds settings for the deadzone sizes
    //  any number less than ~30000
    //  5000 is a moderate rate
    int m_movingDeadzone = 5000;
    int m_staticDeadzone = 10000;

public:
    /**
     *Assigns a joystick's axes (ex. right stick has a rightX and a rightY axis)
     *@param The IDs for each axis of the controller
     *@return None
     */
    Stick(GameControllerAxis xID, GameControllerAxis yID);

    // default constructor
    Stick() { }

    // getters
    float getPosX() { return m_posX; };
    float getPosY() { return m_posY; };
    GameControllerAxis getXAxis() { return m_xAxis; };
    GameControllerAxis getYAxis() { return m_yAxis; };

    // setters
    void setStickFunc(void (*newStickFunc)(int xValue, int yValue)) {
        m_stickFunc = newStickFunc;
    };

    void setStickSetup(int movingDeadzone, int staticDeadzone) {
        m_movingDeadzone = movingDeadzone;
        m_staticDeadzone = staticDeadzone;
    };

    /**
     *Takes raw stick data and processes it into an output in line with a
     *circular joystick. Deadzones also applied. Acts like a setter
     *
     *@param axisValue The value of the axis moved
     *@param axisID An ID differentiating the x and y axes on one stick (0 and 1
     *respectively)
     *@return The updated stick
     *
     */
    float stickUpdate(Sint16 axisValue, int axisID);
};

// class for controllers
class Controller {

private:
    // the identifier (a pointer to the controller object and its instance ID)
    SDL_GameController* m_pointerID;
    SDL_JoystickID m_instanceID;

    // the values used to initialize sticks just identify a generic left or
    // right joystick
    Stick m_leftStick;
    Stick m_rightStick;

    // struct for button functions
    buttonFunctions m_buttonFuncs;

public:
    // A constructor for making a completely blank controller object
    Controller();

    /**
     *Assigns the controller's identifier once it is initialized. Also creates
     *stick objects for the controller
     *
     *@param identifier The pointer to the game controller object
     *
     *@return None
     *
     */
    Controller(SDL_GameController* identifier);

    // getter/setter funcs

    // these are never changed unless there is a new controller anyways so no
    // setter needed
    SDL_GameController* getPointerID() { return m_pointerID; };
    SDL_JoystickID getInstanceID() { return m_instanceID; };

    // sticks get changed via stickResponse and stickUpdate must be saved with
    // stik setters
    Stick getLeftStick() { return m_leftStick; };
    Stick getRightStick() { return m_rightStick; };

    void setLeftStick(Stick newStick) { m_leftStick = newStick; };
    void setRightStick(Stick newStick) { m_rightStick = newStick; };

    buttonFunctions getButtonFuncs() { return m_buttonFuncs; };

    void setButtonFuncs(buttonFunctions funcsStruct) {
        m_buttonFuncs = funcsStruct;

        // also assigns appropriate
        m_leftStick.setStickFunc(funcsStruct.LEFT_JOYSTICK);
        m_rightStick.setStickFunc(funcsStruct.RIGHT_JOYSTICK);
    };
};

// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHolder {

private:
    // Holding the controller objects
    Controller m_controllerList[2];

    // Holding the structs for each controller
    buttonFunctions m_buttonFuncList[2];

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
    ControllerHolder(buttonFunctions functionStruct1,
                     buttonFunctions functionStruct2);
    void setControllerButtonFuncs(int index, buttonFunctions functionStruct);

    // getter and setter function
    Controller getControllerFromList(int i) {
        return (i == 1 || i == 0) ? m_controllerList[i] : m_controllerList[0];
    };

    void setControllerFromList(int i, Controller controller) {
        i = (i == 1 || i == 0) ? i : 0;
        m_controllerList[i] = controller;
    };

    /**
     *Executes a function when a button is pressed
     *
     *@param buttonID The ID of the button pressed
     *@param controllerIndex The index of the active controller for the event
     *
     *@return None
     *
     */
    void buttonResponse(Uint8 buttonID, int controllerIndex);

    /**
     *Identifies which stick requires an update and pass in variable for the
     *appropriate stick update function
     *
     *@param axisValue The value of the axis moved
     *@param axisID An ID differentiating the x and y axes across both sticks
     *@param controllerIndex The index of the active controller for the event
     *
     *
     *@return None. Updates values directly through stick functions
     *
     */
    void stickResponse(Sint16 axisValue, int axisID, int controllerIndex);

    /**
     *Executes a function when a button is pressed
     *
     *@param controllerIndex The index of the active controller for the event
     *
     *@return None
     *
     */
    void controllerAddedResponse(int controllerIndex);

    /**
     *Closes the removed controller and sends an update with all zero values
     *
     *@param controllerIndex The index of the active controller for the event
     *
     *@return None
     *
     */
    void controllerRemovedResponse(int controllerIndex);

    /**
     *Runs the event loop to recieve any controller events. Initializes SDL
     *subsystems and closes them once the loop is terminated.
     *
     *@param None
     *
     *@return None
     *
     */
    void eventLoop();

    /**
     *Runs the event loop with a testing window that shows joystick positions.
     *Initializes SDL subsystems and closes them once the loop is terminated.
     *
     *@param None
     *
     *@return None
     *
     */
    void testingEventLoop();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif