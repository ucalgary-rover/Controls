#pragma once

#include "pub_general.h"

struct ArmMotorState {
    int motorValues[MOTOR_ID_END];

    ArmMotorState operator+(const ArmMotorState& rhs) const {
        ArmMotorState newState;
        for (int i = 0; i < MOTOR_ID_END; i++) {
            newState.motorValues[i] = this->motorValues[i] + rhs.motorValues[i];
        }
        return newState;
    }

    ArmMotorState& operator+=(const ArmMotorState& rhs) {
        for (int i = 0; i < MOTOR_ID_END; i++) {
            this->motorValues[i] += rhs.motorValues[i];
        }
        return *this;
    }

    ArmMotorState operator*(const double scalar) const {
        ArmMotorState newState;
        for (int i = 0; i < MOTOR_ID_END; i++) {
            newState.motorValues[i] = this->motorValues[i] * scalar;
        }
        return newState;
    }

    ArmMotorState& operator*=(const double scalar) {
        for (int i = 0; i < MOTOR_ID_END; i++) {
            this->motorValues[i] *= scalar;
        }
        return *this;
    }

    bool operator==(const ArmMotorState& other) const {
        bool rtn = true;
        for (int i = 0; i < MOTOR_ID_END; i++) {
            rtn &= (this->motorValues[i] == other.motorValues[i]);
        }
        return rtn;
    }

    bool operator!=(const ArmMotorState& other) const {
        return !(*this == other);
    }
};