#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include <SDL2/SDL.h>
#include <iostream>

typedef void (*ControllerFunc)(void *arg);
// a struct for function pointers used for button mapping
struct buttonFunctions {
  ControllerFunc Button;
  void (*BUTTON_A)();
  void (*BUTTON_B)();
  void (*BUTTON_X)();
  void (*BUTTON_Y)();
  void (*BUTTON_BACK)();
  void (*BUTTON_GUIDE)();
  void (*BUTTON_START)();
  void (*BUTTON_LEFTSTICK)();
  void (*BUTTON_RIGHTSTICK)();
  void (*BUTTON_LEFTSHOULDER)();
  void (*BUTTON_RIGHTSHOULDER)();
  void (*BUTTON_DPAD_UP)();
  void (*BUTTON_DPAD_DOWN)();
  void (*BUTTON_DPAD_LEFT)();
  void (*BUTTON_DPAD_RIGHT)();
};

// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHolder {

  // class for joysticks (they are generic and can be applied to any controller)
  class Stick {
  public:
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
     *Assigns a joystick's axes (ex. right stick has a rightX and a rightY axis)
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

  public:
    // the identifier (a pointer to the controller object and its instance ID)
    SDL_GameController *m_pointerID;
    SDL_JoystickID m_instanceID;

    // the values used to initialize sticks just identify a generic left or
    // right joystick
    Stick m_leftStick;
    Stick m_rightStick;

    // getter funcs
    SDL_GameController *getPointerID() { return m_pointerID; };
    SDL_JoystickID getInstanceID() { return m_instanceID; };

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
    Controller(SDL_GameController *identifier) {
      m_pointerID = identifier;

      // joystick / controller instance ID <- joystick pointer <- controller
      // pointer
      m_instanceID;
    }
  };

public:
  // Holding the controller objects
  Controller m_controllerList[2];

  // Holding the structs to map to each controller
  buttonFunctions m_buttonList[2];

  /**
   *Initializes controllers and mapps the appropriate buttons to them
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

#endif
