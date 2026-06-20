#include "../../src/config/LaunchSettingsMenu.hpp"

LaunchSettingsMenu::LaunchSettingsMenu()
{
}
LaunchSettingsMenu::LaunchSettingsMenu(const LaunchSettingsMenu &other)
{
    (void)other;
}
LaunchSettingsMenu::~LaunchSettingsMenu()
{
}
LaunchSettingsMenu &LaunchSettingsMenu::operator=(const LaunchSettingsMenu &other)
{
    (void)other;
    return *this;
}

// clang-format off
// Each entry is 7 rows × 5 columns = 35 chars (0=off, 1=on)
const char LaunchSettingsMenu::GLYPHS[42][36] = {
    "01110" "10001" "10011" "10101" "11001" "10001" "01110", // '0'
    "00100" "01100" "00100" "00100" "00100" "00100" "01110", // '1'
    "01110" "10001" "00001" "00010" "00100" "01000" "11111", // '2'
    "11110" "00001" "00001" "01110" "00001" "00001" "11110", // '3'
    "00010" "00110" "01010" "10010" "11111" "00010" "00010", // '4'
    "11111" "10000" "10000" "11110" "00001" "00001" "11110", // '5'
    "01110" "10000" "10000" "11110" "10001" "10001" "01110", // '6'
    "11111" "00001" "00010" "00100" "01000" "01000" "01000", // '7'
    "01110" "10001" "10001" "01110" "10001" "10001" "01110", // '8'
    "01110" "10001" "10001" "01111" "00001" "00001" "01110", // '9'
    "01110" "10001" "10001" "11111" "10001" "10001" "10001", // 'A'
    "11110" "10001" "10001" "11110" "10001" "10001" "11110", // 'B'
    "01110" "10001" "10000" "10000" "10000" "10001" "01110", // 'C'
    "11110" "10001" "10001" "10001" "10001" "10001" "11110", // 'D'
    "11111" "10000" "10000" "11110" "10000" "10000" "11111", // 'E'
    "11111" "10000" "10000" "11110" "10000" "10000" "10000", // 'F'
    "01110" "10001" "10000" "10111" "10001" "10001" "01110", // 'G'
    "10001" "10001" "10001" "11111" "10001" "10001" "10001", // 'H'
    "01110" "00100" "00100" "00100" "00100" "00100" "01110", // 'I'
    "00001" "00001" "00001" "00001" "10001" "10001" "01110", // 'J'
    "10001" "10010" "10100" "11000" "10100" "10010" "10001", // 'K'
    "10000" "10000" "10000" "10000" "10000" "10000" "11111", // 'L'
    "10001" "11011" "10101" "10101" "10001" "10001" "10001", // 'M'
    "10001" "11001" "10101" "10011" "10001" "10001" "10001", // 'N'
    "01110" "10001" "10001" "10001" "10001" "10001" "01110", // 'O'
    "11110" "10001" "10001" "11110" "10000" "10000" "10000", // 'P'
    "01110" "10001" "10001" "10001" "10101" "10010" "01101", // 'Q'
    "11110" "10001" "10001" "11110" "10100" "10010" "10001", // 'R'
    "01111" "10000" "10000" "01110" "00001" "00001" "11110", // 'S'
    "11111" "00100" "00100" "00100" "00100" "00100" "00100", // 'T'
    "10001" "10001" "10001" "10001" "10001" "10001" "01110", // 'U'
    "10001" "10001" "10001" "10001" "10001" "01010" "00100", // 'V'
    "10001" "10001" "10001" "10101" "10101" "10101" "01010", // 'W'
    "10001" "10001" "01010" "00100" "01010" "10001" "10001", // 'X'
    "10001" "10001" "01010" "00100" "00100" "00100" "00100", // 'Y'
    "11111" "00001" "00010" "00100" "01000" "10000" "11111", // 'Z'
    "00000" "00000" "00000" "00000" "00000" "00000" "00000", // ' '
    "00000" "00000" "00000" "11111" "00000" "00000" "00000", // '-'
    "00000" "00100" "00100" "00000" "00100" "00100" "00000", // ':'
    "00000" "00000" "00000" "00000" "00000" "00110" "00110", // '.'
    "00000" "00000" "00000" "00000" "00000" "00000" "00000", // fallback
    "00000" "00000" "00000" "00000" "00000" "00000" "00000", // unused
};
// clang-format on

