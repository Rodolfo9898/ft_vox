#include "Libft/Modules/DUMB/controls.hpp"
#include "Libft/Modules/DUMB/render_window.hpp"
#include "Libft/Modules/Debug/debug.hpp"
#include "Libft/Modules/Errno/errno.hpp"
#include "src/camera/camera.hpp"
#include "src/render/voxel_renderer.hpp"
#include "src/world/voxel_world.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>

static int ft_vox_fail(const char *operation, int32_t error_code)
{
    std::fprintf(stderr, "ft_vox: %s failed: %s (%d)\n", operation,
        ft_strerror(error_code), error_code);
    return (1);
}

static int ft_vox_enable_debug_crash_traces()
{
#ifdef DEBUG
    int32_t error_code;

    error_code = dbg_enable_crash_stack_traces();
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("debug crash stack trace setup", error_code));
#endif
    return (0);
}

static ft_vox_camera_input ft_vox_read_camera_input(bool boost_enabled)
{
    ft_vox_camera_input input;
    ft_dumb_mouse_delta mouse_delta;

    mouse_delta = ft_dumb_controls_mouse_delta();
    input.move_forward = (ft_dumb_control_is_down(FT_DUMB_CONTROL_UP)
        == FT_TRUE);
    input.move_backward = (ft_dumb_control_is_down(FT_DUMB_CONTROL_DOWN)
        == FT_TRUE);
    input.move_left = (ft_dumb_control_is_down(FT_DUMB_CONTROL_LEFT)
        == FT_TRUE);
    input.move_right = (ft_dumb_control_is_down(FT_DUMB_CONTROL_RIGHT)
        == FT_TRUE);
    input.turn_left = false;
    input.turn_right = false;
    input.look_up = false;
    input.look_down = false;
    input.speed_boost = boost_enabled;
    input.jump_pressed = (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_JUMP)
        == FT_TRUE);
    input.mouse_delta_x = static_cast<double>(mouse_delta.x);
    input.mouse_delta_y = static_cast<double>(mouse_delta.y);
    return (input);
}

static ft_vox_camera_input ft_vox_empty_camera_input()
{
    ft_vox_camera_input input;

    input.move_forward = false;
    input.move_backward = false;
    input.move_left = false;
    input.move_right = false;
    input.turn_left = false;
    input.turn_right = false;
    input.look_up = false;
    input.look_down = false;
    input.speed_boost = false;
    input.jump_pressed = false;
    input.mouse_delta_x = 0.0;
    input.mouse_delta_y = 0.0;
    return (input);
}

static bool ft_vox_has_flag(int argc, char **argv, const char *flag)
{
    int index;

    index = 1;
    while (index < argc)
    {
        if (argv[index] != nullptr && std::strcmp(argv[index], flag) == 0)
            return (true);
        index += 1;
    }
    return (false);
}

static int ft_vox_parse_perf_seconds(int argc, char **argv,
    double *seconds_out)
{
    int index;
    const char *argument;
    const char *value_string;
    char *end_pointer;
    double parsed_value;

    if (seconds_out == nullptr)
        return (-1);
    index = 1;
    while (index < argc)
    {
        argument = argv[index];
        if (argument != nullptr && std::strncmp(argument, "--perf-seconds", 14) == 0)
        {
            value_string = argument + 14;
            if (*value_string == '=')
                value_string += 1;
            else if (*value_string == '\0')
            {
                if (index + 1 >= argc || argv[index + 1] == nullptr)
                    return (0);
                value_string = argv[index + 1];
            }
            if (*value_string == '\0')
                return (-1);
            parsed_value = std::strtod(value_string, &end_pointer);
            if (end_pointer == value_string || *end_pointer != '\0'
                || parsed_value <= 0.0)
                return (-1);
            *seconds_out = parsed_value;
            return (1);
        }
        index += 1;
    }
    return (0);
}

static uint64_t ft_vox_hash_framebuffer(const ft_render_framebuffer &framebuffer)
{
    const uint64_t fnv_offset_basis = UINT64_C(1469598103934665603);
    const uint64_t fnv_prime = UINT64_C(1099511628211);
    uint64_t hash;
    size_t index;
    size_t pixel_count;

    if (framebuffer.pixels == nullptr || framebuffer.width <= 0
        || framebuffer.height <= 0)
        return (UINT64_C(0));
    hash = fnv_offset_basis;
    pixel_count = static_cast<size_t>(framebuffer.width)
        * static_cast<size_t>(framebuffer.height);
    index = 0;
    while (index < pixel_count)
    {
        hash ^= static_cast<uint64_t>(framebuffer.pixels[index]);
        hash *= fnv_prime;
        index += 1;
    }
    return (hash);
}

