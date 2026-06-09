#include "../../src/menu/MenuCanvas.hpp"

MenuCanvas::MenuCanvas(const MenuCanvas &other)
{
    *this = other;
}

MenuCanvas::~MenuCanvas()
{
}

MenuCanvas &MenuCanvas::operator=(const MenuCanvas &other)
{
    if (this != &other)
    {
        this->pixels_ = other.pixels_;
        this->fb_ = other.fb_;
        this->fb_.pixels = this->pixels_.data();
    }
    return (*this);
}


static const uint8_t s_glyphs[39][5] = {
    { 7U, 5U, 5U, 5U, 7U }, { 2U, 6U, 2U, 2U, 7U }, { 7U, 1U, 7U, 4U, 7U },
    { 7U, 1U, 7U, 1U, 7U }, { 5U, 5U, 7U, 1U, 1U }, { 7U, 4U, 7U, 1U, 7U },
    { 7U, 4U, 7U, 5U, 7U }, { 7U, 1U, 1U, 1U, 1U }, { 7U, 5U, 7U, 5U, 7U },
    { 7U, 5U, 7U, 1U, 7U }, { 7U, 5U, 7U, 5U, 5U }, { 6U, 5U, 6U, 5U, 6U },
    { 7U, 4U, 4U, 4U, 7U }, { 6U, 5U, 5U, 5U, 6U }, { 7U, 4U, 6U, 4U, 7U },
    { 7U, 4U, 6U, 4U, 4U }, { 7U, 4U, 5U, 5U, 7U }, { 5U, 5U, 7U, 5U, 5U },
    { 7U, 2U, 2U, 2U, 7U }, { 1U, 1U, 1U, 5U, 7U }, { 5U, 5U, 6U, 5U, 5U },
    { 4U, 4U, 4U, 4U, 7U }, { 5U, 7U, 7U, 5U, 5U }, { 5U, 7U, 7U, 7U, 5U },
    { 7U, 5U, 5U, 5U, 7U }, { 6U, 5U, 6U, 4U, 4U }, { 7U, 5U, 5U, 7U, 1U },
    { 6U, 5U, 6U, 5U, 5U }, { 7U, 4U, 7U, 1U, 7U }, { 7U, 2U, 2U, 2U, 2U },
    { 5U, 5U, 5U, 5U, 7U }, { 5U, 5U, 5U, 5U, 2U }, { 5U, 5U, 7U, 7U, 5U },
    { 5U, 5U, 2U, 5U, 5U }, { 5U, 5U, 2U, 2U, 2U }, { 7U, 1U, 2U, 4U, 7U },
    { 0U, 0U, 0U, 0U, 2U },
    { 0U, 0U, 7U, 0U, 0U },
    { 0U, 0U, 0U, 0U, 7U }
};

static const uint8_t *glyph_for(char c)
{
    if (c >= '0' && c <= '9') return s_glyphs[c - '0'];
    if (c >= 'A' && c <= 'Z') return s_glyphs[10 + (c - 'A')];
    if (c >= 'a' && c <= 'z') return s_glyphs[10 + (c - 'a')];
    if (c == '.') return s_glyphs[36];
    if (c == '-') return s_glyphs[37];
    if (c == '_') return s_glyphs[38];
    return nullptr;
}

MenuCanvas::MenuCanvas() : pixels_(static_cast<size_t>(W * H), MC::BG)
{
    fb_.width  = W;
    fb_.height = H;
    fb_.pixels = pixels_.data();
}

void MenuCanvas::set_pixel(int x, int y, uint32_t color)
{
    if (x >= 0 && x < W && y >= 0 && y < H)
        pixels_[static_cast<size_t>(y * W + x)] = color;
}

