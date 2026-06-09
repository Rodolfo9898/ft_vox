#ifndef MENU_SCENE_HPP
# define MENU_SCENE_HPP

#include "../../src/menu/MenuCanvas.hpp"
#include <string>

class MenuController;

class MenuScene
{
    public:
        virtual ~MenuScene() = default;
        virtual void on_enter() {}
        virtual void on_exit()  {}
        virtual void update(const MenuInput &in, MenuController &ctl) = 0;
        virtual void render(MenuCanvas &c) const = 0;
};

class MainMenuScene : public MenuScene
{
    int selected_ = 0;
    public:
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

class SettingsScene : public MenuScene
{
    public:
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

class CreditsScene : public MenuScene
{
    public:
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

class LoadingScene : public MenuScene
{
    int tick_ = 0;
    public:
        void on_enter() override { tick_ = 0; }
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

class InGameSettingsScene : public MenuScene
{
    public:
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

#endif
