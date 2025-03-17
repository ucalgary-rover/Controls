// #include <SDL2/SDL.h>

#pragma once

enum BuildSide {
    BUILD_SIDE_BASE,
    BUILD_SIDE_ROVER,
};

enum ExtentionType {
    EXTENTION_TYPE_ARM,
    EXTENTION_TYPE_SCIENCE_TOOL,
    EXTENTION_TYPE_NONE,
};

enum MessageFormat {
    MESSAGE_FORMAT_WHEEL,
    MESSAGE_FORMAT_ARM,
    MESSAGE_FORMAT_SCIENCE_TOOL,
};

// Used to streamline the struct (Don't keep in final)<<<<<<<<<<<<<<<<<<<<<<<<<<
typedef void (*ControllerFunc)(void* args);

// a struct for function pointers used for button mapping
// fill buttonArray with button functions
// Can treat the array as a dictionary and these values as keys
// SDL_CONTROLLER_BUTTON_A
// SDL_CONTROLLER_BUTTON_B
// SDL_CONTROLLER_BUTTON_X
// SDL_CONTROLLER_BUTTON_Y
// SDL_CONTROLLER_BUTTON_BACK
// SDL_CONTROLLER_BUTTON_GUIDE
// SDL_CONTROLLER_BUTTON_START
// SDL_CONTROLLER_BUTTON_LEFTSTICK
// SDL_CONTROLLER_BUTTON_RIGHTSTICK
// SDL_CONTROLLER_BUTTON_LEFTSHOULDER
// SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
// SDL_CONTROLLER_BUTTON_DPAD_UP
// SDL_CONTROLLER_BUTTON_DPAD_DOWN
// SDL_CONTROLLER_BUTTON_DPAD_LEFT
// SDL_CONTROLLER_BUTTON_DPAD_RIGHT
struct buttonFunctions {
    ControllerFunc buttonArray[15];
    void (*LEFT_JOYSTICK)(int xValue, int yValue);
    void (*RIGHT_JOYSTICK)(int xValue, int yValue);
};

// Generic Message Format
struct Generic {
    int value;
};

// Message format for wheels
struct WheelMessage {
    int velocity;
    int theta;
    int angle_velocity;
};

// Message format for arm
struct ArmMessage {
    int armXPos;
    int armYPos;
    int armZPos;

    int clawXPos;
    int clawYPos;
    int clawOpen;
    int clawRotation;

    int wristRotation;
};

// Message format for science tool
struct ScienceToolMessage {
    int moveUpDown;
    int moveLeftRight;

    int xPos;
    int yPos;
};
