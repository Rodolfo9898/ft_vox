#include "Libft/Modules/DUMB/controls.hpp"
#include "Libft/Modules/DUMB/render_window.hpp"
#include "Libft/Modules/Debug/debug.hpp"
#include "Libft/Modules/Errno/errno.hpp"
#include "src/camera/camera.hpp"
#include "src/render/voxel_renderer.hpp"
#include "src/world/voxel_world.hpp"
#include <chrono>
#include <cstdio>

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

static ft_vox_camera_input ft_vox_read_camera_input()
{
    ft_vox_camera_input input;

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
    input.speed_boost = (ft_dumb_control_is_down(FT_DUMB_CONTROL_CONFIRM)
        == FT_TRUE);
    return (input);
}

int main()
{
    ft_render_window       window;
    ft_render_window_desc  description;
    ft_render_screen_size  screen_size;
    ft_vox_camera          camera;
    ft_vox_world           world;
    std::chrono::steady_clock::time_point previous_time;
    std::chrono::steady_clock::time_point current_time;
    std::chrono::duration<double> elapsed_time;
    double                 delta_seconds;
    int32_t                error_code;

    if (ft_vox_enable_debug_crash_traces() != 0)
        return (1);
    error_code = ft_vox_world_initialize(&world, "ft_vox-integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("voxel world initialization", error_code));
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
    ft_vox_camera_init(&camera);
    previous_time = std::chrono::steady_clock::now();
    while (window.should_close() == FT_FALSE)
    {
        current_time = std::chrono::steady_clock::now();
        elapsed_time = current_time - previous_time;
        previous_time = current_time;
        delta_seconds = elapsed_time.count();
        if (delta_seconds > 0.1)
            delta_seconds = 0.1;
        error_code = window.poll_events();
        if (error_code != FT_ERR_SUCCESS)
            break ;
        if (window.should_close() == FT_TRUE)
            break ;
        ft_dumb_controls_poll();
        if (ft_dumb_control_is_down(FT_DUMB_CONTROL_BACK) == FT_TRUE)
            break ;
        ft_vox_camera_update(&camera, ft_vox_read_camera_input(),
            delta_seconds);
        ft_vox_render_world(window.framebuffer(), camera, world);
        error_code = window.present();
        if (error_code != FT_ERR_SUCCESS)
            break ;
    }
    ft_vox_world_destroy(&world);
    if (window.destroy() != FT_ERR_SUCCESS)
        return (1);
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("main loop", error_code));
    return (0);
}
