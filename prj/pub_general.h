enum BuildSide {
    BASE,
    ROVER,
};

enum ExtentionType {
    ARM,
    SCIENCE_TOOL,
    NONE,
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
