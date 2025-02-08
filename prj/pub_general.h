enum class BuildSide {
    BASE,
    ROVER,
};

enum class ExtentionType {
    ARM,
    SCIENCE_TOOL,
    NONE,
};

// Used to streamline the struct
typedef void (*ControllerFunc)(void* args);

// a struct for function pointers used for button mapping
struct buttonFunctions {
    ControllerFunc BUTTON_A;
    ControllerFunc BUTTON_B;
    ControllerFunc BUTTON_X;
    ControllerFunc BUTTON_Y;
    ControllerFunc BUTTON_BACK;
    ControllerFunc BUTTON_GUIDE;
    ControllerFunc BUTTON_START;
    ControllerFunc BUTTON_LEFTSTICK;
    ControllerFunc BUTTON_RIGHTSTICK;
    ControllerFunc BUTTON_LEFTSHOULDER;
    ControllerFunc BUTTON_RIGHTSHOULDER;
    ControllerFunc BUTTON_DPAD_UP;
    ControllerFunc BUTTON_DPAD_DOWN;
    ControllerFunc BUTTON_DPAD_LEFT;
    ControllerFunc BUTTON_DPAD_RIGHT;
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
};
