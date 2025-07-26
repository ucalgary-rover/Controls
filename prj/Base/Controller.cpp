// #include "Controller.h"
#include "Controller.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

static const char* file = "Controller";
static GameControllerAxis
covert_stl_axis_to_game_controller(SDL_GameControllerAxis axis);

// Code for the testing window for adjusting controller deadzones
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window* makeWindow() {
    // Window stuffs
    SDL_Window* window = SDL_CreateWindow(
        "Joystick testing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError()
                  << std::endl;
    }

    return window;
}

void renderCross1(SDL_Surface* screenSurface, int x, int y, int w,
                  Uint32 color) {
    SDL_Rect rect = { x, y - w / 2, 1, w };
    SDL_FillRect(screenSurface, &rect, color);

    rect = { x - w / 2, y, w, 1 };
    SDL_FillRect(screenSurface, &rect, color);
}

void renderCross(SDL_Surface* screenSurface, int x, int y, float x1, float x2) {
    renderCross1(screenSurface, x, y, 20,
                 SDL_MapRGB(screenSurface->format, 0x99, 0x99, 0x99));
    renderCross1(screenSurface, x + x1 * SCREEN_WIDTH * 0.2f,
                 y + x2 * SCREEN_HEIGHT * 0.2f, 10,
                 SDL_MapRGB(screenSurface->format, 0xFF, 0xF, 0xAA));
}
//------------------------------------------------------------------

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

// ControllerHolder definitions -----------------------------------------

ControllerHolder::ControllerHolder(buttonFunctions functionStruct1,
                                   buttonFunctions functionStruct2) {

    // storing the structs in a list
    m_buttonFuncList[0] = functionStruct1;
    m_buttonFuncList[1] = functionStruct2;
}

void ControllerHolder::setControllerButtonFuncs(
    int index, buttonFunctions functionStruct) {

    // setting the button functions for each controller
    m_controllerList[index].setButtonFuncs(functionStruct);
}

// Controllerholder eventloop and response
// functions--------------------------------------

void ControllerHolder::buttonResponse(Uint8 buttonID, int controllerIndex) {

    std::string buttonName = getButtonName(buttonID);
    Logging::logV(file, "Button press %d %d", controllerIndex, buttonID);

    for (int i = 0; i < 2; i++) {
        if ((int)m_controllerList[i].getInstanceID() == controllerIndex) {
            controllerIndex = i;
            break;
        }
    }

    // searches thru the dictionary to execute the appropriate function
    m_controllerList[controllerIndex].getButtonFuncs().buttonArray[buttonID]();
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
        (*activeController).setLeftStick(leftStick);

        // otherwise it is right stick
    } else {
        // updating temp stick
        rightStick.stickUpdate(axisValue, stickAxisID);

        // saving updated stick
        (*activeController).setRightStick(rightStick);
    }
}

