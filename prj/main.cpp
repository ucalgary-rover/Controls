#include "Base/Base.h"
#include "mission_control.h"
#include <iostream>

static void unusedButton();
static void unusedStick(int X, int Y);

#if SIDE_TO_BUILD == BUILD_SIDE_BASE

#endif

#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
#endif

int main(int argc, char* argv[]) {
    std::cout << SIDE_TO_BUILD << " " << EXTENTION << "\n";

#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
    std::cout << "Running Base Side: " << EXTENTION << "\n";
    Base base;

    buttonFunctions buttons_controller_1;
    buttons_controller_1.LEFT_JOYSTICK = &unusedStick;
    buttons_controller_1.RIGHT_JOYSTICK = &unusedStick;
    buttons_controller_1.buttonArray = {
        [&base]() { base.incrementChassisSpeed(2); },
        [&base]() { base.incrementChassisSpeed(-2); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&base]() { base.quit(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
    };

    buttonFunctions buttons_controller_2;
    buttons_controller_2.LEFT_JOYSTICK = &unusedStick;
    buttons_controller_2.RIGHT_JOYSTICK = &unusedStick;
    buttons_controller_2.buttonArray = {
        [&base]() { base.incrementClawOpen(2); },
        [&base]() { base.incrementClawOpen(-2); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&base]() { base.quit(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
        [&]() { unusedButton(); },
    };

    base.setButtons(buttons_controller_1, buttons_controller_2);

    base.start();
    std::cout << "After Start\n";

#endif

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
    std::cout << "Running Rover Side: " << EXTENTION << "\n";
    // Rover();
#endif

    return 0;
}

static void unusedButton() { std::cout << "Button Unused\n"; }

static void unusedStick(int X, int Y) {
    std::cout << "X: " << X << "Y: " << Y << "\n";
}