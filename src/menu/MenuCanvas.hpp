#ifndef MENU_CANVAS_HPP
# define MENU_CANVAS_HPP

#include "../../Libft/Modules/DUMB/render_window.hpp"
#include <cstdint>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace MC
{
    static const uint32_t BG      = 0x0D0D1AU;
    static const uint32_t PANEL   = 0x151525U;
    static const uint32_t BTN_N   = 0x1C2E50U;
    static const uint32_t BTN_H   = 0x2A507AU;
    static const uint32_t BTN_A   = 0x3A78B0U;
    static const uint32_t TEXT    = 0xEEEEFFU;
    static const uint32_t TEXT_DIM= 0x8888AAU;
    static const uint32_t TITLE   = 0xFFCC00U;
    static const uint32_t ACCENT  = 0x00B4D8U;
    static const uint32_t GREEN   = 0x4CAF50U;
    static const uint32_t SLIDER  = 0x5DA3FAU;
    static const uint32_t WHITE   = 0xFFFFFFU;
}

class MenuCanvas
{
    public:
        struct MenuInput
        {
            int  mouse_x;
            int  mouse_y;
            bool mouse_clicked;
            bool key_up;
            bool key_down;
            bool key_enter;
            bool key_escape;
        };

        struct Colors
        {
            static const uint32_t BG;
            static const uint32_t PANEL;
            static const uint32_t BTN_N;
            static const uint32_t BTN_H;
            static const uint32_t BTN_A;
            static const uint32_t TEXT;
            static const uint32_t TEXT_DIM;
            static const uint32_t TITLE;
            static const uint32_t ACCENT;
            static const uint32_t GREEN;
            static const uint32_t SLIDER;
            static const uint32_t WHITE;
        };

        static const int W;
        static const int H;


        MenuCanvas();
        MenuCanvas(const MenuCanvas &other);
        ~MenuCanvas();
        MenuCanvas &operator=(const MenuCanvas &other);

        void clear(uint32_t color = MC::BG);
        void fill_rect(int x, int y, int w, int h, uint32_t color);
        void draw_border(int x, int y, int w, int h, uint32_t color, int thickness = 1);
        void draw_text(int x, int y, const char *text, int scale, uint32_t color);
        void draw_text_centered(int cx, int y, const char *text, int scale, uint32_t color);

        bool button(int x, int y, int w, int h,
            const char *label, int scale,
            const MenuInput &in, bool selected = false);
        bool checkbox(int x, int y, bool checked,
            const char *label, int scale,
            const MenuInput &in);
        float slider(int x, int y, int w,
            float value, float vmin, float vmax,
            const char *label, int scale,
            const MenuInput &in);

        bool is_hovered(int x, int y, int w, int h, const MenuInput &in) const;
        bool was_clicked(int x, int y, int w, int h, const MenuInput &in) const;

        const uint32_t *pixels() const;
        int width()  const;
        int height() const;

        ft_render_framebuffer &framebuffer();

    private:
        std::vector<uint32_t>  pixels_;
        ft_render_framebuffer  fb_;

        void set_pixel(int x, int y, uint32_t color);
};

#endif
