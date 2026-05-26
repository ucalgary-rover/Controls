#include "ControllerHandler.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

bool ControllerHandler::is_initialized = false;
std::vector<Controller> ControllerHandler::controllers = {};
std::vector<std::shared_ptr<ControllerLayout>> ControllerHandler::layouts = {};

int ControllerHandler::controllerAssignment[2] = { -1, -1 };

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
// using const char because logging doesn't like std::string apparently
static const char* getButtonName(Uint8 button) {
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

static const char* eventTypeToString(Uint32 eventType) {
    switch (eventType) {
    case SDL_QUIT:
        return "SDL_QUIT";
    case SDL_APP_TERMINATING:
        return "SDL_APP_TERMINATING";
    case SDL_JOYAXISMOTION:
        return "SDL_JOYAXISMOTION";
    case SDL_JOYBALLMOTION:
        return "SDL_JOYBALLMOTION";
    case SDL_JOYHATMOTION:
        return "SDL_JOYHATMOTION";
    case SDL_JOYBUTTONDOWN:
        return "SDL_JOYBUTTONDOWN";
    case SDL_JOYBUTTONUP:
        return "SDL_JOYBUTTONUP";
    case SDL_JOYDEVICEADDED:
        return "SDL_JOYDEVICEADDED";
    case SDL_JOYDEVICEREMOVED:
        return "SDL_JOYDEVICEREMOVED";
    case SDL_CONTROLLERAXISMOTION:
        return "SDL_CONTROLLERAXISMOTION";
    case SDL_CONTROLLERBUTTONDOWN:
        return "SDL_CONTROLLERBUTTONDOWN";
    case SDL_CONTROLLERBUTTONUP:
        return "SDL_CONTROLLERBUTTONUP";
    case SDL_CONTROLLERDEVICEADDED:
        return "SDL_CONTROLLERDEVICEADDED";
    case SDL_CONTROLLERDEVICEREMOVED:
        return "SDL_CONTROLLERDEVICEREMOVED";
    case SDL_CONTROLLERDEVICEREMAPPED:
        return "SDL_CONTROLLERDEVICEREMAPPED";
    default:
        return "SDL_UNKNOWN_EVENT";
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

int ControllerHandler::vectorIndexFromID(int controllerID) {
    for (int i = 0; i < controllerAmount;
         i++) { // should never return an index too large for the vector
        if (controllerAssignment[i] == controllerID) {
            return i;
        }
    }
    return -1;
}

// ControllerHandler definitions -----------------------------------------
bool ControllerHandler::initialize(
    DriveStateManager* driveStateManager,
    DriveMotorStateManager* driveMotorStateManager,
    ArmStateManager* armStateManager,
    ArmMotorStateManager* armManualChangeManager) {

    layouts.push_back(std::make_shared<DriveControllerLayout>(
        driveStateManager, driveMotorStateManager));
    layouts.push_back(std::make_shared<ArmControllerLayout>(
        armStateManager, armManualChangeManager));

    for (const auto& layout : layouts) {
        controllers.push_back(Controller(layout));
    }

    for (int i = 0; i < controllerAmount; ++i) {
        controllerAssignment[i] = -1;
    }

    is_initialized = true;
    return true;
}

// Controllerholder eventloop and response
// functions--------------------------------------

void ControllerHandler::buttonResponse(uint8_t buttonID, int controllerID) {
    // check if ID is valid:
    int index = vectorIndexFromID(controllerID);

    if (index < 0) {
        Logging::logI(file, "Invalid controllerID: %d", controllerID);
        return;
    }

    Logging::logI(file, "ControllerID: %d", controllerID);
    Logging::logI(file, "VectorIndex: %d", index);

    controllers[index].buttonUpdate(buttonID);
}

void ControllerHandler::stickResponse(int16_t axisValue, int axisID,
                                      int controllerID) {

    // check if ID is valid
    int index = vectorIndexFromID(controllerID);

    if (index < 0) {
        Logging::logI(file, "Invalid controllerID: %d", controllerID);
        return;
    }

    // convert axisID to 0 or 1 (0 = x-Axis, 1 = y-Axis)
    int stickAxisID = axisID % 2;

    if (axisID > GAME_CONTROLLER_AXIS_LEFTY) {
        controllers[index].rightStickUpdate(axisValue, stickAxisID);
    } else {
        controllers[index].leftStickUpdate(axisValue, stickAxisID);
    }
}

void ControllerHandler::triggerResponse(int16_t axisValue, int axisID,
                                        int controllerID) {

    // check if ID is valid
    int index = vectorIndexFromID(controllerID);

    if (index < 0) {
        Logging::logI(file, "Invalid controllerID: %d", controllerID);
        return;
    }

    if (axisID > GAME_CONTROLLER_AXIS_TRIGGERRIGHT) {
        controllers[index].rightTriggerUpdate(axisValue);
    } else {
        controllers[index].leftTriggerUpdate(axisValue);
    }
}

void ControllerHandler::controllerAddedResponse(int controllerID) {
    // checks for which controller object is not active (key < 0), deletes it,
    // and connects the new controller

    for (int i = 0; i < controllerAmount; i++) {
        if (controllerAssignment[i] < 0) {

            SDL_GameControllerOpen(controllerID);
            int assignedControllerID
                = SDL_JoystickInstanceID(SDL_JoystickOpen(controllerID));
            controllerAssignment[i] = assignedControllerID;
            Logging::logI(file, "Added contoller: %d", assignedControllerID);
            break;
        }
    }
}

void ControllerHandler::controllerRemovedResponse(int controllerID) {
    // matches the current controller
    for (int i = 0; i < controllerAmount; i++) {
        if (controllerAssignment[i] == controllerID) {

            // sends an update that sets all values to zero to prevent any ghost
            // movement
            for (int j = 0; j < 4; j++) {
                stickResponse(0, j, controllerID);
            };

            controllerAssignment[i] = -1;

            // closes the controller in question
            SDL_GameControllerClose(
                SDL_GameControllerFromInstanceID(controllerID));
            break;
        }
    }
}

void ControllerHandler::eventLoop() {

    bool quit = false;

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
    GameControllerAxis axis;
    int vectorIndexHolder;

    // Currently doesn't differentiate between controllers
    // Create a second array of button timers to separate controllers if needed
    int buttonHoldTimers[SDL_CONTROLLER_BUTTON_MAX] = { 0 };

    // the main update loop
    // reads events and updates accordingly per loop
    while (!quit) {

        // pulls an event from the queue (crucial for updating literally
        // anything!) this is the event loop yet to figure out why event loop
        // has to look like this
        while (SDL_PollEvent(&event)) {

            Logging::logI(file, "SDL Event type: %s (%d), From controllerID %d",
                          eventTypeToString(event.type), event.type,
                          event.cdevice.which);

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

                axis = covert_stl_axis_to_game_controller(
                    (SDL_GameControllerAxis)event.jaxis.axis);
                Logging::logV(file, "Axis motion. Axis was: %d Axis is now: %d",
                              (int)event.jaxis.axis, axis);

                if (axis <= GAME_CONTROLLER_AXIS_LAST_JOYSTICK) {
                    stickResponse(event.jaxis.value, axis, event.cdevice.which);
                } else if (axis < GAME_CONTROLLER_AXIS_MAX) {
                    triggerResponse(event.jaxis.value, axis,
                                    event.cdevice.which);
                }

                break;

            case SDL_CONTROLLERBUTTONDOWN:
                Logging::logI(file, "BUTTON: %s (%d)",
                              getButtonName(event.cbutton.button),
                              event.cbutton.button);

                // time the duration SDL_CONTROLLER_BUTTON_START is held
                switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_START:
                    buttonHoldTimers[SDL_CONTROLLER_BUTTON_START]
                        = SDL_GetTicks();
                    break;
                }

                buttonResponse(event.cbutton.button, event.cdevice.which);

                //-------------------------------------------------------------
                // Hardcoding a button that quits the loop
                // if (SDL_CONTROLLER_BUTTON_START == event.cbutton.button) {
                //     quit = true;
                // }

                break;

            case SDL_CONTROLLERBUTTONUP:
                // deactivates the hold timer
                buttonHoldTimers[event.cbutton.button] = 0;
                break;
            }
        }

        // check button timers and execute actions if needed
        for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; button++) {

            // button has been held for 1.5s
            if (buttonHoldTimers[button] != 0
                && SDL_GetTicks() - buttonHoldTimers[button] >= 1500) {

                switch (button) {

                case SDL_CONTROLLER_BUTTON_START:
                    Logging::logI(file, "Switch from controllerID %d",
                                  event.cdevice.which);

                    // switching logic here
                    vectorIndexHolder = controllerAssignment[0];
                    controllerAssignment[0] = controllerAssignment[1];
                    controllerAssignment[1] = vectorIndexHolder;

                    // start time no longer needed
                    buttonHoldTimers[button] = 0;

                    break;
                }
            }
        }
    }

    Logging::logI(file, "Quitting!: %d", event.type);
    // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<
    for (int i = 0; i < controllerAmount; i++) {
        if (controllerAssignment[i] >= 0) {
            controllerRemovedResponse(controllerAssignment[i]);
        }
    }

    SDL_Quit();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++