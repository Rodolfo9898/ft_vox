#include "../../src/app/Application.hpp"

Application::Application()
{
}

Application::Application(const Application &other)
{
    *this = other;
}

Application::~Application()
{
}

Application &Application::operator=(const Application &other)
{
    (void)other;
    return (*this);
}

int Application::run(int argc, char **argv)
{
    ApplicationOptions options;
    ApplicationWindow  window;
    Camera          camera;
    World           world;
    VoxelRenderer   renderer;
    std::chrono::steady_clock::time_point previous_time;
    std::chrono::steady_clock::time_point current_time;
    std::chrono::steady_clock::time_point perf_start_time;
    std::chrono::duration<double> elapsed_time;
    std::chrono::duration<double> perf_elapsed_time;
    double                 delta_seconds;
    double                 perf_fps;
    double                 perf_frame_ms;
    double                 fps_accumulator;
    double                 render_distance_accumulator;
    double                 display_fps;
    double                 frame_ms;
    uint64_t               perf_frame_hash;
    uint64_t               frame_count;
    uint64_t               fps_frame_count;
    int32_t                error_code;
    int32_t                active_render_distance;
    int32_t                generation_budget;
    const RenderDistanceStrategy *render_strategy;
    bool                   boost_enabled;
    uint32_t               selected_block_id;

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
    error_code = world.initialize("integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("voxel world initialization", error_code));
    if (options.validate_visible_distance_mode == true)
    {
        Camera validation_camera;

        validation_camera.initialize();
        PlayerController::spawn_player_on_ground(&validation_camera, world);
        error_code = world.update_around(validation_camera.x,
            validation_camera.z, WORLD_CHUNK_COUNT);
        if (error_code != FT_ERR_SUCCESS)
        {
            world.destroy();
            return (ApplicationError::fail("visible-distance chunk preparation",
                error_code));
        }
        if (world.validate_visible_distance(
                validation_camera.x, validation_camera.z,
                validation_camera.yaw,
                WORLD_REQUIRED_VISIBLE_DISTANCE) == false)
        {
            std::fprintf(stderr,
                "visible-distance: failed required=%d chunks_loaded=%d\n",
                WORLD_REQUIRED_VISIBLE_DISTANCE,
                world.loaded_chunk_count);
            world.destroy();
            return (1);
        }
        std::printf("visible-distance: ok required=%d chunks_loaded=%d\n",
            WORLD_REQUIRED_VISIBLE_DISTANCE, world.loaded_chunk_count);
        world.destroy();
        return (0);
    }
    render_strategy = &RenderStrategyFactory::select(options.perf_headless_mode);
    if (window.initialize(options.perf_headless_mode) != 0)
    {
        world.destroy();
        return (1);
    }
    camera.initialize();
    PlayerController::spawn_player_on_ground(&camera, world);
    renderer.preload_assets();
    previous_time = std::chrono::steady_clock::now();
    perf_start_time = previous_time;
    frame_count = 0;
    fps_frame_count = 0;
    fps_accumulator = 0.0;
    render_distance_accumulator = 0.0;
    display_fps = 0.0;
    frame_ms = 0.0;
    active_render_distance = WORLD_REQUIRED_VISIBLE_DISTANCE;
    boost_enabled = false;
    selected_block_id = TERRAIN_GENERATOR_DIRT_BLOCK;
    perf_frame_hash = UINT64_C(0);
    while (options.perf_headless_mode == true || window.should_close() == false)
    {
        CameraInput camera_input;

        current_time = std::chrono::steady_clock::now();
        elapsed_time = current_time - previous_time;
        previous_time = current_time;
        delta_seconds = elapsed_time.count();
        if (delta_seconds > 0.1)
            delta_seconds = 0.1;
        if (options.perf_headless_mode == false)
        {
            error_code = window.poll_events();
            if (error_code != FT_ERR_SUCCESS)
                break ;
            if (window.should_close() == true)
                break ;
            ft_dumb_controls_poll();
            if (options.perf_test_mode == false
                && ft_dumb_control_is_down(FT_DUMB_CONTROL_BACK) == FT_TRUE)
                break ;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BOOST) == FT_TRUE)
                boost_enabled = !boost_enabled;
            camera_input = InputReader::read_camera_input(boost_enabled);
            PlayerController::update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
            PlayerController::update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            generation_budget = render_strategy->generation_budget_for_frame(frame_ms,
                boost_enabled);
            error_code = world.update_around(camera.x,
                camera.z, generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS)
                break ;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_PRIMARY)
                == FT_TRUE)
            {
                error_code = BlockInteractor::try_delete_target_block(&world, camera);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_TERTIARY)
                == FT_TRUE)
            {
                error_code = BlockInteractor::try_pick_target_block(&world, camera,
                    &selected_block_id);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_SECONDARY)
                == FT_TRUE)
            {
                error_code = BlockInteractor::try_place_selected_block(&world, camera,
                    selected_block_id);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
        }
        else
        {
            camera_input = InputReader::empty_camera_input();
            if (options.perf_move_mode == true)
            {
                camera_input.move_forward = true;
                camera_input.speed_boost = options.perf_boost_mode;
            }
            PlayerController::update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
            PlayerController::update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            generation_budget = render_strategy->generation_budget_for_frame(frame_ms,
                options.perf_boost_mode);
            error_code = world.update_around(camera.x,
                camera.z, generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS)
                break ;
        }
        frame_ms = delta_seconds * 1000.0;
        render_distance_accumulator = render_distance_accumulator
            + delta_seconds;
        if (render_strategy->should_update_render_distance(
                render_distance_accumulator) == true)
        {
            active_render_distance = render_strategy->update_render_distance(
                active_render_distance, frame_ms, boost_enabled);
            render_distance_accumulator = 0.0;
        }
        fps_accumulator = fps_accumulator + delta_seconds;
        fps_frame_count = fps_frame_count + 1U;
        if (fps_accumulator >= 0.25)
        {
            display_fps = static_cast<double>(fps_frame_count)
                / fps_accumulator;
            fps_accumulator = 0.0;
            fps_frame_count = 0;
        }
        if (options.perf_headless_mode == true)
        {
            renderer.render_world(window.framebuffer(), camera, world,
                static_cast<const RenderDebug *>(nullptr));
        }
        else
        {
            RenderDebug render_debug;

            render_debug.fps = display_fps;
            render_debug.frame_ms = frame_ms;
            render_debug.visible_chunks = 0;
            render_debug.loaded_chunks = world.loaded_chunk_count;
            render_debug.render_distance = world.active_render_distance;
            render_debug.selected_block_id = selected_block_id;
            renderer.render_world(window.framebuffer(), camera, world,
                &render_debug);
        }
        if (options.perf_headless_mode == false)
        {
            error_code = window.present();
            if (error_code != FT_ERR_SUCCESS)
                break ;
        }
        frame_count += 1;
        if (options.perf_test_mode == true)
        {
            perf_frame_hash = FramebufferHasher::hash_framebuffer(window.framebuffer());
            current_time = std::chrono::steady_clock::now();
            perf_elapsed_time = current_time - perf_start_time;
            if (perf_elapsed_time.count() >= options.perf_seconds_limit)
                break ;
        }
    }
    if (options.perf_test_mode == true)
    {
        perf_elapsed_time = std::chrono::steady_clock::now() - perf_start_time;
        if (perf_elapsed_time.count() > 0.0)
        {
            perf_fps = static_cast<double>(frame_count)
                / perf_elapsed_time.count();
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
    {
        if (window.destroy() != FT_ERR_SUCCESS)
            return (1);
    }
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("main loop", error_code));
    return (0);
}
