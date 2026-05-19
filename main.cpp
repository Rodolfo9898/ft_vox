#include "Libft/Modules/DUMB/controls.hpp"
#include "Libft/Modules/DUMB/render_window.hpp"
#include "Libft/Modules/Errno/errno.hpp"
#include <cstdint>

static uint32_t ft_vox_color_from_frame(uint32_t frame_index)
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    red = static_cast<uint8_t>((frame_index * 2U) & 0xFFU);
    green = static_cast<uint8_t>((64U + frame_index) & 0xFFU);
    blue = static_cast<uint8_t>(128U);
    return ((static_cast<uint32_t>(red) << 24U)
        | (static_cast<uint32_t>(green) << 16U)
        | (static_cast<uint32_t>(blue) << 8U)
        | 0xFFU);
}

int main()
{
    ft_render_window       window;
    ft_render_window_desc  description;
    ft_render_screen_size  screen_size;
    uint32_t               frame_index;
    int32_t                error_code;

    error_code = window.initialize();
    if (error_code != FT_ERR_SUCCESS)
        return (1);
    screen_size = ft_render_get_primary_screen_size();
    if (screen_size.width <= 0 || screen_size.height <= 0)
    {
        screen_size.width = 1280;
        screen_size.height = 720;
    }
    description.width = screen_size.width;
    description.height = screen_size.height;
    description.title = "ft_vox";
    description.flags = FT_RENDER_WINDOW_FLAG_FULLSCREEN;
    error_code = window.initialize(description);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)window.destroy();
        return (1);
    }
    frame_index = 0U;
    while (window.should_close() == FT_FALSE)
    {
        if (window.poll_events() != FT_ERR_SUCCESS)
            break ;
        if (window.should_close() == FT_TRUE)
            break ;
        ft_dumb_controls_poll();
        if (ft_dumb_control_is_down(FT_DUMB_CONTROL_BACK) == FT_TRUE)
            break ;
        if (window.clear(ft_vox_color_from_frame(frame_index)) != FT_ERR_SUCCESS)
            break ;
        if (window.present() != FT_ERR_SUCCESS)
            break ;
        frame_index = frame_index + 1U;
    }
    if (window.destroy() != FT_ERR_SUCCESS)
        return (1);
    return (0);
}
