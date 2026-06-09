#ifndef SETTINGS_HPP
# define SETTINGS_HPP

#include "../../Libft/Modules/DUMB/controls.hpp"
#include <cstdint>
#include <functional>
#include <vector>
#include <algorithm>

class Settings
{
    public:
        static Settings &instance();

        bool fps_overlay() const;
        int  render_distance() const;
        ft_dumb_keyboard_layout keyboard_layout() const;

        void set_fps_overlay(bool v);
        void set_render_distance(int v);
        void set_keyboard_layout(ft_dumb_keyboard_layout v);

        using ChangeCallback = std::function<void()>;
        void add_observer(ChangeCallback cb);

    private:
        Settings();
        Settings(const Settings &other);
        ~Settings();
        Settings &operator=(const Settings &other);

        void notify();

        bool                      fps_overlay_      = true;
        int                       render_distance_  = 160;
        ft_dumb_keyboard_layout   layout_           = FT_DUMB_KEYBOARD_LAYOUT_QWERTY;
        std::vector<ChangeCallback> observers_;
};

#endif
