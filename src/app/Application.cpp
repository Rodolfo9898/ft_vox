#include "../../src/app/Application.hpp"

Application::Application() {}
Application::Application(const Application &other) { (void)other; }
Application::~Application() {}
Application &Application::operator=(const Application &other)
{ (void)other; return *this; }

namespace
{
    enum class AppPhase { MAIN_MENU, LOADING, IN_GAME, IN_GAME_SETTINGS };

    static ft_dumb_keyboard_layout detect_system_layout()
    {
#if defined(__APPLE__)
        CFStringRef id_ref = nullptr;
        TISInputSourceRef src = TISCopyCurrentKeyboardLayoutInputSource();
        if (src)
        {
            id_ref = static_cast<CFStringRef>(
                TISGetInputSourceProperty(src, kTISPropertyInputSourceID));
        }
        bool is_azerty = false;
        if (id_ref)
        {
            char buf[128] = {};
            CFStringGetCString(id_ref, buf, sizeof(buf), kCFStringEncodingUTF8);
            is_azerty = (std::strstr(buf, "French") != nullptr
                      || std::strstr(buf, "AZERTY") != nullptr
                      || std::strstr(buf, "azerty") != nullptr);
        }
        if (src) CFRelease(src);
        if (is_azerty) return FT_DUMB_KEYBOARD_LAYOUT_AZERTY;
#elif !defined(_WIN32)
        const char *lang = std::getenv("LANG");
        if (!lang) lang = std::getenv("LC_ALL");
        if (!lang) lang = std::getenv("LC_MESSAGES");
        if (lang && std::strncmp(lang, "fr_", 3) == 0)
            return FT_DUMB_KEYBOARD_LAYOUT_AZERTY;
#endif
        return FT_DUMB_KEYBOARD_LAYOUT_QWERTY;
    }

    static void wait_for_escape_release()
    {
#if defined(_WIN32)
        while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
            Sleep(16);
#endif
    }

    static void clear_pending_quit_messages()
    {
#if defined(_WIN32)
        MSG msg;
        while (PeekMessageA(&msg, nullptr, WM_QUIT, WM_QUIT, PM_REMOVE) != 0) {}
#endif
    }

    static MenuCanvas::MenuInput collect_menu_input(ApplicationWindow &window)
    {
        MenuCanvas::MenuInput in = {};
        int gw = (window.is_gpu_mode() && window.get_gpu_window())
            ? window.get_gpu_window()->get_width()  : MenuCanvas::W;
        int gh = (window.is_gpu_mode() && window.get_gpu_window())
            ? window.get_gpu_window()->get_height() : MenuCanvas::H;
        in.mouse_x      = window.mouse_x() * MenuCanvas::W / (gw > 0 ? gw : 1);
        in.mouse_y      = window.mouse_y() * MenuCanvas::H / (gh > 0 ? gh : 1);
        in.mouse_clicked= window.was_mouse_clicked();
        in.key_up    = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_UP)      == FT_TRUE;
        in.key_down  = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_DOWN)    == FT_TRUE;
        in.key_enter = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_CONFIRM) == FT_TRUE;
        in.key_escape= ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK)    == FT_TRUE;
        return in;
    }
}

