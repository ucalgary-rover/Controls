#pragma once

#include "Logging.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <functional>
#include <string>

#define NAMEOF(var) #var

typedef std::function<void(uint8_t)> ButtonCallback;

#define REGISTER_BUTTON(callbacks, buttonID, buttonCallback)                   \
    callbacks[buttonID] = [this](uint8_t buttonID) { buttonCallback(buttonID); }

class ControllerLayout {
public:
    ControllerLayout() { }

    ControllerLayout(const std::string& file) {
        this->filename = file;
        // Initialize Layout API
        // clang-format off

        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_A, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_B, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_X, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_Y, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_BACK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_GUIDE, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_START, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSTICK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSTICK, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_UP, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_DOWN, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_LEFT, unusedButton);
        REGISTER_BUTTON(buttonCallbacks, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, unusedButton);
        // clang-format on
    }

    void unusedButton(uint8_t buttonID);
    void unusedStick(int X, int Y);
    void unusedTrigger(int axisValue);
    virtual std::string getName() { return filename; }

    virtual void buttonResponse(uint8_t buttonID) { unusedButton(buttonID); }
    virtual void leftStickResponse(int xValue, int yValue) {
        unusedStick(xValue, yValue);
    }
    virtual void rightStickResponse(int xValue, int yValue) {
        unusedStick(xValue, yValue);
    }
    virtual void leftTriggerResponse(int16_t axisValue) {
        unusedTrigger(axisValue);
    }
    virtual void rightTriggerResponse(int16_t axisValue) {
        unusedTrigger(axisValue);
    }

protected:
    std::string filename = "DefaultController";

    ButtonCallback buttonCallbacks[SDL_CONTROLLER_BUTTON_MAX];

    /**
     * Calculates the magnitude of the stick's position from the center
     *
     * @param axisX The distance along the x axis
     * @param axisY The distance along the y axis
     * @param max The maximum limit for the output
     * @return The magnitude of the stick's position as a percentage of 255 (the
     * full radius) (0 - 100%)
     */
    static int stickMagnitude(int axisX, int axisY);

    /**
     * Calculates the angle of the stick's position counterclockwise from
     * the positive y axis in DEGREES (which is ironically reported with a
     * negative value by SDL)
     *
     * @param axisX The distance along the x axis
     * @param axisY The distance along the y axis
     */
    static int stickAngle(int axisX, int axisY);

    // Converter functions
    static float degreeToRadian(int n);

    static int radianToDegree(float n);

    template <typename T>
    static T clampVal(T val, T min, T max) {
        if (val < min) {
            val = min;
        } else if (val > max) {
            val = max;
        }

        return val;
    }

    // General getter, setter, and incrementor for member variables
    template <typename T>
    void triggerToIncrement(int triggerValue, int* compare, T* var, T n, T min,
                            T max, const char* name) {
        if (triggerValue > 0 && *compare < 0) {
            incrementVal(var, n, min, max, name);
        }
        *compare = triggerValue;
    }

    void stickChangeAxis(int axisX, int axisY, float* varX, float* varY,
                         float maxChangeX, float maxChangeY, float rangeX,
                         float rangeY, const char* nameX, const char* nameY);

    // Template functions for setting and incrementing member variables
    /**
     * Sets a member variable to a value within specified limits
     *
     * @param member The member variable to set
     * @param n The value to set the member variable to
     * @param min The minimum limit for the member variable
     * @param max The maximum limit for the member variable
     * @param name The name of the member variable (for logging)
     */
    template <typename T>
    void setVal(T* val, T n, T min, T max, const char* name) {
        *val = clampVal(n, min, max);
        if (std::is_same<T, int>::value) {
            Logging::logI(filename.c_str(), "Setting %s to %d", name, *val);
        } else if (std::is_same<T, float>::value) {
            Logging::logI(filename.c_str(), "Setting %s to %f", name, *val);
        }
    }

    /**
     * Increments a member variable by a value within specified limits
     *
     * @param member The member variable to increment
     * @param n The value to increment the member variable by
     * @param min The minimum limit for the member variable
     * @param max The maximum limit for the member variable
     * @param name The name of the member variable (for logging)
     */
    template <typename T>
    void incrementVal(T* val, T n, T min, T max, const char* name) {
        *val = clampVal(*val + n, min, max);
        if (std::is_same<T, int>::value) {
            Logging::logI(filename.c_str(), "Setting %s to %d", name, *val);
        } else if (std::is_same<T, float>::value) {
            Logging::logI(filename.c_str(), "Setting %s to %f", name, *val);
        }
    }
};
