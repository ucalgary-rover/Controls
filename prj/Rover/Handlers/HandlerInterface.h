
#ifndef HANDLERINTERFACE_H
#define HANDLERINTERFACE_H

#pragma once

class HandlerInterface {
public:
    virtual void instantiateThreads() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif