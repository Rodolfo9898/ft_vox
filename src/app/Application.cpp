#include "../../src/app/Application.hpp"
#include "../../src/settings/Settings.hpp"
#include "../../src/menu/MenuController.hpp"

#if defined(_WIN32)
# include <windows.h>
#endif
#if defined(__APPLE__)
# include <mach/task.h>
# include <mach/mach.h>
#endif
#include <ctime>

namespace
{
    static void wait_for_escape_release_after_launch_menu()
    {
#if defined(_WIN32)
        while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
            Sleep(16);
#endif
    }

    static void clear_pending_quit_messages_after_launch_menu()
    {
#if defined(_WIN32)
        MSG msg;
        while (PeekMessageA(&msg, nullptr, WM_QUIT, WM_QUIT, PM_REMOVE) != 0)
        {
        }
#endif
    }

    static uint32_t system_ram_mb()
    {
#if defined(__APPLE__)
        struct task_basic_info info;
        mach_msg_type_number_t size = TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), TASK_BASIC_INFO,
                reinterpret_cast<task_info_t>(&info), &size) == KERN_SUCCESS)
            return static_cast<uint32_t>(info.resident_size / (1024UL * 1024UL));
#elif !defined(_WIN32)
        FILE *f = std::fopen("/proc/self/status", "r");
        if (f)
        {
            char line[256];
            while (std::fgets(line, sizeof(line), f))
            {
                if (std::strncmp(line, "VmRSS:", 6) == 0)
                {
                    uint32_t kb = 0U;
                    std::sscanf(line + 6, "%u", &kb);
                    std::fclose(f);
                    return kb / 1024U;
                }
            }
            std::fclose(f);
        }
#endif
        return 0U;
    }

    enum class AppPhase { MAIN_MENU, LOADING, IN_GAME, IN_GAME_SETTINGS };
}

Application::Application() {}
Application::Application(const Application &other) { *this = other; }
Application::~Application() {}
Application &Application::operator=(const Application &other)
{
    (void)other; return (*this);
}

