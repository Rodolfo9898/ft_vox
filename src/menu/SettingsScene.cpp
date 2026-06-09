#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"
#include "../../src/settings/Settings.hpp"

static const int STOG_X = MenuCanvas::W - 120;
static const int STOG_Y = 110;
static const int SSLIDER_X = 60;
static const int SSLIDER_Y = 150 + 24;
static const int SSLIDER_W = MenuCanvas::W - 120;
static const int SQ_X = 60,  SQ_Y = 274 + 28;
static const int SA_X = 180, SA_Y = 274 + 28;
static const int SBACK_X = 40, SBACK_Y = MenuCanvas::H - 60;

void SettingsScene::update(const MenuInput &in, MenuController &ctl)
{
    if (in.key_escape)
    { ctl.request_back(); return; }
    Settings &s = Settings::instance();
    if (ctl.canvas().was_clicked(STOG_X, STOG_Y, 60, 20, in))
        s.set_fps_overlay(!s.fps_overlay());
    if (ctl.canvas().is_hovered(SSLIDER_X, SSLIDER_Y, SSLIDER_W, 20, in) && in.mouse_clicked)
    {
        float frac = static_cast<float>(in.mouse_x - SSLIDER_X)
            / static_cast<float>(SSLIDER_W);
        if (frac < 0.0f) frac = 0.0f;
        if (frac > 1.0f) frac = 1.0f;
        int rd = 14 + static_cast<int>(frac * static_cast<float>(160 - 14));
        s.set_render_distance(rd);
    }
    if (ctl.canvas().was_clicked(SQ_X, SQ_Y, 110, 32, in))
        s.set_keyboard_layout(FT_DUMB_KEYBOARD_LAYOUT_QWERTY);
    if (ctl.canvas().was_clicked(SA_X, SA_Y, 110, 32, in))
        s.set_keyboard_layout(FT_DUMB_KEYBOARD_LAYOUT_AZERTY);
    if (ctl.canvas().was_clicked(SBACK_X, SBACK_Y, 200, 36, in))
        ctl.request_back();
}

void SettingsScene::render(MenuCanvas &c) const
{
    Settings &s = Settings::instance();
    c.clear(MC::BG);
    c.draw_text_centered(MenuCanvas::W / 2, 30, "SETTINGS", 4, MC::TITLE);
    c.fill_rect(40, 88, MenuCanvas::W - 80, 1, MC::TEXT_DIM);
    c.draw_text(60, STOG_Y - 4, "FPS OVERLAY", 2, MC::TEXT);
    bool fps = s.fps_overlay();
    c.fill_rect(STOG_X, STOG_Y, 60, 20, fps ? MC::GREEN : MC::PANEL);
    c.draw_border(STOG_X, STOG_Y, 60, 20, fps ? MC::GREEN : MC::TEXT_DIM);
    c.draw_text(STOG_X + 12, STOG_Y + 4, fps ? "ON" : "OFF", 2, MC::TEXT);
    int y2 = 150;
    c.draw_text(60, y2, "RENDER DISTANCE", 2, MC::TEXT);
    c.fill_rect(SSLIDER_X, SSLIDER_Y, SSLIDER_W, 12, MC::PANEL);
    c.draw_border(SSLIDER_X, SSLIDER_Y, SSLIDER_W, 12, MC::TEXT_DIM);
    int rd = s.render_distance();
    float frac = static_cast<float>(rd - 14) / static_cast<float>(160 - 14);
    int fw = static_cast<int>(frac * static_cast<float>(SSLIDER_W));
    c.fill_rect(SSLIDER_X, SSLIDER_Y, fw, 12, MC::SLIDER);
    c.fill_rect(SSLIDER_X + fw - 4, SSLIDER_Y - 4, 8, 20, MC::ACCENT);
    char rbuf[32]; std::snprintf(rbuf, sizeof(rbuf), "%d CHUNKS", rd);
    c.draw_text(SSLIDER_X, SSLIDER_Y + 20, rbuf, 2, MC::TEXT);
    int y3 = 274;
    c.draw_text(60, y3, "KEYBOARD LAYOUT", 2, MC::TEXT);
    ft_dumb_keyboard_layout layout = s.keyboard_layout();
    bool qsel = (layout == FT_DUMB_KEYBOARD_LAYOUT_QWERTY);
    c.fill_rect(SQ_X, SQ_Y, 110, 32, qsel ? MC::BTN_A : MC::BTN_N);
    c.draw_border(SQ_X, SQ_Y, 110, 32, qsel ? MC::ACCENT : MC::TEXT_DIM);
    c.draw_text(SQ_X + 14, SQ_Y + 8, "QWERTY", 2, MC::TEXT);
    c.fill_rect(SA_X, SA_Y, 110, 32, !qsel ? MC::BTN_A : MC::BTN_N);
    c.draw_border(SA_X, SA_Y, 110, 32, !qsel ? MC::ACCENT : MC::TEXT_DIM);
    c.draw_text(SA_X + 14, SA_Y + 8, "AZERTY", 2, MC::TEXT);
    c.fill_rect(SBACK_X, SBACK_Y, 200, 36, MC::BTN_N);
    c.draw_border(SBACK_X, SBACK_Y, 200, 36, MC::TEXT_DIM);
    c.draw_text(SBACK_X + 56, SBACK_Y + 8, "BACK", 3, MC::TEXT);
}
