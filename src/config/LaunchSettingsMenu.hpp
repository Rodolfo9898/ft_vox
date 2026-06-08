#ifndef LAUNCH_SETTINGS_MENU_HPP
# define LAUNCH_SETTINGS_MENU_HPP

#include "../../src/config/LaunchSettings.hpp"
#include "../../src/platform/ApplicationWindow.hpp"

class LaunchSettingsMenu
{
    public:
        LaunchSettingsMenu();
        LaunchSettingsMenu(const LaunchSettingsMenu &other);
        ~LaunchSettingsMenu();
        LaunchSettingsMenu &operator=(const LaunchSettingsMenu &other);

        static int run(LaunchSettings &settings);
};

#endif
