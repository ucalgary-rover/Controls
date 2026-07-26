#pragma once

#include "HandlerInterface.h"

#include "Message.h"
#include "MessageQueue.h"
#include <memory>

#include "HandlerInterface.h"
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include "Logging.h"

using HeadlightQueue = MessageQueue<HeadlightMessage>; // Change type as needed

class HeadlightHandler : public HandlerInterface {
public:
    HeadlightHandler(std::shared_ptr<HeadlightQueue> headlightQueue,
                     const char* arduino_id);

    void start() override;

private:
    std::shared_ptr<HeadlightQueue> m_headlightQueue;

    char control_port[32];
    int serial_port;
};
