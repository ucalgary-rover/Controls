#include "HeadlightHandler.h"

static const char* file = "HeadlightHandler";

HeadlightHandler::HeadlightHandler(
    std::shared_ptr<HeadlightQueue> headlightQueue, const char* arduino_id) :
    HandlerInterface() {
    m_headlightQueue = headlightQueue;

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

void HeadlightHandler::start() {
    while (true) {
        HeadlightMessage headlightMessage = m_headlightQueue->pop();

        // Handle out of bounds data values
        uint brightness = headlightMessage.brightnessPercentage;
        if (brightness > 100) {
            brightness = 100;
        }

        char msg[256];
        sprintf(msg, "Light level: %d\n", brightness);
        write(serial_port, msg, strlen(msg));
    }
}