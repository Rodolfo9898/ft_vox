#include "../../src/camera/Camera.hpp"

Camera::Camera()
{
    this->initialize();
}

Camera::Camera(const Camera &other)
{
    *this = other;
}

Camera::~Camera()
{
}

Camera &Camera::operator=(const Camera &other)
{
    if (this != &other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        this->yaw = other.yaw;
        this->pitch = other.pitch;
        this->speed = other.speed;
        this->vertical_velocity = other.vertical_velocity;
        this->on_ground = other.on_ground;
    }
    return (*this);
}

double Camera::clamp(double value, double minimum, double maximum)
{
    if (value < minimum)
        return (minimum);
    if (value > maximum)
        return (maximum);
    return (value);
}

void Camera::initialize()
{
    this->x = 0.0;
    this->y = 78.62;
    this->z = -8.0;
    this->yaw = 0.0;
    this->pitch = 0.0;
    this->speed = 2.0;
    this->vertical_velocity = 0.0;
    this->on_ground = false;
}

void Camera::update_orientation(const CameraInput &input, double delta_seconds)
{
    if (delta_seconds <= 0.0)
        return ;
    this->yaw = this->yaw + (input.mouse_delta_x * 0.0025);
    this->pitch = this->pitch - (input.mouse_delta_y * 0.0025);
    if (input.turn_left == true)
        this->yaw = this->yaw - (1.8 * delta_seconds);
    if (input.turn_right == true)
        this->yaw = this->yaw + (1.8 * delta_seconds);
    if (input.look_up == true)
        this->pitch = this->pitch + (1.2 * delta_seconds);
    if (input.look_down == true)
        this->pitch = this->pitch - (1.2 * delta_seconds);
    this->pitch = Camera::clamp(this->pitch, -1.2, 1.2);
}

void Camera::update(const CameraInput &input, double delta_seconds)
{
    double local_x;
    double local_z;
    double move_length;
    double distance;
    double yaw_cos;
    double yaw_sin;
    double current_speed;

    if (delta_seconds <= 0.0)
        return ;
    this->update_orientation(input, delta_seconds);
    local_x = 0.0;
    local_z = 0.0;
    if (input.move_forward == true)
        local_z = local_z + 1.0;
    if (input.move_backward == true)
        local_z = local_z - 1.0;
    if (input.move_right == true)
        local_x = local_x + 1.0;
    if (input.move_left == true)
        local_x = local_x - 1.0;
    move_length = (local_x * local_x) + (local_z * local_z);
    if (move_length > 1.0)
    {
        local_x = local_x * 0.7071067811865475;
        local_z = local_z * 0.7071067811865475;
    }
    current_speed = this->speed;
    if (input.speed_boost == true)
        current_speed = current_speed * 20.0;
    distance = current_speed * delta_seconds;
    yaw_cos = std::cos(this->yaw);
    yaw_sin = std::sin(this->yaw);
    this->x = this->x + (((local_x * yaw_cos) + (local_z * yaw_sin))
        * distance);
    this->z = this->z + (((local_z * yaw_cos) - (local_x * yaw_sin))
        * distance);
}
