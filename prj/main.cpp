#include "mission_control.h"
#include <iostream>

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
#include "Base\Base.h"
#endif

#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
#include "Rover\rover.h"
#endif

#include "Rover/Systems/Arm.cpp"
#include "Rover/Systems/Drive.cpp"

int main() {
    std::cout << SIDE_TO_BUILD << " " << EXTENTION << "\n";

#if SIDE_TO_BUILD == BASE
    std::cout << "Running Base Side: " << EXTENTION << "\n";
    // Base();
#endif

#if SIDE_TO_BUILD == ROVER
    std::cout << "Running Rover Side: " << EXTENTION << "\n";
    // Rover();
#endif
}