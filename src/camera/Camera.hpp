#ifndef CAMERA_HPP
# define CAMERA_HPP

#include "../../src/camera/CameraInput.hpp"
#include <cmath>

class Camera
{
    public:
        double x;
        double y;
        double z;
        double yaw;
        double pitch;
        double speed;
        double vertical_velocity;
        bool on_ground;

        Camera();
        Camera(const Camera &other);
        ~Camera();
        Camera &operator=(const Camera &other);

        void initialize();
        void update_orientation(const CameraInput &input,
            double delta_seconds);
        void update(const CameraInput &input, double delta_seconds);

    private:
        static double clamp(double value, double minimum, double maximum);
};

#endif
