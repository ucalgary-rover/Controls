/*
This file is an example of what the SDL2 event queue looks like
Build and run with:
g++ -I src/include test/controllerTest.cpp -o ControllerTest -lSDL2main -lSDL2;
./ControllerTest
 */

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// Helper function to get button name
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

// detecting inital controller and opening it
int main(int argc, char* argv[]) {

    // initializing SDL sub systems`
    // This combo works, it is a little strange tho
    // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Check for game controllers
    if (SDL_NumJoysticks() < 1) {
        std::cout << "No controllers connected!" << std::endl
                  << SDL_NumJoysticks();
    }

    int numJoy
        = SDL_NumJoysticks(); // two controllers returns only two joysticks
    std::cout << numJoy << std::endl;

    // list for controller pointers
    SDL_GameController* pControllerList[2];

    bool quit = false;

    // the struct with all the data <----------
    SDL_Event event;
    SDL_Joystick* joystick;
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
                joystick = SDL_GameControllerGetJoystick(
                    SDL_GameControllerOpen(event.cdevice.which));
                std::cout << "Added ->" << SDL_JoystickInstanceID(joystick)
                          << std::endl;
                break;

            case SDL_CONTROLLERDEVICEREMOVED:

                // checks if there is a controller id and the controller id
                // matches the current controller
                std::cout << "Removed -> " << event.cdevice.which << std::endl;
                SDL_GameControllerClose(
                    SDL_GameControllerFromInstanceID(event.cdevice.which));
                break;

            case SDL_JOYAXISMOTION:

                std::cout << "\nAxis motion\n";

                // axis index has to be converted to be an int to match the enum
                // for the axes value gets stuck sometimes, can be adjusted with
                // the deadzone
                std::cout << event.cdevice.which << std::endl   // device id
                          << (int)event.jaxis.axis << std::endl // axis id
                          << event.jaxis.value << std::endl;    // value
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                std::cout << "\nButton press\n";
                std::cout << getButtonName(event.cbutton.button) << std::endl;
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X) {
                    quit = true;
                }

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                break;
            }
        }
    }

    // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<

    // loops thru and closes all controllers
    for (SDL_GameController* pController : pControllerList) {
        SDL_GameControllerClose(pController);
    }

    SDL_Quit();

    return 0;
}
