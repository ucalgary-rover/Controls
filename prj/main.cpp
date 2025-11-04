#include "mission_control.h"
#include <iostream>

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
#include "Base/Base.h"
#endif

#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
#include "Rover/Rover.h"
#endif

static const char* file = "Main";

int main(int argc, char* argv[]) {
    Logging::logInit();

    Logging::logI(file,
                  "This is Mission Control. %d, %d, you are good for launch",
                  SIDE_TO_BUILD, EXTENTION);

#if SIDE_TO_BUILD == BUILD_SIDE_BASE
    Logging::logI(file, "Running Base Side: %d", EXTENTION);
    Base base;

    base.start();

#endif
#if SIDE_TO_BUILD == BUILD_SIDE_ROVER
    Logging::logI(file, "Running Rover Side: %d", EXTENTION);
    Rover rover;

    rover.start();

#endif

    Logging::logDeinit();
    return 0;
}