void MenuCanvas::clear(uint32_t color)
{
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void MenuCanvas::fill_rect(int x, int y, int w, int h, uint32_t color)
{
    int x1 = std::max(x, 0);
    int y1 = std::max(y, 0);
    int x2 = std::min(x + w, W);
    int y2 = std::min(y + h, H);
    for (int row = y1; row < y2; ++row)
    {
        uint32_t *line = pixels_.data() + row * W;
        for (int col = x1; col < x2; ++col)
            line[col] = color;
    }
}

void MenuCanvas::draw_border(int x, int y, int w, int h, uint32_t color, int thickness)
{
    fill_rect(x,                 y,                 w,         thickness, color);
    fill_rect(x,                 y + h - thickness, w,         thickness, color);
    fill_rect(x,                 y,                 thickness, h,         color);
    fill_rect(x + w - thickness, y,                 thickness, h,         color);
}

void MenuCanvas::draw_text(int x, int y, const char *text, int scale, uint32_t color)
{
    if (!text) return;
    int cx = x;
    for (int i = 0; text[i]; ++i)
    {
        if (text[i] == ' ') { cx += 5 * scale; continue; }
        if (text[i] == ':') { cx += 4 * scale; continue; }
        const uint8_t *bm = glyph_for(text[i]);
        if (!bm) { cx += 5 * scale; continue; }
        for (int row = 0; row < 5; ++row)
            for (int col = 0; col < 3; ++col)
                if (bm[row] & (1U << (2 - col)))
                    for (int sy = 0; sy < scale; ++sy)
                        for (int sx = 0; sx < scale; ++sx)
                            set_pixel(cx + col * scale + sx,
                                      y + row * scale + sy, color);
        cx += 4 * scale;
    }
}

void MenuCanvas::draw_text_centered(int cx, int y, const char *text, int scale, uint32_t color)
{
    if (!text) return;
    int len = 0;
    for (const char *p = text; *p; ++p) ++len;
    int tw = len * 4 * scale;
    draw_text(cx - tw / 2, y, text, scale, color);
}

bool MenuCanvas::is_hovered(int x, int y, int w, int h, const MenuInput &in) const
{
    return in.mouse_x >= x && in.mouse_x < x + w
        && in.mouse_y >= y && in.mouse_y < y + h;
}

bool MenuCanvas::was_clicked(int x, int y, int w, int h, const MenuInput &in) const
{
    return in.mouse_clicked && is_hovered(x, y, w, h, in);
}

bool MenuCanvas::button(int x, int y, int w, int h,
    const char *label, int scale,
    const MenuInput &in, bool selected)
{
    bool hov  = is_hovered(x, y, w, h, in);
    uint32_t bg = selected ? MC::BTN_A : (hov ? MC::BTN_H : MC::BTN_N);
    fill_rect(x, y, w, h, bg);
    draw_border(x, y, w, h, hov ? MC::ACCENT : MC::TEXT_DIM);
    int tw = 0;
    for (const char *p = label; *p; ++p) tw += 4 * scale;
    int tx = x + (w - tw) / 2;
    int ty = y + (h - 5 * scale) / 2;
    draw_text(tx, ty, label, scale, MC::TEXT);
    return was_clicked(x, y, w, h, in);
}

bool MenuCanvas::checkbox(int x, int y, bool checked,
    const char *label, int scale,
    const MenuInput &in)
{
    int box = scale * 6;
    fill_rect(x, y, box, box, checked ? MC::GREEN : MC::PANEL);
    draw_border(x, y, box, box, checked ? MC::GREEN : MC::TEXT_DIM);
    if (checked)
        draw_text(x + scale, y + scale, "X", scale, MC::WHITE);
    draw_text(x + box + scale * 2, y + scale, label, scale, MC::TEXT);
    int click_w = box + scale * 2 + static_cast<int>(std::strlen(label)) * 4 * scale;
    if (was_clicked(x, y, click_w, box, in))
        return !checked;
    return checked;
}

float MenuCanvas::slider(int x, int y, int w,
    float value, float vmin, float vmax,
    const char *label, int scale,
    const MenuInput &in)
{
    int th  = scale * 3;
    int ty  = y + scale * 2;
    fill_rect(x, ty, w, th, MC::PANEL);
    draw_border(x, ty, w, th, MC::TEXT_DIM);
    float frac = (value - vmin) / (vmax - vmin);
    if (frac < 0.0f) frac = 0.0f;
    if (frac > 1.0f) frac = 1.0f;
    int fw = static_cast<int>(frac * static_cast<float>(w));
    fill_rect(x, ty, fw, th, MC::SLIDER);
    int kx = x + fw - scale;
    fill_rect(kx, ty - scale, scale * 2, th + scale * 2, MC::ACCENT);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%s  %d", label, static_cast<int>(value));
    draw_text(x, ty + th + scale * 2, buf, scale, MC::TEXT);
    if (is_hovered(x, ty, w, th + scale * 4, in) && in.mouse_clicked)
    {
        float nf = static_cast<float>(in.mouse_x - x) / static_cast<float>(w);
        nf = std::max(0.0f, std::min(1.0f, nf));
        return vmin + nf * (vmax - vmin);
    }
    return value;
}

const uint32_t *MenuCanvas::pixels() const { return pixels_.data(); }
int MenuCanvas::width()  const { return W; }
int MenuCanvas::height() const { return H; }
ft_render_framebuffer &MenuCanvas::framebuffer() { return fb_; }
