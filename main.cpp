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

static constexpr double FT_VOX_PLAYER_HALF_WIDTH = 0.30;
static constexpr double FT_VOX_PLAYER_EYE_HEIGHT = 1.62;
static constexpr double FT_VOX_PLAYER_HEIGHT = 1.80;

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

static void ft_vox_camera_drop_to_ground(ft_vox_camera *camera,
    const ft_vox_world &world);
static void ft_vox_spawn_player_on_ground(ft_vox_camera *camera,
    const ft_vox_world &world);
static void ft_vox_update_player_horizontal_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds);
static void ft_vox_update_player_vertical_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds);
static bool ft_vox_ground_eye_y_at(const ft_vox_world &world, double x,
    double z, double *ground_eye_y);

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

static int ft_vox_validate_camera_speed()
{
    ft_vox_camera camera;
    ft_vox_camera_input input;
    double normal_start_z;
    double normal_distance;
    double boost_start_z;
    double boost_distance;

    ft_vox_camera_init(&camera);
    input = ft_vox_empty_camera_input();
    input.move_forward = true;
    normal_start_z = camera.z;
    ft_vox_camera_update(&camera, input, 1.0);
    normal_distance = std::fabs(camera.z - normal_start_z);
    input.speed_boost = true;
    boost_start_z = camera.z;
    ft_vox_camera_update(&camera, input, 1.0);
    boost_distance = std::fabs(camera.z - boost_start_z);
    std::printf("camera-speed: normal=%.3f boost=%.3f\n",
        normal_distance, boost_distance);
    if (std::fabs(normal_distance - camera.speed) > 0.05
        || std::fabs(boost_distance - (camera.speed * 20.0)) > 0.5)
        return (1);
    return (0);
}

