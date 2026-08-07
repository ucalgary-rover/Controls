#pragma once

#include <cstdint>

/**
 * This data struct encodes the cylindrical coordinates model of the rover arm position
 * P(theta, r, z, pitch, roll, clawOpen)
 * theta: The rotation of the base.
 * r: The horiznontal distance of the wrist from the base of the arm.
 * z: The vertical distance of the wrist from the base of the arm.
 * pitch: The pitch of the claw.
 * roll: The roll of the claw.
 * clawOpen: The extension of the claw.
 */

struct ArmStateCylindrical {
    float theta;
    float r;
    float z;
    int pitch;
    int roll;
    int clawOpen;

    ArmStateCylindrical operator+(const ArmStateCylindrical& rhs) const {
        ArmStateCylindrical newState;
        newState.theta = this->theta + rhs.theta;
        newState.r = this->r + rhs.r;
        newState.z = this->z + rhs.z;
        newState.pitch = this->pitch + rhs.pitch;
        newState.roll = this->roll + rhs.roll;
        newState.clawOpen = this->clawOpen + rhs.clawOpen;
        return newState;
    }

    ArmStateCylindrical& operator+=(const ArmStateCylindrical& rhs) {
        this->theta += rhs.theta;
        this->r += rhs.r;
        this->z += rhs.z;
        this->pitch += rhs.pitch;
        this->roll += rhs.roll;
        this->clawOpen += rhs.clawOpen;
        return *this;
    }

    ArmStateCylindrical operator*(const double scalar) const {
        ArmStateCylindrical newState;
        newState.theta = this->theta * scalar;
        newState.r = this->r * scalar;
        newState.z = this->z * scalar;
        newState.pitch = this->pitch * scalar;
        newState.roll = this->roll * scalar;
        newState.clawOpen = this->clawOpen * scalar;
        return newState;
    }

    ArmStateCylindrical& operator*=(const double scalar) {
        this->theta *= scalar;
        this->r *= scalar;
        this->z *= scalar;
        this->pitch *= scalar;
        this->roll *= scalar;
        this->clawOpen *= scalar;
        return *this;
    }

    bool operator==(const ArmStateCylindrical& other) const {
        return (this->theta == other.theta) && (this->r == other.r)
               && (this->z == other.z) && (this->pitch == other.pitch)
               && (this->roll == other.roll)
               && (this->clawOpen == other.clawOpen);
    }

    bool operator!=(const ArmStateCylindrical& other) const {
        return !(*this == other);
    }
};
