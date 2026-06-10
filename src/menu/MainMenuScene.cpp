#include "../../src/menu/MainMenuScene.hpp"

MainMenuScene::MainMenuScene()
    : MenuScene() {}
MainMenuScene::MainMenuScene(const MainMenuScene &other)
    : MenuScene() { (void)other; }
MainMenuScene::~MainMenuScene() {}
MainMenuScene &MainMenuScene::operator=(const MainMenuScene &other) { (void)other; return *this; }


static const int BTN_W = 280;
static const int BTN_H = 44;
static const int BTN_X = (MenuCanvas::W - BTN_W) / 2;

static const char *BTN_LABELS[4] = {"START", "SETTINGS", "CREDITS", "EXIT"};

void MainMenuScene::update(const MenuCanvas::MenuInput &in, MenuController &ctl)
{
    if (in.key_up)   { --selected_; if (selected_ < 0) selected_ = 3; }
    if (in.key_down) { ++selected_; if (selected_ > 3) selected_ = 0; }
    for (int i = 0; i < 4; ++i)
    {
        int by = 210 + i * (BTN_H + 12);
        bool hov = ctl.canvas().is_hovered(BTN_X, by, BTN_W, BTN_H, in);
        if (hov) selected_ = i;
    }
    if (in.key_enter)
    {
        switch (selected_)
        {
            case 0: ctl.request_start(""); break;
            case 1: ctl.request_settings(); break;
            case 2: ctl.request_credits(); break;
            case 3: ctl.request_exit(); break;
            default: break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        int by = 210 + i * (BTN_H + 12);
        MenuCanvas::MenuInput tmp = in;
        tmp.mouse_clicked = in.mouse_clicked &&
            ctl.canvas().is_hovered(BTN_X, by, BTN_W, BTN_H, in);
        if (tmp.mouse_clicked)
        {
            switch (i)
            {
                case 0: ctl.request_start(""); break;
                case 1: ctl.request_settings(); break;
                case 2: ctl.request_credits(); break;
                case 3: ctl.request_exit(); break;
                default: break;
            }
        }
    }
}

void MainMenuScene::render(MenuCanvas &c) const
{
    c.clear(MenuCanvas::Colors::BG);
    c.draw_text_centered(MenuCanvas::W / 2, 80, "FT_VOX", 6, MenuCanvas::Colors::TITLE);
    for (int i = 0; i < 4; ++i)
    {
        int by = 210 + i * (BTN_H + 12);
        bool sel = (i == selected_);
        uint32_t bg = sel ? MenuCanvas::Colors::BTN_A : MenuCanvas::Colors::BTN_N;
        c.fill_rect(BTN_X, by, BTN_W, BTN_H, bg);
        c.draw_border(BTN_X, by, BTN_W, BTN_H, sel ? MenuCanvas::Colors::ACCENT : MenuCanvas::Colors::TEXT_DIM);
        int tw = 0;
        for (const char *p = BTN_LABELS[i]; *p; ++p) tw += 16;
        c.draw_text(BTN_X + (BTN_W - tw) / 2, by + (BTN_H - 20) / 2,
            BTN_LABELS[i], 3, MenuCanvas::Colors::TEXT);
    }
    c.draw_text_centered(MenuCanvas::W / 2, MenuCanvas::H - 24,
        "USE ARROWS OR MOUSE TO NAVIGATE  ENTER TO SELECT", 1, MenuCanvas::Colors::TEXT_DIM);
}
