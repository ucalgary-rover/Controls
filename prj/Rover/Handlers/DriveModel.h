#ifndef DRIVE_MODEL_H
#define DRIVE_MODEL_H

#pragma once

#include "Rover/Systems/Drive.h"
#include <array>

class DriveModel {
public:
    static void initialize(Drive* drive);

    /**
     * @brief Takes in an angle and derives a speed value from it
     * @param stickAngle The input angle from the joystick
     * @return A speed value based on the angle
     */
    static float spotTurnSpeed(int stickAngle);

    /**
     * @brief Takes in an angle and derives an adjusted angle and direction
     * from it
     * @param stickTheta The input angle from the left joystick
     * @return An adjusted angle within wheel range of motion. Sign determines
     * direction of wheel spin
     */
    static int strafeAngleAdjust(int stickTheta);

    /**
     * @brief Determines the heading angle of the radial turn
     * @param stickAngle Int of angle of joystick by used for sharpness of turn
     * (in degrees)
     * @return The absolute value of the heading angle of the turn in radians
     */
    static float radialTurnHeadingAngle(int stickAngle);

    /**
     * @brief Determines the angle of a leading wheel (inner or outer)
     * @param headingAngle Float angle of desired turn (in radians)
     * @param innerFlag If true, calculates for the inner wheel, if false
     * calculates the outer
     * @return The specified wheel angle in degrees
     */
    static float radialTurnWheelAngles(float headingAngle, bool innerFlag);

    /**
     * @brief Get the last radial output direction
     * @return The sign of the direction of the last radial output
     */
    static int getLastRadialOutput();

    /**
     * @brief Set the last radial output direction
     * @param value The sign of the direction of the last radial output
     */
    static void setLastRadialOutput(int value);

private:
    static Drive* m_drive;
    static int m_lastRadialOutput;
};

#endif
