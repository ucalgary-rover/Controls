#include "mission_control.h"
#include <iostream>

static void unusedButton();
static void unusedStick(int X, int Y);

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
#include "Base/Base.h"
#endif

#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
#include "Rover/Rover.h"
#endif

int main(int argc, char* argv[]) {
    std::cout << SIDE_TO_BUILD << " " << EXTENTION << "\n";

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
    std::cout << "Running Base Side: " << EXTENTION << "\n";
    Base base;

    buttonFunctions buttons_controller_1;
    buttons_controller_1.LEFT_JOYSTICK = &unusedStick;
    buttons_controller_1.RIGHT_JOYSTICK = &unusedStick;
    buttons_controller_1.buttonArray = {
        [&base]() { base.incrementChassisSpeed(2); }, // SDL_CONTROLLER_BUTTON_A
        [&base]() {
            base.incrementChassisSpeed(-2);
        },                          // SDL_CONTROLLER_BUTTON_B
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_X
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_Y
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_BACK
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_GUIDE
        [&base]() { base.quit(); }, // SDL_CONTROLLER_BUTTON_START
        [&base]() {
            base.incrementClawOpen(-2);
        }, // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [&base]() {
            base.incrementClawOpen(2);
        }, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [&base]() {
            base.incrementWristAngle(-2);
        }, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [&base]() {
            base.incrementWristAngle(2);
        }, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [&base]() { base.incrementClawY(2); }, // SDL_CONTROLLER_BUTTON_DPAD_UP
        [&base]() {
            base.incrementClawY(-2);
        }, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [&base]() {
            base.incrementClawX(-2);
        }, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [&base]() {
            base.incrementClawX(2);
        }, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    buttonFunctions buttons_controller_2;
    buttons_controller_2.LEFT_JOYSTICK = &unusedStick;
    buttons_controller_2.RIGHT_JOYSTICK = &unusedStick;
    buttons_controller_2.buttonArray = {
        [&base]() { base.incrementClawOpen(2); },  // SDL_CONTROLLER_BUTTON_A
        [&base]() { base.incrementClawOpen(-2); }, // SDL_CONTROLLER_BUTTON_B
        [&]() { unusedButton(); },                 // SDL_CONTROLLER_BUTTON_X
        [&]() { unusedButton(); },                 // SDL_CONTROLLER_BUTTON_Y
        [&]() { unusedButton(); },                 // SDL_CONTROLLER_BUTTON_BACK
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_GUIDE
        [&base]() { base.quit(); }, // SDL_CONTROLLER_BUTTON_START
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_LEFTSTICK
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_RIGHTSTICK
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_DPAD_UP
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_DPAD_DOWN
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_DPAD_LEFT
        [&]() { unusedButton(); },  // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    base.setButtons(buttons_controller_1, buttons_controller_2);

    base.start();
    std::cout << "After Start\n";

#endif
#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
    std::cout << "Running Rover Side: " << EXTENTION << "\n";
    Rover rover;
    rover.start();

#endif

    return 0;
}

static void unusedButton() { std::cout << "Button Unused\n"; }

static void unusedStick(int X, int Y) {
    std::cout << "X: " << X << "Y: " << Y << "\n";
}