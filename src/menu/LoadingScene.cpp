#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"

static const char *DOTS[4] = { ".", "..", "...", "...." };

void LoadingScene::update(const MenuInput &in, MenuController &ctl)
{
    (void)in; (void)ctl;
    ++tick_;
}

void LoadingScene::render(MenuCanvas &c) const
{
    c.clear(MC::BG);
    c.draw_text_centered(MenuCanvas::W / 2, MenuCanvas::H / 2 - 60,
        "GENERATING WORLD", 4, MC::TITLE);
    const char *dots = DOTS[(tick_ / 12) % 4];
    c.draw_text_centered(MenuCanvas::W / 2, MenuCanvas::H / 2 + 10,
        dots, 4, MC::ACCENT);
    c.draw_text_centered(MenuCanvas::W / 2, MenuCanvas::H / 2 + 70,
        "PLEASE WAIT", 2, MC::TEXT_DIM);
    int bar_w = MenuCanvas::W - 160;
    int bar_x = 80;
    int bar_y = MenuCanvas::H / 2 + 110;
    c.fill_rect(bar_x, bar_y, bar_w, 10, MC::PANEL);
    c.draw_border(bar_x, bar_y, bar_w, 10, MC::TEXT_DIM);
    int fill = (tick_ % 60) * bar_w / 60;
    c.fill_rect(bar_x, bar_y, fill, 10, MC::SLIDER);
}
