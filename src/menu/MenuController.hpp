#ifndef MENU_CONTROLLER_HPP
# define MENU_CONTROLLER_HPP

#include "../../src/menu/MenuCanvas.hpp"
#include "../../src/menu/MenuScene.hpp"
#include <string>
#include <memory>

class GpuRenderer;

enum class MenuPhase
{
    MAIN_MENU,
    SETTINGS,
    CREDITS,
    LOADING,
    IN_GAME_SETTINGS
};

class MenuController
{
    public:
        MenuController();

        void show_main_menu();
        void show_settings();
        void show_credits();
        void show_loading();
        void show_in_game_settings();

        void update(const MenuInput &in);
        void render(GpuRenderer *gpu);
        void render_overlay(GpuRenderer *gpu);

        bool wants_start()       const { return wants_start_; }
        bool wants_exit()        const { return wants_exit_; }
        bool dismissed_in_game() const { return dismissed_in_game_; }
        bool wants_main_menu()   const { return wants_main_menu_; }

        const std::string &start_seed() const { return start_seed_; }
        void clear_flags();

        void request_start(const std::string &seed);
        void request_exit();
        void request_settings();
        void request_credits();
        void request_back();
        void request_dismiss_in_game();
        void request_main_menu_from_game();

        MenuCanvas &canvas() { return canvas_; }

    private:
        MenuCanvas    canvas_;
        MenuPhase     phase_    = MenuPhase::MAIN_MENU;

        std::unique_ptr<MainMenuScene>        main_scene_;
        std::unique_ptr<SettingsScene>        settings_scene_;
        std::unique_ptr<CreditsScene>         credits_scene_;
        std::unique_ptr<LoadingScene>         loading_scene_;
        std::unique_ptr<InGameSettingsScene>  in_game_scene_;

        bool        wants_start_       = false;
        bool        wants_exit_        = false;
        bool        dismissed_in_game_ = false;
        bool        wants_main_menu_   = false;
        std::string start_seed_;

        MenuScene *current_scene();
};

#endif
