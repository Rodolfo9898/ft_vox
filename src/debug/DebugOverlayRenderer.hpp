#ifndef DEBUG_OVERLAY_RENDERER_HPP
# define DEBUG_OVERLAY_RENDERER_HPP

#include "../../src/debug/RenderDebug.hpp"
#include "../../src/frame/RendererColor.hpp"
#include "../../Libft/Modules/DUMB/render_window.hpp"
#include <cstdint>
#include <cstdio>

class DebugOverlayRenderer
{
    private:
        static const uint8_t *digit_bitmap(char character);
        static void draw_debug_char(ft_render_framebuffer &framebuffer,
            int32_t origin_x, int32_t origin_y, char character, int32_t scale);
        static void draw_debug_text(ft_render_framebuffer &framebuffer,
            int32_t origin_x, int32_t origin_y, const char *text,
            int32_t scale);

    public:
        DebugOverlayRenderer();
        DebugOverlayRenderer(const DebugOverlayRenderer &other);
        ~DebugOverlayRenderer();
        DebugOverlayRenderer &operator=(const DebugOverlayRenderer &other);

        static void draw_crosshair(ft_render_framebuffer &framebuffer);
        static void draw_debug_overlay(ft_render_framebuffer &framebuffer,
            const RenderDebug *debug);
};

#endif
