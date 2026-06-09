#ifndef LOADINGSCENE_HPP
# define LOADINGSCENE_HPP

#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"

class LoadingScene : public MenuScene
{
    int tick_;
    public:
        LoadingScene();
        LoadingScene(const LoadingScene &other);
        ~LoadingScene() override;
        LoadingScene &operator=(const LoadingScene &other);
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
        void on_enter() override;
};

#endif
