#include "ControllerHandler.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

bool ControllerHandler::is_initialized = false;
std::vector<Controller> ControllerHandler::controllers = {};
std::map<int, int> ControllerHandler::controllerAssignment = {};

static const char* file = "ControllerHandler";

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

// function for remapping axes if necessary
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

// ControllerHandler definitions -----------------------------------------
bool ControllerHandler::initialize(
    DriveStateManager* driveStateManager, ArmStateManager* armStateManager,
    ArmMotorStateManager* armManualChangeManager) {
    DriveControllerLayout driveLayout
        = DriveControllerLayout(driveStateManager);
    ArmControllerLayout armLayout
        = ArmControllerLayout(armStateManager, armManualChangeManager);

    Controller controller1 = Controller(&driveLayout);
    Controller controller2 = Controller(&armLayout);

    controllers = { controller1, controller2 };
    for (size_t i = 0; i < controllers.size(); ++i) {
        controllerAssignment[-i - 1] = i;
    }

    is_initialized = true;
    return true;
}

// Controllerholder eventloop and response
// functions--------------------------------------

void ControllerHandler::buttonResponse(uint8_t buttonID, int controllerID) {
    controllers[controllerAssignment[controllerID]].buttonUpdate(buttonID);
}

void ControllerHandler::stickResponse(int16_t axisValue, int axisID,
                                      int controllerID) {

    // convert axisID to 0 or 1 (0 = x-Axis, 1 = y-Axis)
    int stickAxisID = axisID % 2;

    if (axisID > GAME_CONTROLLER_AXIS_LEFTY) {
        controllers[controllerAssignment[controllerID]].rightStickUpdate(
            axisValue, stickAxisID);
    } else {
        controllers[controllerAssignment[controllerID]].leftStickUpdate(
            axisValue, stickAxisID);
    }
}

void ControllerHandler::triggerResponse(int16_t axisValue, int axisID,
                                        int controllerID) {

    if (axisID > GAME_CONTROLLER_AXIS_TRIGGERRIGHT) {
        controllers[controllerAssignment[controllerID]].rightTriggerUpdate(
            axisValue);
    } else {
        controllers[controllerAssignment[controllerID]].leftTriggerUpdate(
            axisValue);
    }
}

void ControllerHandler::controllerAddedResponse(int controllerID) {
    // checks for which controller object is not active (key < 0), deletes it,
    // and connects the new controller
    int mapIndex;
    int vectorIndex;
    for (auto& [key, value] : controllerAssignment) {
        if (key < 0) {
            mapIndex = key;
            vectorIndex = value;
            break;
        }
    }

    SDL_GameControllerOpen(controllerID);
    controllerAssignment.erase(mapIndex);
    controllerAssignment[controllerID] = vectorIndex;
    Logging::logV(file, "Controller added", vectorIndex);
}

void ControllerHandler::controllerRemovedResponse(int controllerID) {
    // matches the current controller
    int mapIndex;
    int vectorIndex;
    for (auto& [key, value] : controllerAssignment) {
        if (key == controllerID) {
            mapIndex = key;
            vectorIndex = value;

            Logging::logV(file, "Controller removed", value);
            break;
        }
    }

    // closes the controller in question
    SDL_GameControllerClose(SDL_GameControllerFromInstanceID(controllerID));

    // sets the key to some invalid value
    controllerAssignment.erase(mapIndex);
    controllerAssignment[-1] = vectorIndex;

    // sends an update that sets all values to zero to prevent any ghost
    // movement
    for (int j = 0; j < 4; j++) {
        stickResponse(0, j, mapIndex);
    };
}

void ControllerHandler::eventLoop() {

    bool quit = false;

    int homePressedStartTime;
    int timeElapsed;

    // initializing SDL sub systems`
    // This combo works, it is a little strange tho
    // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

    int sdl_init_status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    std::cout << "SDL_Init status: " << sdl_init_status << std::endl;
    if (sdl_init_status < 0) {
        Logging::logE(file, "could not initialize sdl2: %s\n", SDL_GetError());
        SDL_Quit();
        quit = true;
    }

    // Check for game controllers
    int sdl_joysticks = SDL_NumJoysticks();

    std::cout << "Connected Joysticks: " << sdl_joysticks << std::endl;
    if (sdl_joysticks < 1) {
        Logging::logI(file, "No controllers connected! Number of joysticks: %d",
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

            Logging::logV(file, "SDL Event type: %d", event.type);

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

                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
                    homePressedStartTime = SDL_GetTicks();

                buttonResponse(event.cbutton.button, event.cdevice.which);

                //-------------------------------------------------------------
                // Hardcoding a button that quits the loop
                // if (SDL_CONTROLLER_BUTTON_START == event.cbutton.button) {
                //     quit = true;
                // }

                break;

            case SDL_CONTROLLERBUTTONUP:
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
                    timeElapsed = SDL_GetTicks() - homePressedStartTime;

                if (timeElapsed >= 3000)
                    // switching logic here
                    for (auto& [key, value] : controllerAssignment) {
                        switch (value) {
                        case 1:
                            value = 2;
                            break;

                        case 2:
                            value = 1;
                            break;
                        }
                    }

                break;
            }
        }
    }

    // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<
    for (auto& [key, value] : controllerAssignment) {
        if (key >= 0) {
            SDL_GameControllerClose(SDL_GameControllerFromInstanceID(key));
        }
    }

    SDL_Quit();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++