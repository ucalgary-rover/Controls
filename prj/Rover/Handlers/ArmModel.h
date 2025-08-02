#include <array>
#include <vector>

class ArmModel {
public:
    static void initialize();
    static std::vector<std::array<double, 3>> getPositions();

    static std::vector<std::array<double, 3>> getOrientations();

    static std::array<int, 6> getAngles();

    static bool updateJointAngles(std::array<int, 6> new_angles);

    static std::array<int, 6>
    generateWristPosition(std::array<double, 3> desired_wrist_position);

    static std::array<int, 6>
    generateClawOrientation(std::array<int, 6> current_angles,
                            double claw_pitch, double claw_roll);

private:
    static bool is_initialized;
};