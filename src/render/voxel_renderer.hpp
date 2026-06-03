#ifndef FT_VOX_VOXEL_RENDERER_HPP
# define FT_VOX_VOXEL_RENDERER_HPP

#include "../../Libft/Modules/DUMB/render_window.hpp"
#include "../camera/camera.hpp"
#include "../world/voxel_world.hpp"

void ft_vox_render_world(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera, const ft_vox_world &world);

#endif
