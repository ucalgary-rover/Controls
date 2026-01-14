// #include "Controller.h"
#include "Controller.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

static const char* file = "Controller";
static GameControllerAxis
covert_stl_axis_to_game_controller(SDL_GameControllerAxis axis);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Stick definitions------------------------------------------------------

Stick::Stick(GameControllerAxis xID, GameControllerAxis yID) {
    m_xAxis = xID;
    m_yAxis = yID;
}

float Stick::stickUpdate(Sint16 axisValue, int axisID) {

    // lists for pointers to position variables
    int* posList[2] = { &m_posX, &m_posY };
    float* rawPosList[2] = { &m_rawX, &m_rawY };

    // converts from Sint16 to float (float conversion may be superfluous)
    float Pos = axisValue;

    // based on ID, sets other values
    float* rawPos = rawPosList[axisID];

    // kind of gross way to switch to the other index value
    // otherPos is used to normalize Pos to circular coordinates
    float otherPos = *rawPosList[(int)!((bool)axisID)] / INT16_MAX;

    // If near the maximum value, adjusts the value to the maximum
    if (abs(Pos) > (INT16_MAX - STATIC_DEADZONE)) {

        // quick and dirty way to adjust the sign of the value
        Pos = (INT16_MAX + 1) * ((Pos < 0) ? -1 : 1);
        // std::cout << "\033[31m" << "\nX Max!\n"
        //           << "\033[0m"; //==========================================

    }
    // if close to zero, adjusts to zero
    else if (abs(Pos) < STATIC_DEADZONE) {

        Pos = 0;
        // std::cout << "\033[31m" << "\nX Zero\n"
        //           << "\033[0m"; //==========================================
    }
    // Checks if it is outside of the moving deadzone, if so, changes value
    else if (abs(Pos - *rawPos) > MOVING_DEADZONE) {
        Pos = Pos;
        // std::cout << "\033[31m" << "\nX Deadzone\n"
        //           << "\033[0m"; //==========================================

        // if none apply, keep the value the same
    } else {

        Pos = *rawPos;
    }

    // Saving new raw value
    // this is important for the moving deadzone
    *rawPos = Pos;

    // adjusting to circular values -255 - 255
    Pos /= INT16_MAX;

    // setting new value
    *posList[axisID] = Pos * SDL_sqrt((1 - (SDL_pow(otherPos, 2) / 2))) * 255;
    *posList[(int)!((bool)axisID)]
        = otherPos * SDL_sqrt((1 - (SDL_pow(Pos, 2) / 2))) * 255;

    // sending new values
    m_stickFunc(m_posX, m_posY);

    return *posList[axisID];
}

// Trigger definitions------------------------------------------------------

Trigger::Trigger(GameControllerAxis axisID) { m_axis = axisID; }

void Trigger::triggerUpdate(Sint16 axisValue) {

    // converts from Sint16 to float (float conversion may be superfluous)
    float Pos = axisValue;

    // Saving new raw value
    // this is important for the moving deadzone
    m_raw = axisValue;

    // adjusting to circular values 0 - 255
    Pos /= INT16_MAX;

    // setting new value
    m_pos = Pos * 255;

    // sending new values
    m_triggerFunc(m_pos);
}

// Controller definitions---------------------------------------------------

Controller::Controller() {
    m_pointerID = nullptr;

    // invalid joystick ID
    m_instanceID = -1;
}

Controller::Controller(SDL_GameController* identifier) {
    m_pointerID = identifier;

    // joystick / controller instance ID <- joystick pointer <-
    // controller pointer
    m_instanceID
        = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(identifier));

    m_rightStick
        = Stick(GAME_CONTROLLER_AXIS_RIGHTX, GAME_CONTROLLER_AXIS_RIGHTY);
    m_leftStick = Stick(GAME_CONTROLLER_AXIS_LEFTX, GAME_CONTROLLER_AXIS_LEFTY);
    m_leftTrigger = Trigger(GAME_CONTROLLER_AXIS_TRIGGERLEFT);
    m_rightTrigger = Trigger(GAME_CONTROLLER_AXIS_TRIGGERRIGHT);
}
