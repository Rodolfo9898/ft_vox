#include "../../src/menu/InGameSettingsScene.hpp"

InGameSettingsScene::InGameSettingsScene()
    : MenuScene() {}
InGameSettingsScene::InGameSettingsScene(const InGameSettingsScene &other)
    : MenuScene() { (void)other; }
InGameSettingsScene::~InGameSettingsScene() {}
InGameSettingsScene &InGameSettingsScene::operator=(const InGameSettingsScene &other) { (void)other; return *this; }


static const int IG_W = 400, IG_H = 320;
static const int IG_X = (MenuCanvas::W - IG_W) / 2;
static const int IG_Y = (MenuCanvas::H - IG_H) / 2;

void InGameSettingsScene::update(const MenuCanvas::MenuInput &in, MenuController &ctl)
{
    Settings &s = Settings::instance();
    int togx = IG_X + IG_W - 80, togy = IG_Y + 50;
    int slx = IG_X + 20, sly = IG_Y + 110;
    int slw = IG_W - 40;
    int qbx = IG_X + 20, qby = IG_Y + 185;
    int abx = IG_X + 140, aby = IG_Y + 185;
    int backbx = IG_X + 20,         backby = IG_Y + IG_H - 50;
    int menubx  = IG_X + IG_W / 2,   menuby = IG_Y + IG_H - 50;

    if (ctl.canvas().was_clicked(togx, togy, 60, 20, in))
        s.set_fps_overlay(!s.fps_overlay());
    if (ctl.canvas().is_hovered(slx, sly, slw, 16, in) && in.mouse_clicked)
    {
        float f = static_cast<float>(in.mouse_x - slx) / static_cast<float>(slw);
        if (f < 0.0f) f = 0.0f;
		if (f > 1.0f) f = 1.0f;
        s.set_render_distance(14 + static_cast<int>(f * static_cast<float>(160 - 14)));
    }
    if (ctl.canvas().was_clicked(qbx, qby, 110, 28, in))
        s.set_keyboard_layout(FT_DUMB_KEYBOARD_LAYOUT_QWERTY);
    if (ctl.canvas().was_clicked(abx, aby, 110, 28, in))
        s.set_keyboard_layout(FT_DUMB_KEYBOARD_LAYOUT_AZERTY);
    if (ctl.canvas().was_clicked(backbx, backby, IG_W / 2 - 30, 30, in)
        || in.key_escape)
        ctl.request_dismiss_in_game();
    if (ctl.canvas().was_clicked(menubx, menuby, IG_W / 2 - 10, 30, in))
        ctl.request_main_menu_from_game();
}

void InGameSettingsScene::render(MenuCanvas &c) const
{
    Settings &s = Settings::instance();
    c.fill_rect(IG_X - 3, IG_Y - 3, IG_W + 6, IG_H + 6, MenuCanvas::Colors::ACCENT);
    c.fill_rect(IG_X, IG_Y, IG_W, IG_H, MenuCanvas::Colors::PANEL);
    c.draw_text_centered(IG_X + IG_W / 2, IG_Y + 8, "SETTINGS", 3, MenuCanvas::Colors::TITLE);
    c.fill_rect(IG_X + 10, IG_Y + 40, IG_W - 20, 1, MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(IG_X + 20, IG_Y + 52, "FPS OVERLAY", 2, MenuCanvas::Colors::TEXT);
    int tx = IG_X + IG_W - 80, ty = IG_Y + 50;
    bool fps = s.fps_overlay();
    c.fill_rect(tx, ty, 60, 20, fps ? MenuCanvas::Colors::GREEN : MenuCanvas::Colors::BTN_N);
    c.draw_border(tx, ty, 60, 20, fps ? MenuCanvas::Colors::GREEN : MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(tx + 12, ty + 4, fps ? "ON" : "OFF", 2, MenuCanvas::Colors::TEXT);
    c.draw_text(IG_X + 20, IG_Y + 92, "RENDER DISTANCE", 2, MenuCanvas::Colors::TEXT);
    int slx = IG_X + 20, sly = IG_Y + 110, slw = IG_W - 40;
    int rd = s.render_distance();
    c.fill_rect(slx, sly, slw, 14, MenuCanvas::Colors::BTN_N);
    c.draw_border(slx, sly, slw, 14, MenuCanvas::Colors::TEXT_DIM);
    float f = static_cast<float>(rd - 14) / static_cast<float>(160 - 14);
    c.fill_rect(slx, sly, static_cast<int>(f * static_cast<float>(slw)), 14, MenuCanvas::Colors::SLIDER);
    char rb[32]; std::snprintf(rb, sizeof(rb), "%d", rd);
    c.draw_text(slx + slw + 8, sly + 2, rb, 2, MenuCanvas::Colors::TEXT);
    c.draw_text(IG_X + 20, IG_Y + 164, "KEYBOARD", 2, MenuCanvas::Colors::TEXT);
    ft_dumb_keyboard_layout layout = s.keyboard_layout();
    bool qsel = (layout == FT_DUMB_KEYBOARD_LAYOUT_QWERTY);
    int qbx = IG_X + 20, qby = IG_Y + 185;
    int abx = IG_X + 140, aby = IG_Y + 185;
    c.fill_rect(qbx, qby, 110, 28, qsel ? MenuCanvas::Colors::BTN_A : MenuCanvas::Colors::BTN_N);
    c.draw_border(qbx, qby, 110, 28, qsel ? MenuCanvas::Colors::ACCENT : MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(qbx + 14, qby + 7, "QWERTY", 2, MenuCanvas::Colors::TEXT);
    c.fill_rect(abx, aby, 110, 28, !qsel ? MenuCanvas::Colors::BTN_A : MenuCanvas::Colors::BTN_N);
    c.draw_border(abx, aby, 110, 28, !qsel ? MenuCanvas::Colors::ACCENT : MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(abx + 14, aby + 7, "AZERTY", 2, MenuCanvas::Colors::TEXT);
    c.fill_rect(IG_X + 20, IG_Y + IG_H - 50, IG_W / 2 - 30, 30, MenuCanvas::Colors::BTN_N);
    c.draw_border(IG_X + 20, IG_Y + IG_H - 50, IG_W / 2 - 30, 30, MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(IG_X + 28, IG_Y + IG_H - 43, "RESUME", 2, MenuCanvas::Colors::TEXT);
    c.fill_rect(IG_X + IG_W / 2, IG_Y + IG_H - 50, IG_W / 2 - 10, 30, MenuCanvas::Colors::BTN_N);
    c.draw_border(IG_X + IG_W / 2, IG_Y + IG_H - 50, IG_W / 2 - 10, 30, MenuCanvas::Colors::TEXT_DIM);
    c.draw_text(IG_X + IG_W / 2 + 8, IG_Y + IG_H - 43, "MAIN MENU", 2, MenuCanvas::Colors::TEXT);
}