static int ft_vox_validate_collision()
{
    ft_vox_world world;
    ft_vox_camera camera;
    ft_vox_camera_input input;
    double start_x;
    double start_z;
    double climb_start_y;
    double before_wall_x;
    double tunnel_start_z;
    double tunnel_end_z;
    double drop_start_y;
    double movement_delta_z;
    double wall_delta_x;
    double climb_delta_y;
    int32_t step_x;
    int32_t step_y;
    int32_t step_z;
    int32_t tunnel_x;
    int32_t tunnel_y;
    int32_t tunnel_z;
    int32_t drop_x;
    int32_t drop_y;
    int32_t drop_z;
    int32_t frame;
    int32_t error_code;
    double step_ground_eye_y;
    bool climbed_step;

    error_code = ft_vox_world_initialize(&world, "ft_vox-integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("collision world initialization", error_code));
    ft_vox_camera_init(&camera);
    ft_vox_spawn_player_on_ground(&camera, world);
    start_x = camera.x;
    start_z = camera.z;
    input = ft_vox_empty_camera_input();
    input.move_forward = true;
    ft_vox_update_player_horizontal_motion(&camera, input, world, 1.0);
    movement_delta_z = camera.z - start_z;
    if (std::fabs(camera.z - start_z) < 0.25)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "collision: failed movement sanity check\n");
        return (1);
    }
    before_wall_x = camera.x;
    (void)ft_vox_world_place_block_at(&world,
        static_cast<int32_t>(std::floor(camera.x + 1.0)),
        static_cast<int32_t>(std::floor(camera.y - 1.0)),
        static_cast<int32_t>(std::floor(camera.z)), TERRAIN_GENERATOR_STONE_BLOCK);
    input = ft_vox_empty_camera_input();
    input.move_right = true;
    ft_vox_update_player_horizontal_motion(&camera, input, world, 0.25);
    wall_delta_x = camera.x - before_wall_x;
    if (camera.x > before_wall_x + 0.75)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "collision: failed wall rejection check\n");
        return (1);
    }
    ft_vox_camera_init(&camera);
    ft_vox_spawn_player_on_ground(&camera, world);
    climb_start_y = camera.y;
    step_x = static_cast<int32_t>(std::floor(camera.x));
    step_z = static_cast<int32_t>(std::floor(camera.z + 1.0));
    if (ft_vox_ground_eye_y_at(world, static_cast<double>(step_x) + 0.5,
            static_cast<double>(step_z) + 0.5, &step_ground_eye_y) == false)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "collision: failed step surface setup\n");
        return (1);
    }
    step_y = static_cast<int32_t>(std::floor(step_ground_eye_y
        - FT_VOX_PLAYER_EYE_HEIGHT));
    frame = 0;
    while (frame < 4)
    {
        error_code = ft_vox_world_place_block_at(&world, step_x, step_y,
            step_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        if (error_code != FT_ERR_SUCCESS && error_code != FT_ERR_ALREADY_EXISTS)
        {
            ft_vox_world_destroy(&world);
            return (ft_vox_fail("collision step setup", error_code));
        }
        frame = frame + 1;
    }
    input = ft_vox_empty_camera_input();
    input.move_forward = true;
    input.jump_pressed = true;
    frame = 0;
    climbed_step = false;
    climb_delta_y = 0.0;
    while (frame < 120)
    {
        ft_vox_update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        ft_vox_update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        if (camera.y - climb_start_y > climb_delta_y)
            climb_delta_y = camera.y - climb_start_y;
        if (camera.y >= climb_start_y + 0.75 && camera.on_ground == true)
            climbed_step = true;
        input.jump_pressed = false;
        frame = frame + 1;
    }
    if (climbed_step == false)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "collision: failed one-block climb check y=%.2f start=%.2f ground=%d\n",
            camera.y, climb_start_y, camera.on_ground == true ? 1 : 0);
        return (1);
    }
    ft_vox_camera_init(&camera);
    ft_vox_spawn_player_on_ground(&camera, world);
    tunnel_x = static_cast<int32_t>(std::floor(camera.x));
    tunnel_y = static_cast<int32_t>(std::floor(camera.y
        - FT_VOX_PLAYER_EYE_HEIGHT));
    tunnel_start_z = camera.z;
    tunnel_z = static_cast<int32_t>(std::floor(camera.z));
    frame = 0;
    while (frame < 6)
    {
        (void)ft_vox_world_delete_block_at(&world, tunnel_x, tunnel_y,
            tunnel_z + frame);
        (void)ft_vox_world_delete_block_at(&world, tunnel_x, tunnel_y + 1,
            tunnel_z + frame);
        (void)ft_vox_world_place_block_at(&world, tunnel_x - 1, tunnel_y,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)ft_vox_world_place_block_at(&world, tunnel_x - 1, tunnel_y + 1,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)ft_vox_world_place_block_at(&world, tunnel_x + 1, tunnel_y,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)ft_vox_world_place_block_at(&world, tunnel_x + 1, tunnel_y + 1,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        frame = frame + 1;
    }
    input = ft_vox_empty_camera_input();
    input.move_forward = true;
    frame = 0;
    while (frame < 90)
    {
        ft_vox_update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        ft_vox_update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        frame = frame + 1;
    }
    tunnel_end_z = camera.z;
    if (tunnel_end_z < tunnel_start_z + 2.0)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr,
            "collision: failed one-block-wide tunnel check start=%.2f end=%.2f\n",
            tunnel_start_z, tunnel_end_z);
        return (1);
    }
    ft_vox_camera_init(&camera);
    ft_vox_spawn_player_on_ground(&camera, world);
    drop_start_y = camera.y;
    drop_x = static_cast<int32_t>(std::floor(camera.x));
    drop_y = static_cast<int32_t>(std::floor(camera.y
        - FT_VOX_PLAYER_EYE_HEIGHT)) - 1;
    drop_z = static_cast<int32_t>(std::floor(camera.z));
    frame = 0;
    while (frame < 5)
    {
        (void)ft_vox_world_delete_block_at(&world, drop_x, drop_y - frame,
            drop_z);
        frame = frame + 1;
    }
    camera.on_ground = false;
    camera.vertical_velocity = 0.0;
    input = ft_vox_empty_camera_input();
    frame = 0;
    while (frame < 90)
    {
        ft_vox_update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        ft_vox_update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        frame = frame + 1;
    }
    if (camera.y > drop_start_y - 1.0)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr,
            "collision: failed one-block drop shaft check start_y=%.2f end_y=%.2f\n",
            drop_start_y, camera.y);
        return (1);
    }
    std::printf("collision: ok start=(%.2f,%.2f) moved_z=%.2f wall_x=%.2f climb_y=%.2f tunnel_z=%.2f drop_y=%.2f\n",
        start_x, start_z, movement_delta_z, wall_delta_x, climb_delta_y,
        tunnel_end_z - tunnel_start_z, drop_start_y - camera.y);
    ft_vox_world_destroy(&world);
    return (0);
}

