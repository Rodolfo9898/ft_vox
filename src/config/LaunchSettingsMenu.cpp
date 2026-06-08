#include "../../src/config/LaunchSettingsMenu.hpp"

#include "../../Libft/Modules/DUMB/controls.hpp"
#include "../../src/gpur/GpuRenderer.hpp"
#include <cstdio>
#include <cstring>
#include <vector>

namespace
{
    struct MenuImage
    {
        int32_t width;
        int32_t height;
        uint32_t *pixels;
    };

    static uint32_t make_color(uint32_t red, uint32_t green, uint32_t blue)
    {
        return ((red << 16U) | (green << 8U) | blue);
    }

    static const char *glyph_pattern(char character)
    {
        static const char glyph_space[] = "00000""00000""00000""00000""00000""00000""00000";
        static const char glyph_dash[] = "00000""00000""00000""11111""00000""00000""00000";
        static const char glyph_colon[] = "00000""00100""00100""00000""00100""00100""00000";
        static const char glyph_period[] = "00000""00000""00000""00000""00000""00110""00110";
        static const char glyph_0[] = "01110""10001""10011""10101""11001""10001""01110";
        static const char glyph_1[] = "00100""01100""00100""00100""00100""00100""01110";
        static const char glyph_2[] = "01110""10001""00001""00010""00100""01000""11111";
        static const char glyph_3[] = "11110""00001""00001""01110""00001""00001""11110";
        static const char glyph_4[] = "00010""00110""01010""10010""11111""00010""00010";
        static const char glyph_5[] = "11111""10000""10000""11110""00001""00001""11110";
        static const char glyph_6[] = "01110""10000""10000""11110""10001""10001""01110";
        static const char glyph_7[] = "11111""00001""00010""00100""01000""01000""01000";
        static const char glyph_8[] = "01110""10001""10001""01110""10001""10001""01110";
        static const char glyph_9[] = "01110""10001""10001""01111""00001""00001""01110";
        static const char glyph_a[] = "01110""10001""10001""11111""10001""10001""10001";
        static const char glyph_b[] = "11110""10001""10001""11110""10001""10001""11110";
        static const char glyph_c[] = "01110""10001""10000""10000""10000""10001""01110";
        static const char glyph_d[] = "11110""10001""10001""10001""10001""10001""11110";
        static const char glyph_e[] = "11111""10000""10000""11110""10000""10000""11111";
        static const char glyph_f[] = "11111""10000""10000""11110""10000""10000""10000";
        static const char glyph_g[] = "01110""10001""10000""10111""10001""10001""01110";
        static const char glyph_h[] = "10001""10001""10001""11111""10001""10001""10001";
        static const char glyph_i[] = "01110""00100""00100""00100""00100""00100""01110";
        static const char glyph_j[] = "00001""00001""00001""00001""10001""10001""01110";
        static const char glyph_k[] = "10001""10010""10100""11000""10100""10010""10001";
        static const char glyph_l[] = "10000""10000""10000""10000""10000""10000""11111";
        static const char glyph_m[] = "10001""11011""10101""10101""10001""10001""10001";
        static const char glyph_n[] = "10001""11001""10101""10011""10001""10001""10001";
        static const char glyph_o[] = "01110""10001""10001""10001""10001""10001""01110";
        static const char glyph_p[] = "11110""10001""10001""11110""10000""10000""10000";
        static const char glyph_q[] = "01110""10001""10001""10001""10101""10010""01101";
        static const char glyph_r[] = "11110""10001""10001""11110""10100""10010""10001";
        static const char glyph_s[] = "01111""10000""10000""01110""00001""00001""11110";
        static const char glyph_t[] = "11111""00100""00100""00100""00100""00100""00100";
        static const char glyph_u[] = "10001""10001""10001""10001""10001""10001""01110";
        static const char glyph_v[] = "10001""10001""10001""10001""10001""01010""00100";
        static const char glyph_w[] = "10001""10001""10001""10101""10101""10101""01010";
        static const char glyph_x[] = "10001""10001""01010""00100""01010""10001""10001";
        static const char glyph_y[] = "10001""10001""01010""00100""00100""00100""00100";
        static const char glyph_z[] = "11111""00001""00010""00100""01000""10000""11111";

        if (character == ' ')
            return (glyph_space);
        if (character == '-')
            return (glyph_dash);
        if (character == ':')
            return (glyph_colon);
        if (character == '.')
            return (glyph_period);
        if (character >= '0' && character <= '9')
        {
            static const char *digits[] =
            {
                glyph_0, glyph_1, glyph_2, glyph_3, glyph_4,
                glyph_5, glyph_6, glyph_7, glyph_8, glyph_9
            };
            return (digits[character - '0']);
        }
        if (character >= 'A' && character <= 'Z')
        {
            static const char *letters[] =
            {
                glyph_a, glyph_b, glyph_c, glyph_d, glyph_e, glyph_f, glyph_g,
                glyph_h, glyph_i, glyph_j, glyph_k, glyph_l, glyph_m, glyph_n,
                glyph_o, glyph_p, glyph_q, glyph_r, glyph_s, glyph_t, glyph_u,
                glyph_v, glyph_w, glyph_x, glyph_y, glyph_z
            };
            return (letters[character - 'A']);
        }
        return (glyph_space);
    }

