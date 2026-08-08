#pragma once

class Motor {
public:
    virtual void setTargetPosition(float target) { };
    virtual float getCurrentPosition() { return 0.f; };
    virtual void setZeroPosition() { };
};