static size_t ft_vox_mesh_index_count_for_block(const ft_vox_world &world,
    int32_t world_x, int32_t world_z)
{
    int32_t index;
    int32_t chunk_x;
    int32_t chunk_z;

    chunk_x = world_x >= 0 ? world_x / GAME_VOXEL_CHUNK_WIDTH
        : -(((-world_x) + GAME_VOXEL_CHUNK_WIDTH - 1)
            / GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = world_z >= 0 ? world_z / GAME_VOXEL_CHUNK_DEPTH
        : -(((-world_z) + GAME_VOXEL_CHUNK_DEPTH - 1)
            / GAME_VOXEL_CHUNK_DEPTH);
    index = 0;
    while (index < world.chunk_count)
    {
        if (world.chunks[index].initialized == true
            && world.chunks[index].chunk_x == chunk_x
            && world.chunks[index].chunk_z == chunk_z)
            return (world.chunks[index].mesh.indices.size());
        index = index + 1;
    }
    return (0U);
}

static int ft_vox_validate_block_edit()
{
    ft_vox_world world;
    double surface_top;
    int32_t edit_x;
    int32_t edit_y;
    int32_t edit_z;
    uint32_t block_id;
    size_t mesh_before;
    size_t mesh_after_place;
    size_t mesh_after_delete;
    int32_t error_code;

    error_code = ft_vox_world_initialize(&world, "ft_vox-integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("block-edit world initialization", error_code));
    edit_x = 2;
    edit_z = -6;
    if (ft_vox_world_surface_top_at(&world, edit_x, edit_z,
            &surface_top) == false)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "block-edit: failed surface lookup\n");
        return (1);
    }
    edit_y = static_cast<int32_t>(std::floor(surface_top + 1.0));
    mesh_before = ft_vox_mesh_index_count_for_block(world, edit_x, edit_z);
    error_code = ft_vox_world_place_block_at(&world, edit_x, edit_y, edit_z,
        TERRAIN_GENERATOR_STONE_BLOCK);
    if (error_code != FT_ERR_SUCCESS)
    {
        ft_vox_world_destroy(&world);
        return (ft_vox_fail("block-edit place", error_code));
    }
    if (ft_vox_world_block_id_at(&world, edit_x, edit_y, edit_z,
            &block_id) == false || block_id != TERRAIN_GENERATOR_STONE_BLOCK)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "block-edit: placed block not visible in storage\n");
        return (1);
    }
    mesh_after_place = ft_vox_mesh_index_count_for_block(world, edit_x,
        edit_z);
    error_code = ft_vox_world_delete_block_at(&world, edit_x, edit_y,
        edit_z);
    if (error_code != FT_ERR_SUCCESS)
    {
        ft_vox_world_destroy(&world);
        return (ft_vox_fail("block-edit delete", error_code));
    }
    if (ft_vox_world_block_id_at(&world, edit_x, edit_y, edit_z,
            &block_id) == false || block_id != GAME_VOXEL_AIR_BLOCK)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr, "block-edit: deleted block not visible in storage\n");
        return (1);
    }
    mesh_after_delete = ft_vox_mesh_index_count_for_block(world, edit_x,
        edit_z);
    if (mesh_after_place == mesh_before || mesh_after_delete != mesh_before)
    {
        ft_vox_world_destroy(&world);
        std::fprintf(stderr,
            "block-edit: mesh counts did not update before=%zu place=%zu delete=%zu\n",
            mesh_before, mesh_after_place, mesh_after_delete);
        return (1);
    }
    std::printf("block-edit: ok before=%zu place=%zu delete=%zu\n",
        mesh_before, mesh_after_place, mesh_after_delete);
    ft_vox_world_destroy(&world);
    return (0);
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

static int32_t ft_vox_clamp_int(int32_t value, int32_t minimum,
    int32_t maximum)
{
    if (value < minimum)
        return (minimum);
    if (value > maximum)
        return (maximum);
    return (value);
}