    static void draw_rectangle(MenuImage &image, int32_t x, int32_t y,
        int32_t width, int32_t height, uint32_t color)
    {
        int32_t row;
        int32_t column;

        row = 0;
        while (row < height)
        {
            column = 0;
            while (column < width)
            {
                if (x + column >= 0 && x + column < image.width
                    && y + row >= 0 && y + row < image.height)
                {
                    image.pixels[static_cast<ft_size_t>((y + row) * image.width
                        + (x + column))] = color;
                }
                column += 1;
            }
            row += 1;
        }
    }

    static void draw_glyph(MenuImage &image, int32_t x, int32_t y,
        char character, int32_t scale, uint32_t color)
    {
        const char *pattern;
        int32_t row;
        int32_t column;

        pattern = glyph_pattern(character);
        row = 0;
        while (row < 7)
        {
            column = 0;
            while (column < 5)
            {
                if (pattern[(row * 5) + column] == '1')
                {
                    draw_rectangle(image, x + (column * scale), y + (row * scale),
                        scale, scale, color);
                }
                column += 1;
            }
            row += 1;
        }
    }

    static void draw_text(MenuImage &image, int32_t x, int32_t y,
        const char *text, int32_t scale, uint32_t color)
    {
        ft_size_t index;
        ft_size_t length;

        if (text == nullptr)
            return ;
        length = std::strlen(text);
        index = 0U;
        while (index < length)
        {
            draw_glyph(image, x + static_cast<int32_t>(index) * (6 * scale), y,
                text[index], scale, color);
            index += 1U;
        }
    }

    static void draw_background(MenuImage &image)
    {
        int32_t y;
        int32_t x;
        uint32_t top_color;
        uint32_t bottom_color;
        uint32_t red;
        uint32_t green;
        uint32_t blue;
        uint32_t blend_top;
        uint32_t blend_bottom;

        y = 0;
        while (y < image.height)
        {
            blend_top = static_cast<uint32_t>(image.height - y);
            blend_bottom = static_cast<uint32_t>(y);
            top_color = make_color(16U, 18U, 34U);
            bottom_color = make_color(34U, 58U, 86U);
            red = ((((top_color >> 16) & 255U) * blend_top)
                + (((bottom_color >> 16) & 255U) * blend_bottom))
                / static_cast<uint32_t>(image.height);
            green = ((((top_color >> 8) & 255U) * blend_top)
                + (((bottom_color >> 8) & 255U) * blend_bottom))
                / static_cast<uint32_t>(image.height);
            blue = (((top_color & 255U) * blend_top)
                + ((bottom_color & 255U) * blend_bottom))
                / static_cast<uint32_t>(image.height);
            x = 0;
            while (x < image.width)
            {
                image.pixels[static_cast<ft_size_t>(y * image.width + x)]
                    = make_color(red, green, blue);
                x += 1;
            }
            y += 1;
        }
    }

    static void draw_menu(MenuImage &image, const LaunchSettings &settings,
        int32_t selected_row)
    {
        const uint32_t accent = make_color(255U, 198U, 92U);
        const uint32_t accent_soft = make_color(84U, 112U, 164U);
        const uint32_t text_primary = make_color(244U, 245U, 246U);
        const uint32_t text_secondary = make_color(194U, 206U, 224U);
        const uint32_t row_bg = make_color(22U, 26U, 40U);
        const uint32_t row_bg_selected = make_color(40U, 50U, 76U);
        int32_t row;
        int32_t row_y;
        char value_buffer[64];

        draw_background(image);
        draw_rectangle(image, 44, 34, image.width - 88, image.height - 68,
            make_color(12U, 15U, 24U));
        draw_rectangle(image, 56, 46, image.width - 112, image.height - 92,
            make_color(18U, 22U, 34U));
        draw_text(image, 84, 70, "VOXEL SETUP", 4, accent);
        draw_text(image, 90, 118, "FIRST LAUNCH CONFIGURATION", 2,
            text_secondary);
        draw_text(image, 90, 146,
            "ARROWS CHANGE VALUES  ENTER SAVES  ESC ACCEPTS CURRENT",
            2, text_secondary);

        row = 0;
        while (row < 3)
        {
            row_y = 214 + (row * 78);
            draw_rectangle(image, 84, row_y - 10, image.width - 168, 64,
                row == selected_row ? row_bg_selected : row_bg);
            draw_rectangle(image, 84, row_y - 10, 10, 64,
                row == selected_row ? accent : accent_soft);
            if (row == 0)
            {
                draw_text(image, 110, row_y, "DISPLAY", 3, text_primary);
                draw_text(image, 520, row_y,
                    settings.display_mode_label(), 3,
                    row == selected_row ? accent : text_secondary);
            }
            else if (row == 1)
            {
                draw_text(image, 110, row_y, "RESOLUTION", 3, text_primary);
                draw_text(image, 520, row_y,
                    settings.resolution_label(), 3,
                    row == selected_row ? accent : text_secondary);
            }
            else
            {
                draw_text(image, 110, row_y, "KEYBOARD", 3, text_primary);
                draw_text(image, 520, row_y,
                    settings.keyboard_layout_label(), 3,
                    row == selected_row ? accent : text_secondary);
            }
            row += 1;
        }

        std::snprintf(value_buffer, sizeof(value_buffer),
            "WINDOWED MODE USES %s", settings.resolution_label());
        draw_text(image, 88, image.height - 66, value_buffer, 2,
            text_secondary);
        draw_text(image, 88, image.height - 36, "ENTER TO SAVE AND LAUNCH", 2,
            text_secondary);
    }