int Application::run(int argc, char **argv)
{
    ApplicationOptions  options;
    LaunchSettings      launch_settings;
    ApplicationWindow   window;
    Camera              camera;
    World               world;
    VoxelRenderer       renderer;
    RenderDebug         render_debug;
    std::chrono::steady_clock::time_point previous_time;
    std::chrono::steady_clock::time_point current_time;
    std::chrono::steady_clock::time_point perf_start_time;
    std::chrono::duration<double> elapsed_time;
    std::chrono::duration<double> perf_elapsed_time;
    double   delta_seconds;
    double   perf_fps;
    double   perf_frame_ms;
    double   fps_accumulator;
    double   render_distance_accumulator;
    double   display_fps;
    double   frame_ms;
    uint64_t perf_frame_hash;
    uint64_t frame_count;
    uint64_t fps_frame_count;
    int32_t  error_code;
    int32_t  active_render_distance;
    int32_t  generation_budget;
    const RenderDistanceStrategy *render_strategy;
    bool     boost_enabled;
    uint32_t selected_block_id;

    if (DebugCrashHandler::enable() != 0)
        return (1);
    if (options.parse(argc, argv) != 0)
        return (1);
    if (options.validate_camera_speed_mode == true)
        return (ApplicationValidator::validate_camera_speed());
    if (options.validate_collision_mode == true)
        return (ApplicationValidator::validate_collision());
    if (options.validate_block_edit_mode == true)
        return (ApplicationValidator::validate_block_edit());
    if (options.validate_visible_distance_mode == true)
        return (ApplicationValidator::validate_visible_distance());
    if (options.validate_terrain_determinism_mode == true)
        return (ApplicationValidator::validate_terrain_determinism());
    if (options.validate_world_scale_mode == true)
        return (ApplicationValidator::validate_world_scale());
    if (options.validate_caves_mode == true)
        return (ApplicationValidator::validate_caves());

    const char *launch_settings_path    = "ft_vox_config.json";
    bool use_launch_settings_menu =
        (options.perf_test_mode == false && options.perf_headless_mode == false);
    if (use_launch_settings_menu == true)
    {
        if (launch_settings.load(launch_settings_path) != 0)
        {
            if (LaunchSettingsMenu::run(launch_settings) != 0)
                return (1);
            if (launch_settings.save(launch_settings_path) != 0)
                std::fprintf(stderr,
                    "Application: unable to save launch settings to %s\n",
                    launch_settings_path);
            wait_for_escape_release_after_launch_menu();
            clear_pending_quit_messages_after_launch_menu();
        }
    }
    ft_dumb_controls_set_keyboard_layout(launch_settings.keyboard_layout);
    Settings::instance().set_keyboard_layout(launch_settings.keyboard_layout);

    render_strategy = &RenderStrategyFactory::select(options.perf_headless_mode);

    if (options.perf_headless_mode == true
        || options.perf_test_mode == true)
    {
        error_code = world.initialize("integration-seed");
        if (error_code != FT_ERR_SUCCESS)
            return ApplicationError::fail("voxel world initialization", error_code);
        if (window.initialize(options.perf_headless_mode,
                launch_settings.resolution_width(),
                launch_settings.resolution_height(),
                launch_settings.fullscreen, true) != 0)
        {
            world.destroy();
            return (1);
        }
        camera.initialize();
        PlayerController::spawn_player_on_ground(&camera, world);
        if (window.is_gpu_mode())
            renderer.initialize_gpu(window.get_gpu_window()->get_width(),
                window.get_gpu_window()->get_height());
        renderer.preload_assets();
        previous_time   = std::chrono::steady_clock::now();
        perf_start_time = previous_time;
        frame_count     = 0; fps_frame_count  = 0;
        fps_accumulator = 0.0; render_distance_accumulator = 0.0;
        display_fps     = 0.0; frame_ms = 0.0;
        active_render_distance = WORLD_REQUIRED_VISIBLE_DISTANCE;
        boost_enabled   = false;
        selected_block_id = TERRAIN_GENERATOR_DIRT_BLOCK;
        perf_frame_hash = UINT64_C(0);
        error_code      = FT_ERR_SUCCESS;
        while (options.perf_headless_mode == true || window.should_close() == false)
        {
            CameraInput camera_input;
            current_time  = std::chrono::steady_clock::now();
            elapsed_time  = current_time - previous_time;
            previous_time = current_time;
            delta_seconds = elapsed_time.count();
            if (delta_seconds > 0.1) delta_seconds = 0.1;
            camera_input = InputReader::empty_camera_input();
            if (options.perf_move_mode == true)
            {
                camera_input.move_forward = true;
                camera_input.speed_boost = options.perf_boost_mode;
            }
            if (options.perf_headless_mode == false)
            {
                window.poll_events();
                if (window.should_close()) break;
                ft_dumb_controls_poll();
                if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK) == FT_TRUE
                    && options.perf_test_mode == false)
                    break;
                if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BOOST) == FT_TRUE)
                    boost_enabled = !boost_enabled;
                camera_input = InputReader::read_camera_input(boost_enabled);
            }
            PlayerController::update_player_vertical_motion(&camera, camera_input,
                world, delta_seconds);
            PlayerController::update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            generation_budget = render_strategy->generation_budget_for_frame(
                frame_ms, boost_enabled || options.perf_boost_mode);
            error_code = world.update_around(camera.x, camera.z,
                generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS) break;
            if (options.perf_headless_mode == false)
            {
                if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_PRIMARY))
                { error_code = BlockInteractor::try_delete_target_block(&world, camera); if (error_code) break; }
                if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_TERTIARY))
                { error_code = BlockInteractor::try_pick_target_block(&world, camera, &selected_block_id); if (error_code) break; }
                if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_SECONDARY))
                { error_code = BlockInteractor::try_place_selected_block(&world, camera, selected_block_id); if (error_code) break; }
            }
            frame_ms = delta_seconds * 1000.0;
            render_distance_accumulator += delta_seconds;
            if (render_strategy->should_update_render_distance(render_distance_accumulator))
            {
                active_render_distance = render_strategy->update_render_distance(
                    active_render_distance, frame_ms,
                    boost_enabled || options.perf_boost_mode);
                render_distance_accumulator = 0.0;
            }
            fps_accumulator += delta_seconds;
            fps_frame_count += 1U;
            if (fps_accumulator >= 0.25)
            {
                display_fps      = static_cast<double>(fps_frame_count) / fps_accumulator;
                fps_accumulator  = 0.0; fps_frame_count = 0;
            }
            renderer.render_world(window.framebuffer(), camera, world,
                static_cast<const RenderDebug *>(nullptr));
            if (options.perf_headless_mode == false)
            {
                window.present();
                (void)display_fps;
            }
            frame_count += 1;
            if (options.perf_test_mode == true)
            {
                perf_frame_hash = FramebufferHasher::hash_framebuffer(window.framebuffer());
                current_time    = std::chrono::steady_clock::now();
                perf_elapsed_time = current_time - perf_start_time;
                if (perf_elapsed_time.count() >= options.perf_seconds_limit)
                    break;
            }
        }
        if (options.perf_test_mode == true)
        {
            perf_elapsed_time = std::chrono::steady_clock::now() - perf_start_time;
            if (perf_elapsed_time.count() > 0.0)
            {
                perf_fps    = static_cast<double>(frame_count) / perf_elapsed_time.count();
                perf_frame_ms = (perf_elapsed_time.count() * 1000.0)
                    / static_cast<double>(frame_count == 0 ? 1 : frame_count);
                std::printf("perf-test: frames=%llu elapsed=%.3f sec fps=%.2f frame_ms=%.3f hash=%016llx\n",
                    static_cast<unsigned long long>(frame_count),
                    perf_elapsed_time.count(), perf_fps, perf_frame_ms,
                    static_cast<unsigned long long>(perf_frame_hash));
            }
            else
            {
                std::printf("perf-test: frames=%llu elapsed=0.000 sec fps=0.00 frame_ms=0.000 hash=%016llx\n",
                    static_cast<unsigned long long>(frame_count),
                    static_cast<unsigned long long>(perf_frame_hash));
            }
        }
        world.destroy();
        if (options.perf_headless_mode == false)
            window.destroy();
        if (error_code != FT_ERR_SUCCESS)
            return ApplicationError::fail("main loop", error_code);
        return (0);
    }

    if (window.initialize(false, 0, 0, true, true) != 0)
        return (1);
    if (window.is_gpu_mode())
        renderer.initialize_gpu(window.get_gpu_window()->get_width(),
            window.get_gpu_window()->get_height());
    renderer.preload_assets();
    window.set_cursor_visible(true);

    MenuController menu_ctl;
    menu_ctl.show_main_menu();

    AppPhase   phase            = AppPhase::MAIN_MENU;
    bool       world_ready      = false;
    int32_t    loading_frames   = 0;
    selected_block_id           = TERRAIN_GENERATOR_DIRT_BLOCK;
    boost_enabled               = false;
    frame_ms                    = 0.0;
    display_fps                 = 0.0;
    fps_accumulator             = 0.0;
    fps_frame_count             = 0U;
    render_distance_accumulator = 0.0;
    frame_count                 = 0U;
    error_code                  = FT_ERR_SUCCESS;
    active_render_distance      = Settings::instance().render_distance();
    previous_time               = std::chrono::steady_clock::now();

    while (window.should_close() == false && !menu_ctl.wants_exit())
    {
        current_time  = std::chrono::steady_clock::now();
        elapsed_time  = current_time - previous_time;
        previous_time = current_time;
        delta_seconds = elapsed_time.count();
        if (delta_seconds > 0.1) delta_seconds = 0.1;

        window.poll_events();
        if (window.should_close()) break;

        MenuInput min_in = {};
        if (phase != AppPhase::IN_GAME)
        {
            ft_dumb_controls_set_mouse_captured(FT_FALSE);
            ft_dumb_controls_poll();
            ft_dumb_controls_set_mouse_captured(FT_TRUE);
            int gw = (window.is_gpu_mode() && window.get_gpu_window())
                ? window.get_gpu_window()->get_width()  : MenuCanvas::W;
            int gh = (window.is_gpu_mode() && window.get_gpu_window())
                ? window.get_gpu_window()->get_height() : MenuCanvas::H;
            min_in.mouse_x = window.mouse_x() * MenuCanvas::W / (gw > 0 ? gw : 1);
            min_in.mouse_y = window.mouse_y() * MenuCanvas::H / (gh > 0 ? gh : 1);
            min_in.mouse_clicked = window.was_mouse_clicked();
            min_in.key_up    = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_UP)      == FT_TRUE;
            min_in.key_down  = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_DOWN)    == FT_TRUE;
            min_in.key_enter = ft_dumb_control_was_pressed(FT_DUMB_CONTROL_CONFIRM) == FT_TRUE;
            min_in.key_escape= ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK)   == FT_TRUE;
        }

        if (phase == AppPhase::MAIN_MENU)
        {
            menu_ctl.update(min_in);
            if (menu_ctl.wants_exit()) break;
            if (menu_ctl.wants_start())
            {
                std::string seed = menu_ctl.start_seed();
                error_code = world.initialize(seed.c_str());
                if (error_code != FT_ERR_SUCCESS)
                {
                    ApplicationError::fail("world initialization", error_code);
                    break;
                }
                camera.initialize();
                PlayerController::spawn_player_on_ground(&camera, world);
                active_render_distance = Settings::instance().render_distance();
                boost_enabled   = false;
                selected_block_id = TERRAIN_GENERATOR_DIRT_BLOCK;
                fps_accumulator = 0.0; fps_frame_count = 0; display_fps = 0.0;
                render_distance_accumulator = 0.0;
                world_ready     = false;
                loading_frames  = 0;
                frame_ms        = 0.0;
                menu_ctl.clear_flags();
                menu_ctl.show_loading();
                phase = AppPhase::LOADING;
            }
        }
        else if (phase == AppPhase::LOADING)
        {
            menu_ctl.update(min_in);
            error_code = world.update_around(camera.x, camera.z, 8,
                active_render_distance);
            if (error_code != FT_ERR_SUCCESS) { menu_ctl.show_main_menu(); phase = AppPhase::MAIN_MENU; world.destroy(); }
            loading_frames++;
            if (loading_frames >= 30 && world.loaded_chunk_count >= 4)
            {
                world_ready = true;
                phase = AppPhase::IN_GAME;
                window.set_cursor_visible(false);
            }
        }
        else if (phase == AppPhase::IN_GAME)
        {
            ft_dumb_controls_poll();
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BOOST) == FT_TRUE)
                boost_enabled = !boost_enabled;
            CameraInput camera_input = InputReader::read_camera_input(boost_enabled);
            PlayerController::update_player_vertical_motion(&camera, camera_input,
                world, delta_seconds);
            PlayerController::update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            active_render_distance = std::min(active_render_distance,
                Settings::instance().render_distance());
            generation_budget = render_strategy->generation_budget_for_frame(
                frame_ms, boost_enabled);
            error_code = world.update_around(camera.x, camera.z,
                generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS) break;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_PRIMARY))
                BlockInteractor::try_delete_target_block(&world, camera);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_TERTIARY))
                BlockInteractor::try_pick_target_block(&world, camera, &selected_block_id);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_SECONDARY))
                BlockInteractor::try_place_selected_block(&world, camera, selected_block_id);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK) == FT_TRUE)
            {
                world.destroy(); world_ready = false;
                menu_ctl.show_main_menu(); phase = AppPhase::MAIN_MENU;
                window.set_cursor_visible(true);
            }
            if (window.was_settings_key_pressed())
            {
                menu_ctl.show_in_game_settings();
                phase = AppPhase::IN_GAME_SETTINGS;
                window.set_cursor_visible(true);
            }
            render_distance_accumulator += delta_seconds;
            if (render_strategy->should_update_render_distance(render_distance_accumulator))
            {
                active_render_distance = render_strategy->update_render_distance(
                    active_render_distance, frame_ms, boost_enabled);
                active_render_distance = std::min(active_render_distance,
                    Settings::instance().render_distance());
                render_distance_accumulator = 0.0;
            }
            frame_ms = delta_seconds * 1000.0;
            fps_accumulator += delta_seconds;
            fps_frame_count += 1U;
            if (fps_accumulator >= 0.25)
            {
                display_fps     = static_cast<double>(fps_frame_count) / fps_accumulator;
                fps_accumulator = 0.0; fps_frame_count = 0;
            }
        }
        else if (phase == AppPhase::IN_GAME_SETTINGS)
        {
            menu_ctl.update(min_in);
            ft_dumb_controls_set_keyboard_layout(
                Settings::instance().keyboard_layout());
            if (menu_ctl.dismissed_in_game())
            {
                menu_ctl.clear_flags();
                phase = AppPhase::IN_GAME;
                window.set_cursor_visible(false);
            }
            else if (menu_ctl.wants_main_menu())
            {
                world.destroy(); world_ready = false;
                menu_ctl.show_main_menu(); menu_ctl.clear_flags();
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
            else if (phase == AppPhase::IN_GAME && world_ready)
            {
                render_debug.fps               = display_fps;
                render_debug.frame_ms          = frame_ms;
                render_debug.visible_chunks    = 0;
                render_debug.loaded_chunks     = world.loaded_chunk_count;
                render_debug.render_distance   = world.active_render_distance;
                render_debug.selected_block_id = selected_block_id;
                render_debug.ram_mb            = system_ram_mb();
                render_debug.vram_approx_mb    = renderer.get_gpu_renderer()
                    ? renderer.get_gpu_renderer()->gpu_mb_approx() : 0U;
                const RenderDebug *dbg_ptr =
                    Settings::instance().fps_overlay() ? &render_debug : nullptr;
                renderer.render_world(window.framebuffer(), camera, world, dbg_ptr);
            }
            else if (phase == AppPhase::IN_GAME_SETTINGS && world_ready)
            {
                renderer.render_world(window.framebuffer(), camera, world, nullptr);
                if (gpu) menu_ctl.render_overlay(gpu);
            }
        }

        window.present();
        frame_count += 1;
    }

    if (world_ready)
        world.destroy();
    window.set_cursor_visible(true);
    window.destroy();
    return (0);
}
