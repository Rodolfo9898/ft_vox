#ifndef SETTINGSSCENE_HPP
# define SETTINGSSCENE_HPP

#include "../../src/menu/MenuScene.hpp"
#include "../../src/menu/MenuController.hpp"
#include "../../src/settings/Settings.hpp"

class SettingsScene : public MenuScene
{
    public:
        SettingsScene();
        SettingsScene(const SettingsScene &other);
        ~SettingsScene() override;
        SettingsScene &operator=(const SettingsScene &other);
        void update(const MenuCanvas::MenuInput &in, MenuController &ctl) override;
        void render(MenuCanvas &c) const override;
};

#endif