void ControllerHolder::triggerResponse(Sint16 axisValue, int axisID,
                                       int controllerIndex) {
    // Identifying which trigger is in play
    for (int i = 0; i < 2; i++) {
        if ((int)m_controllerList[i].getInstanceID() == controllerIndex) {
            controllerIndex = i;
            break;
        }
    }
    // setting which controller to update for this event
    Controller* activeController = &m_controllerList[controllerIndex];

    // Making temp triggers
    Trigger leftTrigger = (*activeController).getLeftTrigger();
    Trigger rightTrigger = (*activeController).getRightTrigger();

    // identifies if left trigger
    if (axisID == (int)leftTrigger.getAxis()) {
        // updating temp trigger
        leftTrigger.triggerUpdate(axisValue);

        // saving updated trigger
        (*activeController).setLeftTrigger(leftTrigger);

        // otherwise it is right trigger
    } else if (axisID == (int)rightTrigger.getAxis()) {
        // updating temp trigger
        rightTrigger.triggerUpdate(axisValue);

        // saving updated trigger
        (*activeController).setRightTrigger(rightTrigger);
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

            Logging::logV(file, "Controller added", i);
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
        Logging::logV(file, "No controllers connected! Number of joysticks: %d",
                      SDL_NumJoysticks());
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
                {
                    GameControllerAxis axis
                        = covert_stl_axis_to_game_controller(
                            (SDL_GameControllerAxis)event.jaxis.axis);
                    Logging::logV(file,
                                  "Axis motion. Axis was: %d Axis is now: %d",
                                  (int)event.jaxis.axis, axis);

                    if (axis <= GAME_CONTROLLER_AXIS_LAST_JOYSTICK) {
                        stickResponse(event.jaxis.value, axis,
                                      event.cdevice.which);
                    } else if (axis < GAME_CONTROLLER_AXIS_MAX) {
                        triggerResponse(event.jaxis.value, axis,
                                        event.cdevice.which);
                    }

                    break;
                }

            case SDL_CONTROLLERBUTTONDOWN:

                buttonResponse(event.cbutton.button, event.cdevice.which);

                //-------------------------------------------------------------
                // Hardcoding a button that quits the loop
                // if (SDL_CONTROLLER_BUTTON_START == event.cbutton.button) {
                //     quit = true;
                // }

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

void ControllerHolder::testingEventLoop() {

    bool quit = false;

    SDL_Window* window = makeWindow();

    // initializing SDL sub systems`
    // This combo works, it is a little strange tho
    // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        SDL_Quit();
    }

    // Check for game controllers
    if (SDL_NumJoysticks() < 1) {
        Logging::logV(file, "No controllers connected! Number of joysticks: %d",
                      SDL_NumJoysticks());
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
            case SDL_JOYAXISMOTION: {

                // for now, making sure triggers don't cause problems

                GameControllerAxis axis = covert_stl_axis_to_game_controller(
                    (SDL_GameControllerAxis)event.jaxis.axis);
                Logging::logV(file, "Axis motion", (int)event.jaxis.axis);
                Logging::logV(file, "Axis motion", (int)axis);

                if (axis < 4) {
                    stickResponse(event.jaxis.value, axis, event.cdevice.which);
                }

                break;
            }

            case SDL_CONTROLLERBUTTONDOWN:

                buttonResponse(event.cbutton.button, event.cdevice.which);

                //-------------------------------------------------------------
                // Hardcoding a button that quits the loop
                if (SDL_CONTROLLER_BUTTON_START == event.cbutton.button) {
                    std::cout << "Quitting";
                    quit = true;
                }

                break;
            }
        }

        // Testing code!!!
        //  --------------------------------------------------------------------------
        //  rendering the crosses

        SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
        SDL_FillRect(screenSurface, nullptr,
                     SDL_MapRGB(screenSurface->format, 0x33, 0x33, 0x33));

        renderCross(screenSurface, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2,
                    m_controllerList[0].getLeftStick().getPosX() / 255,
                    m_controllerList[0].getLeftStick().getPosY() / 255);
        renderCross(screenSurface, 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2,
                    m_controllerList[0].getRightStick().getPosX() / 255,
                    m_controllerList[0].getRightStick().getPosY() / 255);

        // must be used to update window
        SDL_UpdateWindowSurface(window);
    }

    // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<
    for (Controller controller : m_controllerList) {
        if (controller.getPointerID()) {
            SDL_GameControllerClose(controller.getPointerID());
        }
    }

    quit = false;

    SDL_Quit();
}

static GameControllerAxis
covert_stl_axis_to_game_controller(SDL_GameControllerAxis axis) {
    switch (axis) {
    case SDL_CONTROLLER_AXIS_LEFTX:
        return GAME_CONTROLLER_AXIS_LEFTX;
    case SDL_CONTROLLER_AXIS_LEFTY:
        return GAME_CONTROLLER_AXIS_LEFTY;
    case SDL_CONTROLLER_AXIS_RIGHTX:
        return GAME_CONTROLLER_AXIS_TRIGGERLEFT;
    case SDL_CONTROLLER_AXIS_RIGHTY:
        return GAME_CONTROLLER_AXIS_RIGHTX;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        return GAME_CONTROLLER_AXIS_RIGHTY;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        return GAME_CONTROLLER_AXIS_TRIGGERRIGHT;
    default:
        return GAME_CONTROLLER_AXIS_INVALID;
    }
}