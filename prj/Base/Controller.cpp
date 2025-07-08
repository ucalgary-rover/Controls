// #include "Controller.h"
#include "Controller.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// Helper function to get button names
std::string getButtonName(Uint8 button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A:
        return "Cross (A)";
    case SDL_CONTROLLER_BUTTON_B:
        return "Circle (B)";
    case SDL_CONTROLLER_BUTTON_X:
        return "Square (X)";
    case SDL_CONTROLLER_BUTTON_Y:
        return "Triangle (Y)";
    case SDL_CONTROLLER_BUTTON_BACK:
        return "Share";
    case SDL_CONTROLLER_BUTTON_GUIDE:
        return "PS Button";
    case SDL_CONTROLLER_BUTTON_START:
        return "Options";
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return "Left Stick Button";
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return "Right Stick Button";
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return "L1";
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return "R1";
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return "D-Pad Up";
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return "D-Pad Down";
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return "D-Pad Left";
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return "D-Pad Right";

    // yet to trigger for some reason
    default:
        return "Unknown Button";
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Stick definitions------------------------------------------------------

Stick::Stick(SDL_GameControllerAxis xID, SDL_GameControllerAxis yID) {
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
    if (abs(Pos) > (INT16_MAX - m_staticDeadzone)) {

        // quick and dirty way to adjust the sign of the value
        Pos = (INT16_MAX + 1) * ((Pos < 0) ? -1 : 1);
        // std::cout << "\033[31m" << "\nX Max!\n"
        //           << "\033[0m"; //==========================================

    }
    // if close to zero, adjusts to zero
    else if (abs(Pos) < m_staticDeadzone) {

        Pos = 0;
        // std::cout << "\033[31m" << "\nX Zero\n"
        //           << "\033[0m"; //==========================================
    }
    // Checks if it is outside of the moving deadzone, if so, changes value
    else if (abs(Pos - *rawPos) > m_movingDeadzone) {
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
        = Stick(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);
    m_leftStick = Stick(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
}

// ControllerHolder definitions -----------------------------------------

ControllerHolder::ControllerHolder(buttonFunctions functionStruct1,
                                   buttonFunctions functionStruct2) {

    // storing the structs in a list
    m_buttonFuncList[0] = functionStruct1;
    m_buttonFuncList[1] = functionStruct2;
}

// Controllerholder eventloop and response
// functions--------------------------------------

void ControllerHolder::buttonResponse(Uint8 buttonID, int controllerIndex) {

    std::string buttonName = getButtonName(buttonID);
    std::cout << controllerIndex << std::endl;
    std::cout << "Button press\n";

    for (int i = 0; i < 2; i++) {
        if ((int)m_controllerList[i].getInstanceID() == controllerIndex) {
            controllerIndex = i;
            break;
        }
    }

    // searches thru the dictionary to execute the appropriate function
    m_controllerList[controllerIndex].getButtonFuncs().buttonArray[buttonID](
        &buttonName);
}

void ControllerHolder::stickResponse(Sint16 axisValue, int axisID,
                                     int controllerIndex) {
    // Identifying which stick is in play
    for (int i = 0; i < 2; i++) {
        if ((int)m_controllerList[i].getInstanceID() == controllerIndex) {
            controllerIndex = i;
            break;
        }
    }
    // setting which controller to update for this event
    Controller* activeController = &m_controllerList[controllerIndex];

    // Making temp sticks
    Stick leftStick = (*activeController).getLeftStick();
    Stick rightStick = (*activeController).getRightStick();

    // an axis ID to pass into the stick update function
    int stickAxisID = (axisID == (int)leftStick.getXAxis()
                       || axisID == (int)rightStick.getXAxis())
                          ? 0
                          : 1;

    // identifies if left stick

    if (axisID == (int)leftStick.getXAxis()
        || axisID == (int)leftStick.getYAxis()) {
        // updating temp stick
        leftStick.stickUpdate(axisValue, stickAxisID);

        // saving updated stick
        (*activeController).setLeftStick(rightStick);

        // otherwise it is right stick
    } else {
        // updating temp stick
        rightStick.stickUpdate(axisValue, stickAxisID);

        // saving updated stick
        (*activeController).setRightStick(rightStick);
    }
}

void ControllerHolder::controllerAddedResponse(int controllerIndex) {
    // checks for which controller object is not active and connects
    // the new controller there
    for (int i = 0; i < 3; i++) {

        // have to do this since nullptrs are special little
        // snowflakes they don't play nice with "!" I guess
        if (!((bool)m_controllerList[i].getPointerID())) {
            m_controllerList[i]
                = Controller(SDL_GameControllerOpen(controllerIndex));

            // sets buttonfuncs based on the position the connected
            // controller is assigned
            m_controllerList[i].setButtonFuncs(m_buttonFuncList[i]);

            std::cout << i << " added" << std::endl;
            break;
        }
    }
}

void ControllerHolder::controllerRemovedResponse(int controllerIndex) {
    // checks if there is a controller id and the controller id
    // matches the current controller
    for (int i = 0; i < 3; i++) {
        if (m_controllerList[i].getInstanceID() == controllerIndex) {

            // closes the controller in question
            SDL_GameControllerClose(m_controllerList[i].getPointerID());
            m_controllerList[i] = Controller();

            // sends an update that sets all values to zero to prevent any ghost
            // movement
            for (int j = 0; j < 4; j++) {
                stickResponse(0, j, i);
            };
            break;
        }
    }
}

void ControllerHolder::eventLoop() {

    bool quit = false;

    // initializing SDL sub systems`
    // This combo works, it is a little strange tho
    // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        SDL_Quit();
        quit = true;
    }

    // Check for game controllers
    if (SDL_NumJoysticks() < 1) {
        std::cout << "No controllers connected!" << std::endl
                  << "Number of joysticks: " << SDL_NumJoysticks();
    }

    SDL_Event event;

    // the main update loop
    // reads events and updates accordingly per loop
    while (!quit) {

        // pulls an event from the queue (crucial for updating literally
        // anything!) this is the event loop yet to figure out why event loop
        // has to look like this
        while (SDL_PollEvent(&event)) {

            // checks what kind of event
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_CONTROLLERDEVICEADDED:

                controllerAddedResponse(event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:

                controllerRemovedResponse(event.cdevice.which);
                break;

            // working now!
            // featuring dante from devil may cry! /satire
            case SDL_JOYAXISMOTION:
                // for now, making sure triggers don't cause problems
                if (event.jaxis.axis < 4) {
                    stickResponse(event.jaxis.value, event.jaxis.axis,
                                  event.cdevice.which);
                }

                break;

            case SDL_CONTROLLERBUTTONDOWN:

                buttonResponse(event.cbutton.button, event.cdevice.which);

                //-------------------------------------------------------------
                // Hardcoding a button that quits the loop
                if (SDL_CONTROLLER_BUTTON_START == event.cbutton.button) {
                    quit = true;
                }

                break;
            }
        }
    }

    // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<
    for (Controller controller : m_controllerList) {
        if (controller.getPointerID()) {
            SDL_GameControllerClose(controller.getPointerID());
        }
    }

    SDL_Quit();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
