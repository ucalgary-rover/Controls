#include "mission_control.h"
#include "pub_general.h"
#include <iostream>

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