int Application::run(int argc, char **argv)
{
    ApplicationOptions          options;
    LaunchSettings              launch_settings;
    ApplicationWindow           window;
    VoxelRenderer               renderer;
    const RenderDistanceStrategy *strategy;

    if (DebugCrashHandler::enable() != 0) return 1;
    if (options.parse(argc, argv) != 0)   return 1;
    if (options.validate_camera_speed_mode)    return ApplicationValidator::validate_camera_speed();
    if (options.validate_collision_mode)       return ApplicationValidator::validate_collision();
    if (options.validate_block_edit_mode)      return ApplicationValidator::validate_block_edit();
    if (options.validate_visible_distance_mode)return ApplicationValidator::validate_visible_distance();
    if (options.validate_terrain_determinism_mode)
        return ApplicationValidator::validate_terrain_determinism();
    if (options.validate_world_scale_mode)     return ApplicationValidator::validate_world_scale();
    if (options.validate_caves_mode)           return ApplicationValidator::validate_caves();

    const char *cfg = "ft_vox_config.json";
    if (!options.perf_test_mode && !options.perf_headless_mode)
    {
        if (launch_settings.load(cfg) != 0)
        {
            if (LaunchSettingsMenu::run(launch_settings) != 0) return 1;
            if (launch_settings.save(cfg) != 0)
                std::fprintf(stderr,
                    "Application: unable to save settings to %s\n", cfg);
            wait_for_escape_release();
            clear_pending_quit_messages();
        }
    }
    if (launch_settings.keyboard_layout == FT_DUMB_KEYBOARD_LAYOUT_QWERTY)
    {
        ft_dumb_keyboard_layout detected = detect_system_layout();
        if (detected != FT_DUMB_KEYBOARD_LAYOUT_QWERTY)
            launch_settings.keyboard_layout = detected;
    }
    ft_dumb_controls_set_keyboard_layout(launch_settings.keyboard_layout);
    Settings::instance().set_keyboard_layout(launch_settings.keyboard_layout);
    strategy = &RenderStrategyFactory::select(options.perf_headless_mode);

    if (options.perf_headless_mode || options.perf_test_mode)
    {
        PerfSession perf;
        return perf.run(options, launch_settings, *strategy);
    }

    if (window.initialize(false, 0, 0, true, true) != 0) return 1;
    if (window.is_gpu_mode())
        renderer.initialize_gpu(window.get_gpu_window()->get_width(),
            window.get_gpu_window()->get_height());
    renderer.preload_assets();
    window.set_cursor_visible(true);

    MenuController menu_ctl;
    GameSession    game_session;
    menu_ctl.show_main_menu();

    AppPhase phase         = AppPhase::MAIN_MENU;
    int      loading_frames = 0;
    auto     prev_time     = std::chrono::steady_clock::now();

    while (!window.should_close() && !menu_ctl.wants_exit())
    {
        auto   now = std::chrono::steady_clock::now();
        double dt  = std::min(
            std::chrono::duration<double>(now - prev_time).count(), 0.1);
        prev_time  = now;

        window.poll_events();
        if (window.should_close()) break;

        if (phase != AppPhase::IN_GAME)
        {
            ft_dumb_controls_set_mouse_captured(FT_FALSE);
            ft_dumb_controls_poll();
            ft_dumb_controls_set_mouse_captured(FT_TRUE);
        }

        if (phase == AppPhase::MAIN_MENU)
        {
            MenuCanvas::MenuInput in = collect_menu_input(window);
            menu_ctl.update(in);
            if (menu_ctl.wants_start())
            {
                if (game_session.start(menu_ctl.start_seed(), window, renderer) == FT_ERR_SUCCESS)
                {
                    menu_ctl.show_loading();
                    menu_ctl.clear_flags();
                    loading_frames = 0;
                    phase = AppPhase::LOADING;
                }
            }
        }
        else if (phase == AppPhase::LOADING)
        {
            MenuCanvas::MenuInput in = collect_menu_input(window);
            menu_ctl.update(in);
            if (game_session.loading_tick(*strategy) != FT_ERR_SUCCESS)
            {
                game_session.stop();
                menu_ctl.show_main_menu();
                phase = AppPhase::MAIN_MENU;
            }
            else if (game_session.is_ready_to_play() && ++loading_frames >= 30)
            {
                phase = AppPhase::IN_GAME;
                window.set_cursor_visible(false);
            }
        }
        else if (phase == AppPhase::IN_GAME)
        {
            GameSession::Action action = game_session.update(dt, *strategy, window);
            if (action == GameSession::Action::OPEN_SETTINGS)
            {
                menu_ctl.show_in_game_settings();
                phase = AppPhase::IN_GAME_SETTINGS;
                window.set_cursor_visible(true);
            }
            else if (action == GameSession::Action::EXIT_TO_MENU || action == GameSession::Action::ERROR)
            {
                game_session.stop();
                menu_ctl.show_main_menu();
                phase = AppPhase::MAIN_MENU;
                window.set_cursor_visible(true);
                if (action == GameSession::Action::ERROR) break;
            }
        }
        else if (phase == AppPhase::IN_GAME_SETTINGS)
        {
            MenuCanvas::MenuInput in = collect_menu_input(window);
            menu_ctl.update(in);
            ft_dumb_controls_set_keyboard_layout(Settings::instance().keyboard_layout());
            if (menu_ctl.dismissed_in_game())
            {
                menu_ctl.clear_flags();
                phase = AppPhase::IN_GAME;
                window.set_cursor_visible(false);
            }
            else if (menu_ctl.wants_main_menu())
            {
                game_session.stop();
                menu_ctl.show_main_menu();
                menu_ctl.clear_flags();
                phase = AppPhase::MAIN_MENU;
                window.set_cursor_visible(true);
            }
        }

        if (window.is_gpu_mode())
        {
            GpuRenderer *gpu = renderer.get_gpu_renderer();
            if (phase == AppPhase::MAIN_MENU || phase == AppPhase::LOADING)
            {
                if (gpu) menu_ctl.render(gpu);
            }
            else if (phase == AppPhase::IN_GAME)
                game_session.render(window, renderer, true);
            else if (phase == AppPhase::IN_GAME_SETTINGS)
            {
                game_session.render(window, renderer, false);
                if (gpu) menu_ctl.render_overlay(gpu);
            }
        }
        window.present();
    }
    if (game_session.is_active()) game_session.stop();
    window.set_cursor_visible(true);
    window.destroy();
    return 0;
}
