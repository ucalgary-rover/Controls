#pragma once

#include "HandlerInterface.h"
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include "Logging.h"

class HeadlightHandler : public HandlerInterface {
public:
    HeadlightHandler(const char* arduino_id);

    ~HeadlightHandler();

    /** @brief Sets the headlights percentage level
     * @return None
     */
    void setHeadlightPercentage(int percentage);

    void start();

private:
    char control_port[32];
    int serial_port;
};