const char *LaunchSettingsMenu::glyph_pattern(char ch)
{
    if (ch == ' ')
        return GLYPHS[36];
    if (ch == '-')
        return GLYPHS[37];
    if (ch == ':')
        return GLYPHS[38];
    if (ch == '.')
        return GLYPHS[39];
    if (ch >= '0' && ch <= '9')
        return GLYPHS[ch - '0'];
    if (ch >= 'A' && ch <= 'Z')
        return GLYPHS[10 + (ch - 'A')];
    return GLYPHS[36];
}

uint32_t LaunchSettingsMenu::make_color(uint32_t r, uint32_t g, uint32_t b)
{
    return (r << 16U) | (g << 8U) | b;
}

void LaunchSettingsMenu::draw_rectangle(Image &img, int32_t x, int32_t y, int32_t w, int32_t h,
                                        uint32_t color)
{
    for (int32_t row = 0; row < h; ++row)
        for (int32_t col = 0; col < w; ++col)
            if (x + col >= 0 && x + col < img.width && y + row >= 0 && y + row < img.height)
                img.pixels[static_cast<size_t>((y + row) * img.width + (x + col))] = color;
}

void LaunchSettingsMenu::draw_glyph(Image &img, int32_t x, int32_t y, char ch, int32_t scale,
                                    uint32_t color)
{
    const char *pat = glyph_pattern(ch);
    for (int32_t row = 0; row < 7; ++row)
        for (int32_t col = 0; col < 5; ++col)
            if (pat[row * 5 + col] == '1')
                draw_rectangle(img, x + col * scale, y + row * scale, scale, scale, color);
}

void LaunchSettingsMenu::draw_text(Image &img, int32_t x, int32_t y, const char *text,
                                   int32_t scale, uint32_t color)
{
    if (!text)
        return;
    for (size_t i = 0; text[i]; ++i)
        draw_glyph(img, x + static_cast<int32_t>(i) * 6 * scale, y, text[i], scale, color);
}

void LaunchSettingsMenu::draw_background(Image &img)
{
    const uint32_t top = make_color(16U, 18U, 34U);
    const uint32_t bot = make_color(34U, 58U, 86U);
    for (int32_t y = 0; y < img.height; ++y)
    {
        uint32_t bt = static_cast<uint32_t>(img.height - y);
        uint32_t bb = static_cast<uint32_t>(y);
        uint32_t h = static_cast<uint32_t>(img.height);
        uint32_t r = (((top >> 16) & 255U) * bt + ((bot >> 16) & 255U) * bb) / h;
        uint32_t g = (((top >> 8) & 255U) * bt + ((bot >> 8) & 255U) * bb) / h;
        uint32_t b = ((top & 255U) * bt + (bot & 255U) * bb) / h;
        for (int32_t x = 0; x < img.width; ++x)
            img.pixels[static_cast<size_t>(y * img.width + x)] = make_color(r, g, b);
    }
}

void LaunchSettingsMenu::draw_row(Image &img, const LaunchSettings &settings, int32_t row,
                                  int32_t row_y, int32_t selected_row)
{
    const uint32_t accent = make_color(255U, 198U, 92U);
    const uint32_t accent_soft = make_color(84U, 112U, 164U);
    const uint32_t text_primary = make_color(244U, 245U, 246U);
    const uint32_t text_secondary = make_color(194U, 206U, 224U);
    const uint32_t row_bg = make_color(22U, 26U, 40U);
    const uint32_t row_bg_sel = make_color(40U, 50U, 76U);

    draw_rectangle(img, 84, row_y - 10, img.width - 168, 64,
                   row == selected_row ? row_bg_sel : row_bg);
    draw_rectangle(img, 84, row_y - 10, 10, 64, row == selected_row ? accent : accent_soft);

    const char *label = row == 0 ? "DISPLAY" : (row == 1 ? "RESOLUTION" : "KEYBOARD");
    const char *value =
        row == 0 ? settings.display_mode_label()
                 : (row == 1 ? settings.resolution_label() : settings.keyboard_layout_label());
    draw_text(img, 110, row_y, label, 3, text_primary);
    draw_text(img, 520, row_y, value, 3, row == selected_row ? accent : text_secondary);
}

