#pragma once

#include <cstdint>

struct ArmState {
    float x;
    float y;
    float z;
    int pitch;
    int roll;
    int clawOpen;

    ArmState operator+(const ArmState& rhs) const {
        ArmState newState;
        newState.x = this->x + rhs.x;
        newState.y = this->y + rhs.y;
        newState.z = this->z + rhs.z;
        newState.pitch = this->pitch + rhs.pitch;
        newState.roll = this->roll + rhs.roll;
        newState.clawOpen = this->clawOpen + rhs.clawOpen;
        return newState;
    }

    ArmState& operator+=(const ArmState& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->pitch += rhs.pitch;
        this->roll += rhs.roll;
        this->clawOpen += rhs.clawOpen;
        return *this;
    }

    ArmState operator*(const double scalar) const {
        ArmState newState;
        newState.x = this->x * scalar;
        newState.y = this->y * scalar;
        newState.z = this->z * scalar;
        newState.pitch = this->pitch * scalar;
        newState.roll = this->roll * scalar;
        newState.clawOpen = this->clawOpen * scalar;
        return newState;
    }

    ArmState& operator*=(const double scalar) {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        this->pitch *= scalar;
        this->roll *= scalar;
        this->clawOpen *= scalar;
        return *this;
    }

    bool operator==(const ArmState& other) const {
        return (this->x == other.x) && (this->y == other.y)
               && (this->z == other.z) && (this->pitch == other.pitch)
               && (this->roll == other.roll)
               && (this->clawOpen == other.clawOpen);
    }

    bool operator!=(const ArmState& other) const { return !(*this == other); }
};
