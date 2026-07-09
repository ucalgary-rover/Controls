#include "Rover/Handlers/HeadlightHandler.h"

static const char* file = "HeadlightHandler";

HeadlightHandler::HeadlightHandler(const char* arduino_id) {
    char resolved_path[256] = "/dev/serial/by-id/";
    char buffer[256];

    strcat(resolved_path, arduino_id);
    int resolved_len = readlink(resolved_path, buffer, sizeof(buffer) - 1);

    strcpy(control_port, "/dev/");
    strcat(control_port, strrchr(buffer, '/') + 1);

    if (control_port[0] == '\0') {
        Logging::logE("HeadlightHandler",
                      "Failed to find Arduino serial port for %s\n",
                      arduino_id);
        return;
    }

    serial_port = open(control_port, O_RDWR);
    if (serial_port < 0) {
        Logging::logE("HeadlightHandler", "Failed to open %s: %s\n",
                      control_port, strerror(errno));
        return;
    }

    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        Logging::logE("HeadlightHandler", "Error %i from tcgetattr: %s\n",
                      errno, strerror(errno));
        close(serial_port);
        serial_port = -1;
        return;
    }

    // Set input and output baud rates to 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
}

HeadlightHandler::~HeadlightHandler() { close(serial_port); }

void HeadlightHandler::setHeadlightPercentage(int percentage) {
    char msg[256];
    sprintf(msg, "Light level: %d\n", percentage);
    write(serial_port, msg, strlen(msg));
}

void HeadlightHandler::start() {
    // This function is intentionally left empty as the HeadlightHandler does not require a separate thread for operation.
}