#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"

void CreditsScene::update(const MenuInput &in, MenuController &ctl)
{
    if (in.key_escape || ctl.canvas().was_clicked(
            (MenuCanvas::W - 200) / 2, MenuCanvas::H - 70, 200, 36, in))
        ctl.request_back();
}

void CreditsScene::render(MenuCanvas &c) const
{
    c.clear(MC::BG);
    c.draw_text_centered(MenuCanvas::W / 2, 50, "CREDITS", 4, MC::TITLE);
    c.fill_rect(60, 115, MenuCanvas::W - 120, 1, MC::TEXT_DIM);
    c.draw_text_centered(MenuCanvas::W / 2, 145, "MADE BY", 3, MC::TEXT_DIM);
    c.draw_text_centered(MenuCanvas::W / 2, 200, "RPEREZ-T", 4, MC::ACCENT);
    c.draw_text_centered(MenuCanvas::W / 2, 260, "AND", 2, MC::TEXT_DIM);
    c.draw_text_centered(MenuCanvas::W / 2, 295, "BVANGENE", 4, MC::ACCENT);
    c.fill_rect(60, 365, MenuCanvas::W - 120, 1, MC::TEXT_DIM);
    int bx = (MenuCanvas::W - 200) / 2;
    c.fill_rect(bx, MenuCanvas::H - 70, 200, 36, MC::BTN_N);
    c.draw_border(bx, MenuCanvas::H - 70, 200, 36, MC::TEXT_DIM);
    c.draw_text(bx + 56, MenuCanvas::H - 58, "BACK", 3, MC::TEXT);
}
