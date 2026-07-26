#pragma once

#include <cstdint>

struct DriveState {
    int speed;
    int heading;
    int angularVelocity;

    DriveState& operator+=(const DriveState& rhs) {
        this->speed += rhs.speed;
        this->heading += rhs.heading;
        this->angularVelocity += rhs.angularVelocity;
        return *this;
    }

    DriveState& operator-=(const DriveState& rhs) {
        this->speed -= rhs.speed;
        this->heading -= rhs.heading;
        this->angularVelocity -= rhs.angularVelocity;
        return *this;
    }

    DriveState& operator*=(const double scalar) {
        this->speed *= scalar;
        this->heading *= scalar;
        this->angularVelocity *= scalar;
        return *this;
    }

    bool operator==(const DriveState& other) const {
        return (this->speed == other.speed) && (this->heading == other.heading)
               && (this->angularVelocity == other.angularVelocity);
    }

    bool operator!=(const DriveState& other) const { return !(*this == other); }
};
