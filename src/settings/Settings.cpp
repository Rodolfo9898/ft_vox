#include "../../src/settings/Settings.hpp"
#include <algorithm>

Settings &Settings::instance()
{
    static Settings s;
    return s;
}

Settings::Settings()
{
}

void Settings::set_fps_overlay(bool v)
{
    if (fps_overlay_ == v) return;
    fps_overlay_ = v;
    notify();
}

void Settings::set_render_distance(int v)
{
    int clamped = std::max(14, std::min(160, v));
    if (render_distance_ == clamped) return;
    render_distance_ = clamped;
    notify();
}

void Settings::set_keyboard_layout(ft_dumb_keyboard_layout v)
{
    if (layout_ == v) return;
    layout_ = v;
    ft_dumb_controls_set_keyboard_layout(v);
    notify();
}

void Settings::add_observer(ChangeCallback cb)
{
    observers_.push_back(cb);
}

void Settings::notify()
{
    for (auto &cb : observers_)
        cb();
}
