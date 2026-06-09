#include "../../src/app/PerfSession.hpp"

PerfSession::PerfSession() {}
PerfSession::PerfSession(const PerfSession &other) { (void)other; }
PerfSession::~PerfSession() {}
PerfSession &PerfSession::operator=(const PerfSession &other)
{ (void)other; return *this; }

int PerfSession::run(const ApplicationOptions &options,
    const LaunchSettings &launch_settings,
    const RenderDistanceStrategy &strategy)
{
    Camera          camera;
    World           world;
    ApplicationWindow window;
    VoxelRenderer   renderer;

    int32_t  error_code = world.initialize("integration-seed");
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

    auto   prev_time   = std::chrono::steady_clock::now();
    auto   perf_start  = prev_time;
    double fps_acc     = 0.0, rd_acc = 0.0, display_fps = 0.0, frame_ms = 0.0;
    uint64_t frames = 0U, fps_frames = 0U;
    int32_t  active_rd = WORLD_REQUIRED_VISIBLE_DISTANCE;
    bool     boost     = false;
    uint32_t sel_block = TERRAIN_GENERATOR_DIRT_BLOCK;
    uint64_t perf_hash = UINT64_C(0);

    while (options.perf_headless_mode || !window.should_close())
    {
        auto   now = std::chrono::steady_clock::now();
        double dt  = std::min(
            std::chrono::duration<double>(now - prev_time).count(), 0.1);
        prev_time  = now;

        CameraInput input = InputReader::empty_camera_input();
        if (options.perf_move_mode)
        {
            input.move_forward = true;
            input.speed_boost  = options.perf_boost_mode;
        }
        if (!options.perf_headless_mode)
        {
            window.poll_events();
            if (window.should_close()) break;
            ft_dumb_controls_poll();
            if (!options.perf_test_mode &&
                ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BACK) == FT_TRUE)
                break;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BOOST) == FT_TRUE)
                boost = !boost;
            input = InputReader::read_camera_input(boost);
        }
        PlayerController::update_player_vertical_motion(&camera, input,
            world, dt);
        PlayerController::update_player_horizontal_motion(&camera, input,
            world, dt);
        int gen = strategy.generation_budget_for_frame(frame_ms,
            boost || options.perf_boost_mode);
        error_code = world.update_around(camera.x, camera.z, gen, active_rd);
        if (error_code != FT_ERR_SUCCESS) break;
        if (!options.perf_headless_mode)
        {
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_PRIMARY))
                BlockInteractor::try_delete_target_block(&world, camera);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_TERTIARY))
                BlockInteractor::try_pick_target_block(&world, camera, &sel_block);
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_SECONDARY))
                BlockInteractor::try_place_selected_block(&world, camera, sel_block);
        }
        frame_ms = dt * 1000.0;
        rd_acc  += dt;
        if (strategy.should_update_render_distance(rd_acc))
        {
            active_rd = strategy.update_render_distance(active_rd, frame_ms,
                boost || options.perf_boost_mode);
            rd_acc = 0.0;
        }
        fps_acc += dt; ++fps_frames;
        if (fps_acc >= 0.25)
        {
            display_fps = static_cast<double>(fps_frames) / fps_acc;
            fps_acc = 0.0; fps_frames = 0U;
        }
        (void)display_fps;
        renderer.render_world(window.framebuffer(), camera, world, nullptr);
        if (!options.perf_headless_mode) window.present();
        ++frames;
        if (options.perf_test_mode)
        {
            perf_hash = FramebufferHasher::hash_framebuffer(window.framebuffer());
            if (std::chrono::duration<double>(
                    std::chrono::steady_clock::now() - perf_start).count()
                    >= options.perf_seconds_limit)
                break;
        }
    }
    if (options.perf_test_mode)
    {
        double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - perf_start).count();
        if (elapsed > 0.0)
        {
            std::printf(
                "perf-test: frames=%llu elapsed=%.3f sec fps=%.2f "
                "frame_ms=%.3f hash=%016llx\n",
                static_cast<unsigned long long>(frames), elapsed,
                static_cast<double>(frames) / elapsed,
                elapsed * 1000.0 / static_cast<double>(frames == 0 ? 1 : frames),
                static_cast<unsigned long long>(perf_hash));
        }
        else
        {
            std::printf(
                "perf-test: frames=%llu elapsed=0.000 sec fps=0.00 "
                "frame_ms=0.000 hash=%016llx\n",
                static_cast<unsigned long long>(frames),
                static_cast<unsigned long long>(perf_hash));
        }
    }
    world.destroy();
    if (!options.perf_headless_mode) window.destroy();
    if (error_code != FT_ERR_SUCCESS)
        return ApplicationError::fail("main loop", error_code);
    return (0);
}
