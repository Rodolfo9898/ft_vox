#ifndef CREDITSSCENE_HPP
# define CREDITSSCENE_HPP

#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"

class CreditsScene : public MenuScene
{
    public:
        CreditsScene();
        CreditsScene(const CreditsScene &other);
        ~CreditsScene() override;
        CreditsScene &operator=(const CreditsScene &other);
        void update(const MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

#endif
