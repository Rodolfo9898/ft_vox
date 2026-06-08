#ifndef LAUNCH_SETTINGS_HPP
# define LAUNCH_SETTINGS_HPP

#include "../../Libft/Modules/DUMB/controls.hpp"
#include <cstdint>

class LaunchSettings
{
    public:
        bool fullscreen;
        int32_t resolution_preset;
        ft_dumb_keyboard_layout keyboard_layout;

        LaunchSettings();
        LaunchSettings(const LaunchSettings &other);
        ~LaunchSettings();
        LaunchSettings &operator=(const LaunchSettings &other);

        int load(const char *path);
        int save(const char *path) const;

        int32_t resolution_width() const;
        int32_t resolution_height() const;
        const char *resolution_label() const;
        const char *display_mode_label() const;
        const char *keyboard_layout_label() const;
};

#endif
