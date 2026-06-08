#include "../../src/config/LaunchSettings.hpp"

#include "../../Libft/Modules/JSon/json.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace
{
    static const int32_t g_resolution_widths[] = {1280, 1920, 3840};
    static const int32_t g_resolution_heights[] = {720, 1080, 2160};
    static const char *g_resolution_labels[] =
    {
        "720P",
        "1080P",
        "4K WIDESCREEN"
    };

    static int32_t clamp_resolution_preset(int32_t preset)
    {
        if (preset < 0)
            return (0);
        if (preset >= 3)
            return (2);
        return (preset);
    }

    static ft_dumb_keyboard_layout clamp_keyboard_layout(int32_t layout)
    {
        if (layout == static_cast<int32_t>(FT_DUMB_KEYBOARD_LAYOUT_AZERTY))
            return (FT_DUMB_KEYBOARD_LAYOUT_AZERTY);
        return (FT_DUMB_KEYBOARD_LAYOUT_QWERTY);
    }
}

LaunchSettings::LaunchSettings()
{
    this->fullscreen = false;
    this->resolution_preset = 0;
    this->keyboard_layout = FT_DUMB_KEYBOARD_LAYOUT_AZERTY;
}

LaunchSettings::LaunchSettings(const LaunchSettings &other)
{
    *this = other;
}

LaunchSettings::~LaunchSettings()
{
}

LaunchSettings &LaunchSettings::operator=(const LaunchSettings &other)
{
    if (this != &other)
    {
        this->fullscreen = other.fullscreen;
        this->resolution_preset = other.resolution_preset;
        this->keyboard_layout = other.keyboard_layout;
    }
    return (*this);
}

int LaunchSettings::load(const char *path)
{
    json_group *groups;
    json_group *settings_group;
    json_item *item;
    char *end_pointer;
    long parsed_value;
    std::string content;
    std::FILE *file;
    long file_size_long;
    size_t file_size;
    size_t read_count;

    if (path == nullptr)
        return (1);
    file = std::fopen(path, "rb");
    if (file == nullptr)
        return (1);
    if (std::fseek(file, 0, SEEK_END) != 0)
    {
        std::fclose(file);
        return (1);
    }
    file_size_long = std::ftell(file);
    if (file_size_long < 0)
    {
        std::fclose(file);
        return (1);
    }
    if (std::fseek(file, 0, SEEK_SET) != 0)
    {
        std::fclose(file);
        return (1);
    }
    file_size = static_cast<size_t>(file_size_long);
    content.resize(file_size);
    if (file_size > 0U)
    {
        read_count = std::fread(&content[0], 1, file_size, file);
        if (read_count != file_size)
        {
            std::fclose(file);
            return (1);
        }
    }
    std::fclose(file);
    content.push_back('\0');
    groups = json_read_from_string(content.c_str());
    if (groups == nullptr)
        return (1);
    settings_group = json_find_group(groups, "settings");
    if (settings_group == nullptr)
    {
        json_free_groups(groups);
        return (1);
    }
    item = json_find_item(settings_group, "fullscreen");
    if (item != nullptr && item->value != nullptr)
        this->fullscreen = (std::strcmp(item->value, "true") == 0);
    item = json_find_item(settings_group, "resolution_preset");
    if (item != nullptr && item->value != nullptr)
    {
        parsed_value = std::strtol(item->value, &end_pointer, 10);
        if (end_pointer != item->value && *end_pointer == '\0')
            this->resolution_preset = static_cast<int32_t>(parsed_value);
    }
    item = json_find_item(settings_group, "keyboard_layout");
    if (item != nullptr && item->value != nullptr)
    {
        if (std::strcmp(item->value, "azerty") == 0)
            this->keyboard_layout = FT_DUMB_KEYBOARD_LAYOUT_AZERTY;
        else
            this->keyboard_layout = FT_DUMB_KEYBOARD_LAYOUT_QWERTY;
    }
    this->resolution_preset = clamp_resolution_preset(this->resolution_preset);
    this->keyboard_layout = clamp_keyboard_layout(
        static_cast<int32_t>(this->keyboard_layout));
    json_free_groups(groups);
    return (0);
}

int LaunchSettings::save(const char *path) const
{
    json_group *groups;
    json_item *fullscreen_item;
    json_item *resolution_item;
    json_item *keyboard_item;
    int32_t error_code;

    if (path == nullptr)
        return (1);
    groups = json_create_json_group("settings");
    if (groups == nullptr)
        return (1);
    fullscreen_item = json_create_item("fullscreen",
        this->fullscreen == true ? FT_TRUE : FT_FALSE);
    resolution_item = json_create_item("resolution_preset",
        clamp_resolution_preset(this->resolution_preset));
    keyboard_item = json_create_item("keyboard_layout",
        this->keyboard_layout == FT_DUMB_KEYBOARD_LAYOUT_AZERTY
            ? "azerty" : "qwerty");
    if (fullscreen_item == nullptr || resolution_item == nullptr
        || keyboard_item == nullptr)
    {
        if (fullscreen_item != nullptr)
            json_free_items(fullscreen_item);
        if (resolution_item != nullptr)
            json_free_items(resolution_item);
        if (keyboard_item != nullptr)
            json_free_items(keyboard_item);
        json_free_groups(groups);
        return (1);
    }
    json_add_item_to_group(groups, fullscreen_item);
    json_add_item_to_group(groups, resolution_item);
    json_add_item_to_group(groups, keyboard_item);
    error_code = json_write_to_file(path, groups);
    json_free_groups(groups);
    return (error_code == 0 ? 0 : 1);
}

int32_t LaunchSettings::resolution_width() const
{
    return (g_resolution_widths[clamp_resolution_preset(this->resolution_preset)]);
}

int32_t LaunchSettings::resolution_height() const
{
    return (g_resolution_heights[clamp_resolution_preset(this->resolution_preset)]);
}

const char *LaunchSettings::resolution_label() const
{
    return (g_resolution_labels[clamp_resolution_preset(this->resolution_preset)]);
}

const char *LaunchSettings::display_mode_label() const
{
    if (this->fullscreen == true)
        return ("FULLSCREEN");
    return ("WINDOWED");
}

const char *LaunchSettings::keyboard_layout_label() const
{
    if (this->keyboard_layout == FT_DUMB_KEYBOARD_LAYOUT_AZERTY)
        return ("AZERTY");
    return ("QWERTY");
}