static void ft_vox_camera_forward(const ft_vox_camera &camera,
    double *direction_x, double *direction_y, double *direction_z)
{
    double pitch_cos;

    if (direction_x == nullptr || direction_y == nullptr
        || direction_z == nullptr)
        return ;
    pitch_cos = std::cos(camera.pitch);
    *direction_x = std::sin(camera.yaw) * pitch_cos;
    *direction_y = std::sin(camera.pitch);
    *direction_z = std::cos(camera.yaw) * pitch_cos;
}

static int32_t ft_vox_try_delete_target_block(ft_vox_world *world,
    const ft_vox_camera &camera)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    ft_vox_camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = ft_vox_world_raycast_solid(world, camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &block_x,
        &block_y, &block_z);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    return (ft_vox_world_delete_block_at(world, block_x, block_y, block_z));
}

static int32_t ft_vox_try_pick_target_block(const ft_vox_world *world,
    const ft_vox_camera &camera, uint32_t *selected_block_id)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t hit_x;
    int32_t hit_y;
    int32_t hit_z;
    int32_t place_x;
    int32_t place_y;
    int32_t place_z;
    uint32_t hit_block_id;
    int32_t error_code;

    if (world == nullptr || selected_block_id == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    ft_vox_camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = ft_vox_world_raycast_edit_target(world, camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (hit_block_id != GAME_VOXEL_AIR_BLOCK
        && terrain_block_is_known(hit_block_id) == FT_TRUE)
        *selected_block_id = hit_block_id;
    return (FT_ERR_SUCCESS);
}

static bool ft_vox_block_overlaps_player(const ft_vox_camera &camera,
    int32_t block_x, int32_t block_y, int32_t block_z)
{
    double player_min_x;
    double player_max_x;
    double player_min_y;
    double player_max_y;
    double player_min_z;
    double player_max_z;

    player_min_x = camera.x - FT_VOX_PLAYER_HALF_WIDTH;
    player_max_x = camera.x + FT_VOX_PLAYER_HALF_WIDTH;
    player_min_y = camera.y - FT_VOX_PLAYER_EYE_HEIGHT;
    player_max_y = player_min_y + FT_VOX_PLAYER_HEIGHT;
    player_min_z = camera.z - FT_VOX_PLAYER_HALF_WIDTH;
    player_max_z = camera.z + FT_VOX_PLAYER_HALF_WIDTH;
    if (static_cast<double>(block_x + 1) <= player_min_x
        || static_cast<double>(block_x) >= player_max_x)
        return (false);
    if (static_cast<double>(block_y + 1) <= player_min_y
        || static_cast<double>(block_y) >= player_max_y)
        return (false);
    if (static_cast<double>(block_z + 1) <= player_min_z
        || static_cast<double>(block_z) >= player_max_z)
        return (false);
    return (true);
}

static int32_t ft_vox_try_place_selected_block(ft_vox_world *world,
    const ft_vox_camera &camera, uint32_t selected_block_id)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t hit_x;
    int32_t hit_y;
    int32_t hit_z;
    int32_t place_x;
    int32_t place_y;
    int32_t place_z;
    uint32_t hit_block_id;
    int32_t error_code;

    if (world == nullptr || selected_block_id == GAME_VOXEL_AIR_BLOCK)
        return (FT_ERR_INVALID_ARGUMENT);
    ft_vox_camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = ft_vox_world_raycast_edit_target(world, camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (ft_vox_block_overlaps_player(camera, place_x, place_y, place_z)
        == true)
        return (FT_ERR_SUCCESS);
    error_code = ft_vox_world_place_block_at(world, place_x, place_y, place_z,
        selected_block_id);
    if (error_code == FT_ERR_ALREADY_EXISTS || error_code == FT_ERR_NOT_FOUND)
        return (FT_ERR_SUCCESS);
    return (error_code);
}

static int32_t ft_vox_update_render_distance(int32_t current_distance,
    double frame_ms, bool boost_enabled)
{
    int32_t next_distance;

    next_distance = current_distance;
    if (frame_ms > 34.0 || boost_enabled == true)
        next_distance = next_distance - 16;
    else if (frame_ms > 25.0)
        next_distance = next_distance - 8;
    else if (frame_ms < 15.0)
        next_distance = next_distance + 8;
    return (ft_vox_clamp_int(next_distance, FT_VOX_WORLD_MIN_RENDER_DISTANCE,
        FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE));
}

static int32_t ft_vox_generation_budget_for_frame(double frame_ms,
    bool boost_enabled)
{
    if (boost_enabled == true || frame_ms > 25.0)
        return (1);
    if (frame_ms < 12.0)
        return (4);
    return (2);
}

static bool ft_vox_ground_eye_y_at(const ft_vox_world &world, double x,
    double z, double *ground_eye_y)
{
    double surface_top;
    int32_t block_x;
    int32_t block_z;

    if (ground_eye_y == nullptr)
        return (false);
    block_x = static_cast<int32_t>(std::floor(x));
    block_z = static_cast<int32_t>(std::floor(z));
    if (ft_vox_world_surface_top_at(&world, block_x, block_z,
            &surface_top) == true)
    {
        *ground_eye_y = surface_top + FT_VOX_PLAYER_EYE_HEIGHT;
        return (true);
    }
    return (false);
}

static void ft_vox_camera_drop_to_ground(ft_vox_camera *camera,
    const ft_vox_world &world)
{
    double ground_eye_y;

    if (camera == nullptr)
        return ;
    if (ft_vox_ground_eye_y_at(world, camera->x, camera->z, &ground_eye_y)
        == true)
    {
        camera->y = ground_eye_y;
        camera->vertical_velocity = 0.0;
        camera->on_ground = true;
    }
}

static bool ft_vox_player_box_intersects_world(const ft_vox_world &world,
    double x, double eye_y, double z)
{
    const double foot_clearance = 0.05;
    const double head_clearance = 0.05;
    const double side_clearance = 0.02;
    double feet_y;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
    int32_t min_z;
    int32_t max_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;

    feet_y = eye_y - FT_VOX_PLAYER_EYE_HEIGHT;
    min_x = static_cast<int32_t>(std::floor(x - FT_VOX_PLAYER_HALF_WIDTH
        + side_clearance));
    max_x = static_cast<int32_t>(std::floor(x + FT_VOX_PLAYER_HALF_WIDTH
        - side_clearance));
    min_y = static_cast<int32_t>(std::floor(feet_y + foot_clearance));
    max_y = static_cast<int32_t>(std::floor(feet_y + FT_VOX_PLAYER_HEIGHT
        - head_clearance));
    min_z = static_cast<int32_t>(std::floor(z - FT_VOX_PLAYER_HALF_WIDTH
        + side_clearance));
    max_z = static_cast<int32_t>(std::floor(z + FT_VOX_PLAYER_HALF_WIDTH
        - side_clearance));
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

static void ft_vox_spawn_player_on_ground(ft_vox_camera *camera,
    const ft_vox_world &world)
{
    int32_t origin_x;
    int32_t origin_z;
    int32_t radius;
    int32_t offset_x;
    int32_t offset_z;
    double candidate_x;
    double candidate_z;
    double ground_eye_y;

    if (camera == nullptr)
        return ;
    origin_x = static_cast<int32_t>(std::floor(camera->x));
    origin_z = static_cast<int32_t>(std::floor(camera->z));
    radius = 0;
    while (radius <= 8)
    {
        offset_z = -radius;
        while (offset_z <= radius)
        {
            offset_x = -radius;
            while (offset_x <= radius)
            {
                if (offset_x == -radius || offset_x == radius
                    || offset_z == -radius || offset_z == radius)
                {
                    candidate_x = static_cast<double>(origin_x + offset_x)
                        + 0.5;
                    candidate_z = static_cast<double>(origin_z + offset_z)
                        + 0.5;
                    if (ft_vox_ground_eye_y_at(world, candidate_x,
                            candidate_z, &ground_eye_y) == true
                        && ft_vox_player_box_intersects_world(world,
                            candidate_x, ground_eye_y, candidate_z) == false)
                    {
                        camera->x = candidate_x;
                        camera->z = candidate_z;
                        camera->y = ground_eye_y;
                        camera->vertical_velocity = 0.0;
                        camera->on_ground = true;
                        return ;
                    }
                }
                offset_x = offset_x + 1;
            }
            offset_z = offset_z + 1;
        }
        radius = radius + 1;
    }
    ft_vox_camera_drop_to_ground(camera, world);
}

static void ft_vox_camera_horizontal_delta(const ft_vox_camera &camera,
    const ft_vox_camera_input &input, double delta_seconds, double *delta_x,
    double *delta_z)
{
    double local_x;
    double local_z;
    double move_length;
    double distance;
    double yaw_cos;
    double yaw_sin;
    double speed;

    if (delta_x == nullptr || delta_z == nullptr)
        return ;
    *delta_x = 0.0;
    *delta_z = 0.0;
    if (delta_seconds <= 0.0)
        return ;
    local_x = 0.0;
    local_z = 0.0;
    if (input.move_forward == true)
        local_z = local_z + 1.0;
    if (input.move_backward == true)
        local_z = local_z - 1.0;
    if (input.move_right == true)
        local_x = local_x + 1.0;
    if (input.move_left == true)
        local_x = local_x - 1.0;
    move_length = (local_x * local_x) + (local_z * local_z);
    if (move_length > 1.0)
    {
        local_x = local_x * 0.7071067811865475;
        local_z = local_z * 0.7071067811865475;
    }
    speed = camera.speed;
    if (input.speed_boost == true)
        speed = speed * 20.0;
    distance = speed * delta_seconds;
    yaw_cos = std::cos(camera.yaw);
    yaw_sin = std::sin(camera.yaw);
    *delta_x = ((local_x * yaw_cos) + (local_z * yaw_sin)) * distance;
    *delta_z = ((local_z * yaw_cos) - (local_x * yaw_sin)) * distance;
}

static void ft_vox_player_block_range(double x, double eye_y, double z,
    int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
    int32_t *min_z, int32_t *max_z)
{
    const double collision_epsilon = 0.001;
    double feet_y;

    if (min_x == nullptr || max_x == nullptr || min_y == nullptr
        || max_y == nullptr || min_z == nullptr || max_z == nullptr)
        return ;
    feet_y = eye_y - FT_VOX_PLAYER_EYE_HEIGHT;
    *min_x = static_cast<int32_t>(std::floor(x - FT_VOX_PLAYER_HALF_WIDTH
        + collision_epsilon));
    *max_x = static_cast<int32_t>(std::floor(x + FT_VOX_PLAYER_HALF_WIDTH
        - collision_epsilon));
    *min_y = static_cast<int32_t>(std::floor(feet_y + collision_epsilon));
    *max_y = static_cast<int32_t>(std::floor(feet_y + FT_VOX_PLAYER_HEIGHT
        - collision_epsilon));
    *min_z = static_cast<int32_t>(std::floor(z - FT_VOX_PLAYER_HALF_WIDTH
        + collision_epsilon));
    *max_z = static_cast<int32_t>(std::floor(z + FT_VOX_PLAYER_HALF_WIDTH
        - collision_epsilon));
}

static bool ft_vox_move_player_axis(ft_vox_camera *camera,
    const ft_vox_world &world, int axis, double delta)
{
    const double collision_epsilon = 0.001;
    double next_x;
    double next_y;
    double next_z;
    double candidate;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
    int32_t min_z;
    int32_t max_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
    bool collided;

    if (camera == nullptr || std::fabs(delta) < 0.000001)
        return (false);
    next_x = camera->x;
    next_y = camera->y;
    next_z = camera->z;
    if (axis == 0)
        next_x = next_x + delta;
    else if (axis == 1)
        next_y = next_y + delta;
    else
        next_z = next_z + delta;
    collided = false;
    ft_vox_player_block_range(next_x, next_y, next_z, &min_x, &max_x,
        &min_y, &max_y, &min_z, &max_z);
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
                {
                    collided = true;
                    if (axis == 0 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_x)
                            - FT_VOX_PLAYER_HALF_WIDTH - collision_epsilon;
                        if (candidate < next_x)
                            next_x = candidate;
                    }
                    else if (axis == 0)
                    {
                        candidate = static_cast<double>(block_x + 1)
                            + FT_VOX_PLAYER_HALF_WIDTH + collision_epsilon;
                        if (candidate > next_x)
                            next_x = candidate;
                    }
                    else if (axis == 1 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_y)
                            - FT_VOX_PLAYER_HEIGHT + FT_VOX_PLAYER_EYE_HEIGHT
                            - collision_epsilon;
                        if (candidate < next_y)
                            next_y = candidate;
                    }
                    else if (axis == 1)
                    {
                        candidate = static_cast<double>(block_y + 1)
                            + FT_VOX_PLAYER_EYE_HEIGHT + collision_epsilon;
                        if (candidate > next_y)
                            next_y = candidate;
                    }
                    else if (axis == 2 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_z)
                            - FT_VOX_PLAYER_HALF_WIDTH - collision_epsilon;
                        if (candidate < next_z)
                            next_z = candidate;
                    }
                    else
                    {
                        candidate = static_cast<double>(block_z + 1)
                            + FT_VOX_PLAYER_HALF_WIDTH + collision_epsilon;
                        if (candidate > next_z)
                            next_z = candidate;
                    }
                }
                block_x = block_x + 1;
            }
            block_z = block_z + 1;
        }
        block_y = block_y + 1;
    }
    camera->x = next_x;
    camera->y = next_y;
    camera->z = next_z;
    return (collided);
}

