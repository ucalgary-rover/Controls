#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include "ControllerLayout.h"
#include "pub_general.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

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

    /**
     *Takes raw stick data and processes it into an output in line with a
     *circular joystick. Deadzones also applied. Acts like a setter
     *
     *@param axisValue The value of the axis moved
     *@param axisID An ID differentiating the x and y axes on one stick (0 and 1
     *respectively)
     *@return none
     *
     */
    void stickUpdate(Sint16 axisValue, int axisID);
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

    /**
     *Takes raw trigger data and processes it into an output in line with
     *the range of a joystick. Deadzones also applied. Acts like a setter
     *
     *@param axisValue The value of the axis moved
     */
    void triggerUpdate(Sint16 axisValue);
};

// class for controllers
class Controller {

private:
    // the values used to initialize sticks just identify a generic left or
    // right joystick
    Stick m_leftStick;
    Stick m_rightStick;

    Trigger m_leftTrigger;
    Trigger m_rightTrigger;

    // struct for button functions
    buttonFunctions m_buttonFuncs;

    std::unique_ptr<ControllerLayout> controllerLayout;

public:
    // A constructor for making a completely blank controller object
    Controller();

    /**
     *Assigns the controller's layout once it is initialized. Also creates
     *stick and trigger objects for the controller
     *
     *@param layout The layout which values are sent to
     *
     *@return None
     *
     */
    Controller(std::unique_ptr<ControllerLayout> layout);

    void leftTriggerUpdate(int16_t axisValue);
    void rightTriggerUpdate(int16_t axisValue);
    void leftStickUpdate(int16_t axisValue, int axisID);
    void rightStickUpdate(int16_t axisValue, int axisID);
    void buttonUpdate(int buttonID);

    std::string getLayoutName();
};

#endif