    static void cycle_resolution(LaunchSettings &settings, int direction)
    {
        settings.resolution_preset = (settings.resolution_preset + direction + 3) % 3;
    }
}

LaunchSettingsMenu::LaunchSettingsMenu()
{
}

LaunchSettingsMenu::LaunchSettingsMenu(const LaunchSettingsMenu &other)
{
    *this = other;
}

LaunchSettingsMenu::~LaunchSettingsMenu()
{
}

LaunchSettingsMenu &LaunchSettingsMenu::operator=(const LaunchSettingsMenu &other)
{
    (void)other;
    return (*this);
}

int LaunchSettingsMenu::run(LaunchSettings &settings)
{
    ApplicationWindow render_window;
    GpuRenderer renderer;
    int32_t error_code;
    int32_t selected_row;
    bool should_exit;
    MenuImage image;
    std::vector<uint32_t> pixels;
    int32_t width;
    int32_t height;

    error_code = render_window.initialize(false, 960, 540, false, true);
    if (error_code != FT_ERR_SUCCESS)
        return (1);
    if (render_window.is_gpu_mode() == false)
    {
        (void)render_window.destroy();
        return (1);
    }
    width = render_window.get_gpu_window()->get_width();
    height = render_window.get_gpu_window()->get_height();
    if (renderer.initialize(width, height) == false)
    {
        (void)render_window.destroy();
        return (1);
    }
    pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
    selected_row = 0;
    should_exit = false;
    while (should_exit == false && render_window.should_close() == false)
    {
        render_window.poll_events();
        ft_dumb_controls_poll();
        if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_UP) == FT_TRUE)
        {
            selected_row -= 1;
            if (selected_row < 0)
                selected_row = 2;
        }
        if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_DOWN) == FT_TRUE)
        {
            selected_row += 1;
            if (selected_row > 2)
                selected_row = 0;
        }
        if (selected_row == 0)
        {
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_LEFT) == FT_TRUE
                || ft_dumb_control_was_pressed(FT_DUMB_CONTROL_RIGHT) == FT_TRUE)
            {
                settings.fullscreen = !settings.fullscreen;
            }
        }
        else if (selected_row == 1)
        {
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_LEFT) == FT_TRUE)
                cycle_resolution(settings, -1);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_RIGHT) == FT_TRUE)
                cycle_resolution(settings, 1);
        }
        else if (selected_row == 2)
        {
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_LEFT) == FT_TRUE
                || ft_dumb_control_was_pressed(FT_DUMB_CONTROL_RIGHT) == FT_TRUE)
            {
                if (settings.keyboard_layout == FT_DUMB_KEYBOARD_LAYOUT_QWERTY)
                    settings.keyboard_layout = FT_DUMB_KEYBOARD_LAYOUT_AZERTY;
                else
                    settings.keyboard_layout = FT_DUMB_KEYBOARD_LAYOUT_QWERTY;
            }
        }
        if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_CONFIRM) == FT_TRUE
            || ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK) == FT_TRUE)
        {
            should_exit = true;
        }
        image.width = width;
        image.height = height;
        image.pixels = pixels.data();
        draw_menu(image, settings, selected_row);
        renderer.upload_overlay(image.pixels, width, height);
        renderer.draw_overlay();
        error_code = render_window.present();
        if (error_code != FT_ERR_SUCCESS)
        {
            renderer.destroy();
            (void)render_window.destroy();
            return (1);
        }
        if (render_window.should_close() == true)
            should_exit = true;
    }
    renderer.destroy();
    (void)render_window.destroy();
    return (0);
}
