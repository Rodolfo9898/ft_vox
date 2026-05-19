#ifndef FT_VOX_CAMERA_HPP
# define FT_VOX_CAMERA_HPP

struct ft_vox_camera_input
{
    bool move_forward;
    bool move_backward;
    bool move_left;
    bool move_right;
};

struct ft_vox_camera
{
    double x;
    double y;
    double z;
    double yaw;
    double speed;
};

void ft_vox_camera_init(ft_vox_camera *camera);
void ft_vox_camera_update(ft_vox_camera *camera,
    const ft_vox_camera_input &input, double delta_seconds);

#endif