static void ft_vox_camera_follow_ground(ft_vox_camera *camera,
    const ft_vox_world &world)
{
    const double eye_height = 1.62;
    double surface_top;
    int32_t block_x;
    int32_t block_z;

    if (camera == nullptr)
        return ;
    block_x = static_cast<int32_t>(std::floor(camera->x));
    block_z = static_cast<int32_t>(std::floor(camera->z));
    if (ft_vox_world_surface_top_at(&world, block_x, block_z,
            &surface_top) == true)
    {
        camera->y = surface_top + eye_height;
        camera->vertical_velocity = 0.0;
        camera->on_ground = true;
    }
}

static bool ft_vox_player_intersects_world(const ft_vox_world &world,
    double x, double y, double z)
{
    const double player_radius = 0.28;
    const double eye_height = 1.62;
    const double player_height = 1.80;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
    int32_t min_z;
    int32_t max_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
    double feet_y;

    feet_y = y - eye_height;
    min_x = static_cast<int32_t>(std::floor(x - player_radius));
    max_x = static_cast<int32_t>(std::floor(x + player_radius));
    min_y = static_cast<int32_t>(std::floor(feet_y + 0.08));
    max_y = static_cast<int32_t>(std::floor(feet_y + player_height - 0.08));
    min_z = static_cast<int32_t>(std::floor(z - player_radius));
    max_z = static_cast<int32_t>(std::floor(z + player_radius));
    block_y = min_y;
    while (block_y <= max_y)
    {
        block_z = min_z;
        while (block_z <= max_z)
        {
            block_x = min_x;
            while (block_x <= max_x)
            {
                if (ft_vox_world_solid_block_at(&world, block_x, block_y,
                        block_z) == true)
                    return (true);
                block_x = block_x + 1;
            }
            block_z = block_z + 1;
        }
        block_y = block_y + 1;
    }
    return (false);
}

static void ft_vox_update_player_horizontal_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds)
{
    (void)world;
    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    ft_vox_camera_update(camera, input, delta_seconds);
}

static void ft_vox_update_player_vertical_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds)
{
    const double eye_height = 1.62;
    const double jump_speed = 7.2;
    const double gravity = 22.0;
    const double max_fall_speed = -42.0;
    double new_y;
    double surface_top;
    double ground_eye_y;
    int32_t block_x;
    int32_t block_z;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    if (camera->on_ground == true && input.jump_pressed == true)
    {
        camera->vertical_velocity = jump_speed;
        camera->on_ground = false;
    }
    camera->vertical_velocity = camera->vertical_velocity
        - (gravity * delta_seconds);
    if (camera->vertical_velocity < max_fall_speed)
        camera->vertical_velocity = max_fall_speed;
    new_y = camera->y + (camera->vertical_velocity * delta_seconds);
    if (camera->vertical_velocity > 0.0
        && ft_vox_player_intersects_world(world, camera->x, new_y,
            camera->z) == true)
    {
        camera->vertical_velocity = 0.0;
        return ;
    }
    block_x = static_cast<int32_t>(std::floor(camera->x));
    block_z = static_cast<int32_t>(std::floor(camera->z));
    if (ft_vox_world_surface_top_at(&world, block_x, block_z,
            &surface_top) == true)
    {
        ground_eye_y = surface_top + eye_height;
        if (new_y <= ground_eye_y && camera->vertical_velocity <= 0.0)
        {
            camera->y = ground_eye_y;
            camera->vertical_velocity = 0.0;
            camera->on_ground = true;
            return ;
        }
    }
    camera->y = new_y;
    camera->on_ground = false;
}

