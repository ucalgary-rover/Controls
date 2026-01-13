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
    std::function<void(int, int)> m_stickFunc;

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
    void setStickFunc(std::function<void(int, int)> newStickFunc) {
        m_stickFunc = newStickFunc;
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

// class for triggers (generic and can be applied to any controller)
class Trigger {
private:
    // identifier for the axis
    GameControllerAxis m_axis;

    // position value from 0 - 255
    int m_pos = 0;

    // raw value for the use of moving deadzone comparison
    float m_raw = 0;

    // a function for sending this trigger's data
    std::function<void(int)> m_triggerFunc;

public:
    /**
     *Assigns a trigger's axis (ex. left trigger has a left axis)
     *@param The ID for the axis of the controller
     *@return None
     */
    Trigger(GameControllerAxis axisID);

    // default constructor
    Trigger() { }

    // getters
    float getPos() { return m_pos; };
    GameControllerAxis getAxis() { return m_axis; };

    // setters
    void setTriggerFunc(std::function<void(int)> newTriggerFunc) {
        m_triggerFunc = newTriggerFunc;
    };

    /**
     *Takes raw trigger data and processes it into an output in line with a
     *circular joystick. Deadzones also applied. Acts like a setter
     *
     *@param axisValue The value of the axis moved
     */
    void triggerUpdate(Sint16 axisValue);
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

    Trigger m_leftTrigger;
    Trigger m_rightTrigger;

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
    Trigger getLeftTrigger() { return m_leftTrigger; };
    Trigger getRightTrigger() { return m_rightTrigger; };

    void setLeftStick(Stick newStick) { m_leftStick = newStick; };
    void setRightStick(Stick newStick) { m_rightStick = newStick; };
    void setLeftTrigger(Trigger newTrigger) { m_leftTrigger = newTrigger; };
    void setRightTrigger(Trigger newTrigger) { m_rightTrigger = newTrigger; };

    buttonFunctions getButtonFuncs() { return m_buttonFuncs; };

    void setButtonFuncs(buttonFunctions funcsStruct) {
        m_buttonFuncs = funcsStruct;

        // Assign appropriate Stick
        m_leftStick.setStickFunc(funcsStruct.LEFT_JOYSTICK);
        m_rightStick.setStickFunc(funcsStruct.RIGHT_JOYSTICK);

        // Assign appropriate Trigger
        m_leftTrigger.setTriggerFunc(funcsStruct.LEFT_TRIGGER);
        m_rightTrigger.setTriggerFunc(funcsStruct.RIGHT_TRIGGER);
    };
};

#endif