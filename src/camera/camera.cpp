#include "camera.hpp"
#include <cmath>

static double ft_vox_clamp(double value, double minimum, double maximum)
{
    if (value < minimum)
        return (minimum);
    if (value > maximum)
        return (maximum);
    return (value);
}

void ft_vox_camera_init(ft_vox_camera *camera)
{
    if (camera == nullptr)
        return ;
    camera->x = 0.0;
    camera->y = 92.0;
    camera->z = -42.0;
    camera->yaw = 0.0;
    camera->pitch = -0.22;
    camera->speed = 10.0;
    return ;
}

void ft_vox_camera_update(ft_vox_camera *camera,
    const ft_vox_camera_input &input, double delta_seconds)
{
    double local_x;
    double local_z;
    double move_length;
    double distance;
    double yaw_cos;
    double yaw_sin;
    double speed;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    if (input.turn_left == true)
        camera->yaw = camera->yaw - (1.8 * delta_seconds);
    if (input.turn_right == true)
        camera->yaw = camera->yaw + (1.8 * delta_seconds);
    if (input.look_up == true)
        camera->pitch = camera->pitch + (1.2 * delta_seconds);
    if (input.look_down == true)
        camera->pitch = camera->pitch - (1.2 * delta_seconds);
    camera->pitch = ft_vox_clamp(camera->pitch, -1.1, 0.35);
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
    speed = camera->speed;
    if (input.speed_boost == true)
        speed = speed * 20.0;
    distance = speed * delta_seconds;
    yaw_cos = std::cos(camera->yaw);
    yaw_sin = std::sin(camera->yaw);
    camera->x = camera->x + (((local_x * yaw_cos) + (local_z * yaw_sin))
        * distance);
    camera->z = camera->z + (((local_z * yaw_cos) - (local_x * yaw_sin))
        * distance);
    return ;
}