int main(int argc, char **argv)
{
    ft_render_window       window;
    ft_render_window_desc  description;
    ft_render_screen_size  screen_size;
    ft_render_framebuffer  headless_framebuffer;
    ft_vox_camera          camera;
    ft_vox_world           world;
    std::chrono::steady_clock::time_point previous_time;
    std::chrono::steady_clock::time_point current_time;
    std::chrono::steady_clock::time_point perf_start_time;
    std::chrono::duration<double> elapsed_time;
    std::chrono::duration<double> perf_elapsed_time;
    double                 delta_seconds;
    double                 perf_fps;
    double                 perf_frame_ms;
    double                 perf_seconds_limit;
    double                 fps_accumulator;
    double                 display_fps;
    uint64_t               perf_frame_hash;
    uint64_t               frame_count;
    uint64_t               fps_frame_count;
    int32_t                error_code;
    bool                   perf_test_mode;
    bool                   perf_headless_mode;
    bool                   boost_enabled;
    std::vector<uint32_t>  headless_pixels;

    if (ft_vox_enable_debug_crash_traces() != 0)
        return (1);
    perf_headless_mode = ft_vox_has_flag(argc, argv, "--perf-test-headless");
    perf_test_mode = ft_vox_has_flag(argc, argv, "--perf-test")
        || perf_headless_mode;
    perf_seconds_limit = 10.0;
    error_code = ft_vox_parse_perf_seconds(argc, argv, &perf_seconds_limit);
    if (error_code < 0)
    {
        std::fprintf(stderr,
            "ft_vox: invalid perf duration, use --perf-seconds=SECONDS\n");
        return (1);
    }
    error_code = ft_vox_world_initialize(&world, "ft_vox-integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("voxel world initialization", error_code));
    if (perf_headless_mode == false)
    {
        error_code = window.initialize();
        if (error_code != FT_ERR_SUCCESS)
        {
            ft_vox_world_destroy(&world);
            return (ft_vox_fail("window object initialization", error_code));
        }
        screen_size = ft_render_get_primary_screen_size();
        if (screen_size.width <= 0 || screen_size.height <= 0)
        {
            screen_size.width = 1280;
            screen_size.height = 720;
        }
        description.width = screen_size.width;
        description.height = screen_size.height;
        description.title = "ft_vox";
        description.flags = FT_RENDER_WINDOW_FLAG_FULLSCREEN;
        error_code = window.initialize(description);
        if (error_code != FT_ERR_SUCCESS)
        {
            (void)window.destroy();
            ft_vox_world_destroy(&world);
            return (ft_vox_fail("window creation", error_code));
        }
        error_code = window.set_fullscreen(FT_TRUE);
        if (error_code != FT_ERR_SUCCESS)
        {
            (void)window.destroy();
            ft_vox_world_destroy(&world);
            return (ft_vox_fail("fullscreen activation", error_code));
        }
        headless_framebuffer = window.framebuffer();
    }
    else
    {
        headless_framebuffer.width = 1280;
        headless_framebuffer.height = 720;
        headless_pixels.resize(static_cast<size_t>(headless_framebuffer.width)
            * static_cast<size_t>(headless_framebuffer.height));
        headless_framebuffer.pixels = headless_pixels.data();
    }
    ft_vox_camera_init(&camera);
    ft_vox_camera_follow_ground(&camera, world);
    previous_time = std::chrono::steady_clock::now();
    perf_start_time = previous_time;
    frame_count = 0;
    fps_frame_count = 0;
    fps_accumulator = 0.0;
    display_fps = 0.0;
    boost_enabled = false;
    perf_frame_hash = UINT64_C(0);
    while (perf_headless_mode == true || window.should_close() == FT_FALSE)
    {
        ft_vox_camera_input camera_input;

        current_time = std::chrono::steady_clock::now();
        elapsed_time = current_time - previous_time;
        previous_time = current_time;
        delta_seconds = elapsed_time.count();
        if (delta_seconds > 0.1)
            delta_seconds = 0.1;
        if (perf_headless_mode == false)
        {
            error_code = window.poll_events();
            if (error_code != FT_ERR_SUCCESS)
                break ;
            if (window.should_close() == FT_TRUE)
                break ;
            ft_dumb_controls_poll();
            if (perf_test_mode == false
                && ft_dumb_control_is_down(FT_DUMB_CONTROL_BACK) == FT_TRUE)
                break ;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_BOOST) == FT_TRUE)
                boost_enabled = !boost_enabled;
            camera_input = ft_vox_read_camera_input(boost_enabled);
            ft_vox_update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            ft_vox_update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
        }
        else
        {
            camera_input = ft_vox_empty_camera_input();
            ft_vox_update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            ft_vox_update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
            error_code = FT_ERR_SUCCESS;
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
        ft_vox_render_world(headless_framebuffer, camera, world,
            perf_headless_mode == true ? -1.0 : display_fps);
        if (perf_headless_mode == false)
        {
            error_code = window.present();
            if (error_code != FT_ERR_SUCCESS)
                break ;
        }
        frame_count += 1;
        if (perf_test_mode == true)
        {
            perf_frame_hash = ft_vox_hash_framebuffer(headless_framebuffer);
            current_time = std::chrono::steady_clock::now();
            perf_elapsed_time = current_time - perf_start_time;
            if (perf_elapsed_time.count() >= perf_seconds_limit)
                break ;
        }
    }
    if (perf_test_mode == true)
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
    ft_vox_world_destroy(&world);
    if (perf_headless_mode == false)
    {
        if (window.destroy() != FT_ERR_SUCCESS)
            return (1);
    }
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("main loop", error_code));
    return (0);
}
