#include "mission_control.h"
#include <iostream>

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