static void ft_vox_update_player_horizontal_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds)
{
    double delta_x;
    double delta_z;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    ft_vox_camera_update_orientation(camera, input, delta_seconds);
    ft_vox_camera_horizontal_delta(*camera, input, delta_seconds, &delta_x,
        &delta_z);
    (void)ft_vox_move_player_axis(camera, world, 0, delta_x);
    (void)ft_vox_move_player_axis(camera, world, 2, delta_z);
}

static void ft_vox_update_player_vertical_motion(ft_vox_camera *camera,
    const ft_vox_camera_input &input, const ft_vox_world &world,
    double delta_seconds)
{
    const double jump_speed = 12.0;
    const double gravity = 22.0;
    const double max_fall_speed = -42.0;
    double delta_y;
    double previous_y;
    bool collided;

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
    delta_y = camera->vertical_velocity * delta_seconds;
    previous_y = camera->y;
    collided = ft_vox_move_player_axis(camera, world, 1, delta_y);
    if (collided == true)
    {
        camera->on_ground = camera->vertical_velocity <= 0.0
            && camera->y >= previous_y + delta_y;
        camera->vertical_velocity = 0.0;
        return ;
    }
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
    double                 render_distance_accumulator;
    double                 display_fps;
    double                 frame_ms;
    uint64_t               perf_frame_hash;
    uint64_t               frame_count;
    uint64_t               fps_frame_count;
    int32_t                error_code;
    int32_t                active_render_distance;
    int32_t                generation_budget;
    bool                   perf_test_mode;
    bool                   perf_headless_mode;
    bool                   perf_move_mode;
    bool                   perf_boost_mode;
    bool                   validate_camera_speed_mode;
    bool                   validate_collision_mode;
    bool                   validate_block_edit_mode;
    bool                   validate_visible_distance_mode;
    bool                   boost_enabled;
    uint32_t               selected_block_id;
    std::vector<uint32_t>  headless_pixels;

    if (ft_vox_enable_debug_crash_traces() != 0)
        return (1);
    perf_headless_mode = ft_vox_has_flag(argc, argv, "--perf-test-headless");
    perf_test_mode = ft_vox_has_flag(argc, argv, "--perf-test")
        || perf_headless_mode;
    perf_move_mode = ft_vox_has_flag(argc, argv, "--perf-move");
    perf_boost_mode = ft_vox_has_flag(argc, argv, "--perf-boost");
    validate_camera_speed_mode = ft_vox_has_flag(argc, argv,
        "--validate-camera-speed");
    validate_collision_mode = ft_vox_has_flag(argc, argv,
        "--validate-collision");
    validate_block_edit_mode = ft_vox_has_flag(argc, argv,
        "--validate-block-edit");
    validate_visible_distance_mode = ft_vox_has_flag(argc, argv,
        "--validate-visible-distance");
    perf_seconds_limit = 10.0;
    error_code = ft_vox_parse_perf_seconds(argc, argv, &perf_seconds_limit);
    if (error_code < 0)
    {
        std::fprintf(stderr,
            "ft_vox: invalid perf duration, use --perf-seconds=SECONDS\n");
        return (1);
    }
    if (validate_camera_speed_mode == true)
        return (ft_vox_validate_camera_speed());
    if (validate_collision_mode == true)
        return (ft_vox_validate_collision());
    if (validate_block_edit_mode == true)
        return (ft_vox_validate_block_edit());
    error_code = ft_vox_world_initialize(&world, "ft_vox-integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ft_vox_fail("voxel world initialization", error_code));
    if (validate_visible_distance_mode == true)
    {
        ft_vox_camera validation_camera;

        ft_vox_camera_init(&validation_camera);
        ft_vox_spawn_player_on_ground(&validation_camera, world);
        error_code = ft_vox_world_update_around(&world, validation_camera.x,
            validation_camera.z, FT_VOX_WORLD_CHUNK_COUNT);
        if (error_code != FT_ERR_SUCCESS)
        {
            ft_vox_world_destroy(&world);
            return (ft_vox_fail("visible-distance chunk preparation",
                error_code));
        }
        if (ft_vox_world_validate_visible_distance(&world,
                validation_camera.x, validation_camera.z,
                validation_camera.yaw,
                FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE) == false)
        {
            std::fprintf(stderr,
                "visible-distance: failed required=%d chunks_loaded=%d\n",
                FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE,
                world.loaded_chunk_count);
            ft_vox_world_destroy(&world);
            return (1);
        }
        std::printf("visible-distance: ok required=%d chunks_loaded=%d\n",
            FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE, world.loaded_chunk_count);
        ft_vox_world_destroy(&world);
        return (0);
    }
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
    ft_vox_spawn_player_on_ground(&camera, world);
    ft_vox_render_preload_assets();
    previous_time = std::chrono::steady_clock::now();
    perf_start_time = previous_time;
    frame_count = 0;
    fps_frame_count = 0;
    fps_accumulator = 0.0;
    render_distance_accumulator = 0.0;
    display_fps = 0.0;
    frame_ms = 0.0;
    active_render_distance = FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE;
    boost_enabled = false;
    selected_block_id = TERRAIN_GENERATOR_DIRT_BLOCK;
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
            ft_vox_update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
            ft_vox_update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            generation_budget = ft_vox_generation_budget_for_frame(frame_ms,
                boost_enabled);
            error_code = ft_vox_world_update_around(&world, camera.x,
                camera.z, generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS)
                break ;
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_PRIMARY)
                == FT_TRUE)
            {
                error_code = ft_vox_try_delete_target_block(&world, camera);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_TERTIARY)
                == FT_TRUE)
            {
                error_code = ft_vox_try_pick_target_block(&world, camera,
                    &selected_block_id);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
            if (ft_dumb_control_was_pressed(FT_DUMB_CONTROL_MOUSE_SECONDARY)
                == FT_TRUE)
            {
                error_code = ft_vox_try_place_selected_block(&world, camera,
                    selected_block_id);
                if (error_code != FT_ERR_SUCCESS)
                    break ;
            }
        }
        else
        {
            camera_input = ft_vox_empty_camera_input();
            if (perf_move_mode == true)
            {
                camera_input.move_forward = true;
                camera_input.speed_boost = perf_boost_mode;
            }
            ft_vox_update_player_vertical_motion(&camera, camera_input, world,
                delta_seconds);
            ft_vox_update_player_horizontal_motion(&camera, camera_input,
                world, delta_seconds);
            generation_budget = ft_vox_generation_budget_for_frame(frame_ms,
                perf_boost_mode);
            error_code = ft_vox_world_update_around(&world, camera.x,
                camera.z, generation_budget, active_render_distance);
            if (error_code != FT_ERR_SUCCESS)
                break ;
        }
        frame_ms = delta_seconds * 1000.0;
        render_distance_accumulator = render_distance_accumulator
            + delta_seconds;
        if (perf_headless_mode == false && render_distance_accumulator >= 0.25)
        {
            active_render_distance = ft_vox_update_render_distance(
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
        if (perf_headless_mode == true)
        {
            ft_vox_render_world(headless_framebuffer, camera, world,
                static_cast<const ft_vox_render_debug *>(nullptr));
        }
        else
        {
            ft_vox_render_debug render_debug;

            render_debug.fps = display_fps;
            render_debug.frame_ms = frame_ms;
            render_debug.visible_chunks = 0;
            render_debug.loaded_chunks = world.loaded_chunk_count;
            render_debug.render_distance = world.active_render_distance;
            render_debug.selected_block_id = selected_block_id;
            ft_vox_render_world(headless_framebuffer, camera, world,
                &render_debug);
        }
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
