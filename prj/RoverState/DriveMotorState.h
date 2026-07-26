#pragma once

#include "pub_general.h"

struct DriveMotorState {
    float steer[WHEEL_COUNT] = {};
    float drive[WHEEL_COUNT] = {};

    DriveMotorState& operator+=(const DriveMotorState& rhs) {
        for (int i = 0; i < WHEEL_COUNT; i++) {
            this->steer[i] += rhs.steer[i];
            this->drive[i] += rhs.drive[i];
        }
        return *this;
    }

    DriveMotorState& operator-=(const DriveMotorState& rhs) {
        for (int i = 0; i < WHEEL_COUNT; i++) {
            this->steer[i] -= rhs.steer[i];
            this->drive[i] -= rhs.drive[i];
        }
        return *this;
    }

    DriveMotorState operator*(const double scalar) const {
        DriveMotorState newState;
        for (int i = 0; i < WHEEL_COUNT; i++) {
            newState.steer[i] = this->steer[i] * scalar;
            newState.drive[i] = this->drive[i] * scalar;
        }
        return newState;
    }

    DriveMotorState& operator*=(const double scalar) {
        for (int i = 0; i < WHEEL_COUNT; i++) {
            this->steer[i] *= scalar;
            this->drive[i] *= scalar;
        }
        return *this;
    }

    bool operator==(const DriveMotorState& other) const {
        bool rtn = true;
        for (int i = 0; i < WHEEL_COUNT; i++) {
            rtn &= (this->steer[i] == other.steer[i]);
            rtn &= (this->drive[i] == other.drive[i]);
        }
        return rtn;
    }

    bool operator!=(const DriveMotorState& other) const {
        return !(*this == other);
    }
};
