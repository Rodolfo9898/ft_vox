#ifndef FT_VOX_DEBUG_VIEW_HPP
# define FT_VOX_DEBUG_VIEW_HPP

#include "../../Libft/Modules/DUMB/render_window.hpp"
#include "../camera/camera.hpp"

void ft_vox_debug_view_draw(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera);

#endif