void LaunchSettingsMenu::draw_menu(Image &img, const LaunchSettings &settings, int32_t selected_row)
{
    const uint32_t accent = make_color(255U, 198U, 92U);
    const uint32_t text_secondary = make_color(194U, 206U, 224U);
    char value_buf[64];

    draw_background(img);
    draw_rectangle(img, 44, 34, img.width - 88, img.height - 68, make_color(12U, 15U, 24U));
    draw_rectangle(img, 56, 46, img.width - 112, img.height - 92, make_color(18U, 22U, 34U));
    draw_text(img, 84, 70, "VOXEL SETUP", 4, accent);
    draw_text(img, 90, 118, "FIRST LAUNCH CONFIGURATION", 2, text_secondary);
    draw_text(img, 90, 146, "ARROWS CHANGE VALUES  ENTER SAVES  ESC ACCEPTS CURRENT", 2,
              text_secondary);
    for (int32_t row = 0; row < 3; ++row)
        draw_row(img, settings, row, 214 + row * 78, selected_row);
    std::snprintf(value_buf, sizeof(value_buf), "WINDOWED MODE USES %s",
                  settings.resolution_label());
    draw_text(img, 88, img.height - 66, value_buf, 2, text_secondary);
    draw_text(img, 88, img.height - 36, "ENTER TO SAVE AND LAUNCH", 2, text_secondary);
}

int LaunchSettingsMenu::init_renderer(ApplicationWindow &window, GpuRenderer &renderer,
                                      int32_t &width, int32_t &height)
{
    if (window.initialize(false, 960, 540, false, true) != FT_ERR_SUCCESS)
        return 1;
    if (!window.is_gpu_mode())
    {
        (void)window.destroy();
        return 1;
    }
    width = window.get_gpu_window()->get_width();
    height = window.get_gpu_window()->get_height();
    if (!renderer.initialize(width, height))
    {
        (void)window.destroy();
        return 1;
    }
    return 0;
}

void LaunchSettingsMenu::handle_input(LaunchSettings &settings, int32_t &selected_row,
                                      bool &should_exit)
{
    if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_UP) == FT_TRUE)
        selected_row = (selected_row == 0) ? 2 : selected_row - 1;
    if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_DOWN) == FT_TRUE)
        selected_row = (selected_row == 2) ? 0 : selected_row + 1;

    bool left = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_LEFT) == FT_TRUE;
    bool right = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_RIGHT) == FT_TRUE;
    if (selected_row == 0 && (left || right))
        settings.fullscreen = !settings.fullscreen;
    if (selected_row == 1)
    {
        if (left)
            settings.resolution_preset = (settings.resolution_preset + 2) % 3;
        if (right)
            settings.resolution_preset = (settings.resolution_preset + 1) % 3;
    }
    if (selected_row == 2 && (left || right))
        settings.keyboard_layout = (settings.keyboard_layout == FT_DUMB_KEYBOARD_LAYOUT_QWERTY)
                                       ? FT_DUMB_KEYBOARD_LAYOUT_AZERTY
                                       : FT_DUMB_KEYBOARD_LAYOUT_QWERTY;
    if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_CONFIRM) == FT_TRUE ||
        ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK) == FT_TRUE)
        should_exit = true;
}

int LaunchSettingsMenu::render_frame(ApplicationWindow &window, GpuRenderer &renderer, Image &img,
                                     const LaunchSettings &settings, int32_t selected_row,
                                     std::vector<uint32_t> &pixels)
{
    img.pixels = pixels.data();
    draw_menu(img, settings, selected_row);
    renderer.upload_overlay(img.pixels, img.width, img.height);
    renderer.draw_overlay();
    if (window.present() != FT_ERR_SUCCESS)
    {
        renderer.destroy();
        (void)window.destroy();
        return 1;
    }
    return 0;
}

int LaunchSettingsMenu::run_loop(ApplicationWindow &window, GpuRenderer &renderer,
                                 LaunchSettings &settings, int32_t width, int32_t height)
{
    std::vector<uint32_t> pixels(static_cast<size_t>(width) * static_cast<size_t>(height));
    Image img = {width, height, pixels.data()};
    int32_t selected_row = 0;
    bool should_exit = false;

    while (!should_exit && !window.should_close())
    {
        window.poll_events();
        ft_dumb_controls_poll();
        handle_input(settings, selected_row, should_exit);
        should_exit = should_exit || window.should_close();
        if (render_frame(window, renderer, img, settings, selected_row, pixels) != 0)
            return 1;
    }
    renderer.destroy();
    (void)window.destroy();
    return 0;
}

int LaunchSettingsMenu::run(LaunchSettings &settings)
{
    ApplicationWindow window;
    GpuRenderer renderer;
    int32_t width = 0;
    int32_t height = 0;

    if (init_renderer(window, renderer, width, height) != 0)
        return 1;
    return run_loop(window, renderer, settings, width, height);
}
