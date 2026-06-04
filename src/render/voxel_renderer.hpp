#ifndef FT_VOX_VOXEL_RENDERER_HPP
# define FT_VOX_VOXEL_RENDERER_HPP

#include "../../Libft/Modules/DUMB/render_window.hpp"
#include "../camera/camera.hpp"
#include "../world/voxel_world.hpp"

struct ft_vox_render_debug
{
    double  fps;
    double  frame_ms;
    int32_t visible_chunks;
    int32_t loaded_chunks;
    int32_t render_distance;
};

void ft_vox_render_world(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera, const ft_vox_world &world,
    double display_fps = -1.0);
void ft_vox_render_world(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera, const ft_vox_world &world,
    const ft_vox_render_debug *debug);

#endif
