#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "pub_general.h"
#include <SDL2/SDL.h>
#include <iostream>

// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHolder {

    // class for joysticks (they are generic and can be applied to any
    // controller)
    class Stick {
    private:
        // identifiers for the axes
        // funny enough, triggers also operate with axes
        SDL_GameControllerAxis X_AXIS;
        SDL_GameControllerAxis Y_AXIS;

        // positions values from -32768 - 32767 (note up and left are negative)
        float m_xPos;
        float m_yPos;

        // raw values for the use of moving deadzone comparison
        float m_rawX;
        float m_rawY;

    public:
        /**
         *Assigns a joystick's axes (ex. right stick has a rightX and a rightY
         *axis)
         *
         *@param values The pointer to the game controller object
         *
         *@return None
         *
         */
        Stick(SDL_GameControllerAxis xID, SDL_GameControllerAxis yID) {
            X_AXIS = xID;
            Y_AXIS = yID;
        }
    };

    // class for controllers
    class Controller {

    private:
        // the identifier (a pointer to the controller object and its instance
        // ID)
        SDL_GameController* m_pointerID;
        SDL_JoystickID m_instanceID;

        // the values used to initialize sticks just identify a generic left or
        // right joystick
        Stick m_leftStick;
        Stick m_rightStick;

        // struct for button functions
        buttonFunctions m_buttonFuncs;

    public:
        // getter/setter funcs

        // these are never changed unless there is a new controller anyways so
        // no setter needed
        SDL_GameController* getPointerID() { return m_pointerID; };
        SDL_JoystickID getInstanceID() { return m_instanceID; };

        Stick getLeftStick() { return m_leftStick; };
        Stick getRightStick() { return m_rightStick; };
        void setLeftStick(Stick lStick) { m_leftStick = lStick; };
        void setRightStick(Stick rStick) { m_rightStick = rStick; };

        void setButtonFuncs(buttonFunctions funcsStruct) {
            m_buttonFuncs = funcsStruct;
        };

        // A constructor for making a completely blank controller object
        Controller() {
            m_pointerID;

            // invalid joystick ID
            m_instanceID;
        }

        /**
         *Assigns the controller's identifier once it is initialized
         *
         *@param values The pointer to the game controller object
         *
         *@return None
         *
         */
        Controller(SDL_GameController* identifier) {
            m_pointerID = identifier;

            // joystick / controller instance ID <- joystick pointer <-
            // controller pointer
            m_instanceID;
        }

        /**
         *Executes a function when a button is pressed
         *
         *@param buttonID The ID of the button pressed
         *
         *@return None
         *
         */
        void buttonResponse(Uint8 buttonID);
    };

private:
    // Holding the controller objects
    Controller m_controllerList[2];

public:
    // getter and setter function
    Controller getControllerFromList(int i) {
        return (i == 1 || i == 0) ? m_controllerList[i] : m_controllerList[0];
    };

    void setControllerFromList(int i, Controller controller) {
        i = (i == 1 || i == 0) ? i : 0;
        m_controllerList[i] = controller;
    };
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

    /**
     *Runs the event loop to recieve any controller events
     *
     *@param values None
     *
     *@return None
     *
     */
    void eventLoop();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
