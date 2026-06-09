#ifndef MAINMENUSCENE_HPP
# define MAINMENUSCENE_HPP

#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"

class MainMenuScene : public MenuScene
{
    int selected_;
    public:
        MainMenuScene();
        MainMenuScene(const MainMenuScene &other);
        ~MainMenuScene() override;
        MainMenuScene &operator=(const MainMenuScene &other);
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

#endif
