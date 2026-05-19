#include "camera.hpp"

void ft_vox_camera_init(ft_vox_camera *camera)
{
    if (camera == nullptr)
        return ;
    camera->x = 0.0;
    camera->y = 64.0;
    camera->z = 0.0;
    camera->yaw = 0.0;
    camera->speed = 1.0;
    return ;
}

void ft_vox_camera_update(ft_vox_camera *camera,
    const ft_vox_camera_input &input, double delta_seconds)
{
    double move_x;
    double move_z;
    double move_length;
    double distance;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    move_x = 0.0;
    move_z = 0.0;
    if (input.move_forward == true)
        move_z = move_z + 1.0;
    if (input.move_backward == true)
        move_z = move_z - 1.0;
    if (input.move_right == true)
        move_x = move_x + 1.0;
    if (input.move_left == true)
        move_x = move_x - 1.0;
    move_length = (move_x * move_x) + (move_z * move_z);
    if (move_length > 1.0)
    {
        move_x = move_x * 0.7071067811865475;
        move_z = move_z * 0.7071067811865475;
    }
    distance = camera->speed * delta_seconds;
    camera->x = camera->x + (move_x * distance);
    camera->z = camera->z + (move_z * distance);
    return ;
}
