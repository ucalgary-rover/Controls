#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include "pub_general.h"
#include <SDL2/SDL.h>
#include <iostream>


// class for joysticks (they are generic and can be applied to any controller)
class Stick {
    private:
      // identifiers for the axes
      // funny enough, triggers also operate with axes
      SDL_GameControllerAxis X_AXIS;
      SDL_GameControllerAxis Y_AXIS;
    
      // positions values from -255 - 255 (note up and left are negative)
      int m_posX = 0;
      int m_posY = 0;
    
      // raw values for the use of moving deadzone comparison
      float m_rawX = 0;
      float m_rawY = 0;
    
    public:
      /**
       *Assigns a joystick's axes (ex. right stick has a rightX and a rightY axis)
       *
       *@param values The pointer to the game controller object
       *
       *@return None
       *
       */
      Stick(SDL_GameControllerAxis xID, SDL_GameControllerAxis yID);
    
      // default constructor
      Stick() {}
    
      // getters
      float getPosX() { return m_posX; };
      float getPosY() { return m_posY; };
    
      /**
       *Takes raw stick data and processes it into an output in line with a
       *circular joystick. Deadzones also applied. Acts like a setter
       *
       *@param axisValue The value of the axis moved
       *@param axisID An ID differentiating the x and y axes on one stick (0 and 1
       *respectively)
       *
       *
       *@return The adjusted float value of the changed axis
       *
       */
      float stickUpdate(Sint16 axisValue, int axisID);
    };
    
    // class for controllers
    class Controller {
    
    private:
      // the identifier (a pointer to the controller object and its instance ID)
      SDL_GameController *m_pointerID;
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
      Controller(SDL_GameController *identifier);
    
      // getter/setter funcs
    
      // these are never changed unless there is a new controller anyways so no
      // setter needed
      SDL_GameController *getPointerID() { return m_pointerID; };
      SDL_JoystickID getInstanceID() { return m_instanceID; };
    
      // sticks get changed via stickResponse and stickUpdate
      Stick getLeftStick() { return m_leftStick; };
      Stick getRightStick() { return m_rightStick; };
    
      void setButtonFuncs(buttonFunctions funcsStruct) {
        m_buttonFuncs = funcsStruct;
      };
    
      /**
       *Executes a function when a button is pressed
       *
       *@param buttonID The ID of the button pressed
       *
       *@return None
       *
       */
      void buttonResponse(Uint8 buttonID);
    
      /**
       *Identifies which stick requires an update and pass in variable for the
       *appropriate stick update function
       *
       *@param axisValue The value of the axis moved
       *@param axisID An ID differentiating the x and y axes across both sticks
       *
       *
       *@return None. Updates values directly through stick functions
       *
       */
      void stickResponse(Sint16 axisValue, int axisID);
    };
    
    // A class for initializing controllers and running the pollEvent loop
    // Holds a list of structs that are linked to the positions of the controllers
    // in the list <<<<<<<<<<<<<<<<<<<<<<<
    class ControllerHolder {
    
    private:
      // Holding the controller objects
      Controller m_controllerList[2];
    
      //Holding the structs for each controller
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
    
      // getter and setter function
      Controller getControllerFromList(int i) {
        return (i == 1 || i == 0) ? m_controllerList[i] : m_controllerList[0];
      };
    
      void setControllerFromList(int i, Controller controller) {
        i = (i == 1 || i == 0) ? i : 0;
        m_controllerList[i] = controller;
      };
    
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
    #endif