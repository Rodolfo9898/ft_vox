#ifndef INGAMESETTINGSSCENE_HPP
# define INGAMESETTINGSSCENE_HPP

#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"
#include "../../src/settings/Settings.hpp"

class InGameSettingsScene : public MenuScene
{
    public:
        InGameSettingsScene();
        InGameSettingsScene(const InGameSettingsScene &other);
        ~InGameSettingsScene() override;
        InGameSettingsScene &operator=(const InGameSettingsScene &other);
        void update(const MenuCanvas::MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

#endif
