#include "voxel_world.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include <cmath>
#include <cstring>

static int32_t ft_vox_floor_divide(int32_t value, int32_t divisor)
{
    if (value >= 0)
        return (value / divisor);
    return (-(((-value) + divisor - 1) / divisor));
}

static int32_t ft_vox_positive_modulo(int32_t value, int32_t divisor)
{
    int32_t result;

    result = value % divisor;
    if (result < 0)
        result += divisor;
    return (result);
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

static int32_t ft_vox_render_distance_to_chunk_radius(int32_t render_distance)
{
    int32_t clamped_distance;
    int32_t chunk_radius;

    clamped_distance = ft_vox_clamp_int(render_distance,
        FT_VOX_WORLD_MIN_RENDER_DISTANCE,
        FT_VOX_WORLD_CACHE_CHUNK_RADIUS * GAME_VOXEL_CHUNK_WIDTH);
    chunk_radius = (clamped_distance + GAME_VOXEL_CHUNK_WIDTH - 1)
        / GAME_VOXEL_CHUNK_WIDTH;
    if (chunk_radius < 1)
        chunk_radius = 1;
    if (chunk_radius > FT_VOX_WORLD_CACHE_CHUNK_RADIUS)
        chunk_radius = FT_VOX_WORLD_CACHE_CHUNK_RADIUS;
    return (chunk_radius);
}

static const ft_vox_world_chunk *ft_vox_find_chunk(const ft_vox_world *world,
    int32_t chunk_x, int32_t chunk_z)
{
    int32_t index;

    if (world == nullptr)
        return (nullptr);
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == true
            && world->chunks[index].chunk_x == chunk_x
            && world->chunks[index].chunk_z == chunk_z)
        {
            return (&world->chunks[index]);
        }
        index = index + 1;
    }
    return (nullptr);
}

static ft_vox_world_chunk *ft_vox_find_chunk_mutable(ft_vox_world *world,
    int32_t chunk_x, int32_t chunk_z)
{
    int32_t index;

    if (world == nullptr)
        return (nullptr);
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == true
            && world->chunks[index].chunk_x == chunk_x
            && world->chunks[index].chunk_z == chunk_z)
        {
            return (&world->chunks[index]);
        }
        index = index + 1;
    }
    return (nullptr);
}

static ft_vox_world_chunk *ft_vox_find_free_chunk_slot(ft_vox_world *world)
{
    int32_t index;

    if (world == nullptr)
        return (nullptr);
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == false)
            return (&world->chunks[index]);
        index = index + 1;
    }
    return (nullptr);
}

static void ft_vox_destroy_world_chunk(ft_vox_world_chunk *world_chunk)
{
    if (world_chunk == nullptr || world_chunk->initialized == false)
        return ;
    (void)chunk_mesh_destroy(world_chunk->mesh);
    (void)world_chunk->chunk.destroy();
    world_chunk->initialized = false;
}

static void ft_vox_copy_seed(ft_vox_world *world, const char *seed)
{
    if (world == nullptr)
        return ;
    if (seed == nullptr)
        seed = "";
    std::strncpy(world->seed, seed, sizeof(world->seed) - 1U);
    world->seed[sizeof(world->seed) - 1U] = '\0';
}

static int32_t ft_vox_initialize_chunk(ft_vox_world_chunk *world_chunk,
    int32_t chunk_x, int32_t chunk_z, const char *seed)
{
    int32_t error_code;

    world_chunk->chunk_x = chunk_x;
    world_chunk->chunk_z = chunk_z;
    world_chunk->world_x = chunk_x * GAME_VOXEL_CHUNK_WIDTH;
    world_chunk->world_z = chunk_z * GAME_VOXEL_CHUNK_DEPTH;
    world_chunk->initialized = false;
    error_code = world_chunk->chunk.initialize();
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    error_code = chunk_mesh_initialize(world_chunk->mesh);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)world_chunk->chunk.destroy();
        return (error_code);
    }
    error_code = terrain_generate_chunk(world_chunk->chunk,
        world_chunk->world_x, world_chunk->world_z, seed);
    if (error_code == FT_ERR_SUCCESS)
        error_code = chunk_mesh_generate_from_chunk(world_chunk->mesh,
            world_chunk->chunk);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)chunk_mesh_destroy(world_chunk->mesh);
        (void)world_chunk->chunk.destroy();
        return (error_code);
    }
    world_chunk->initialized = true;
    return (FT_ERR_SUCCESS);
}

int32_t ft_vox_world_initialize(ft_vox_world *world, const char *seed)
{
    int32_t index;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    world->chunk_count = FT_VOX_WORLD_CHUNK_COUNT;
    world->loaded_chunk_count = 0;
    world->center_chunk_x = 0;
    world->center_chunk_z = 0;
    world->active_render_distance = FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE;
    ft_vox_copy_seed(world, seed);
    index = 0;
    while (index < world->chunk_count)
    {
        world->chunks[index].chunk_x = 0;
        world->chunks[index].chunk_z = 0;
        world->chunks[index].world_x = 0;
        world->chunks[index].world_z = 0;
        world->chunks[index].initialized = false;
        index = index + 1;
    }
    error_code = ft_vox_world_update_around(world, 0.0, 0.0,
        FT_VOX_WORLD_CHUNK_COUNT, world->active_render_distance);
    if (error_code != FT_ERR_SUCCESS)
        ft_vox_world_destroy(world);
    return (error_code);
}

void ft_vox_world_destroy(ft_vox_world *world)
{
    int32_t index;

    if (world == nullptr)
        return ;
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == true)
        {
            ft_vox_destroy_world_chunk(&world->chunks[index]);
        }
        index = index + 1;
    }
    world->loaded_chunk_count = 0;
    return ;
}

static int32_t ft_vox_chunk_distance_squared(int32_t chunk_x, int32_t chunk_z,
    int32_t center_chunk_x, int32_t center_chunk_z)
{
    int32_t delta_x;
    int32_t delta_z;

    delta_x = chunk_x - center_chunk_x;
    delta_z = chunk_z - center_chunk_z;
    return ((delta_x * delta_x) + (delta_z * delta_z));
}

static void ft_vox_world_evict_far_chunks(ft_vox_world *world,
    int32_t center_chunk_x, int32_t center_chunk_z)
{
    int32_t index;
    int32_t cache_radius_squared;

    if (world == nullptr)
        return ;
    cache_radius_squared = FT_VOX_WORLD_CACHE_CHUNK_RADIUS
        * FT_VOX_WORLD_CACHE_CHUNK_RADIUS;
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == true
            && ft_vox_chunk_distance_squared(world->chunks[index].chunk_x,
                world->chunks[index].chunk_z, center_chunk_x, center_chunk_z)
                > cache_radius_squared)
        {
            ft_vox_destroy_world_chunk(&world->chunks[index]);
            if (world->loaded_chunk_count > 0)
                world->loaded_chunk_count = world->loaded_chunk_count - 1;
        }
        index = index + 1;
    }
}

static int32_t ft_vox_world_generate_missing_chunk(ft_vox_world *world,
    int32_t chunk_x, int32_t chunk_z)
{
    ft_vox_world_chunk *slot;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    if (ft_vox_find_chunk(world, chunk_x, chunk_z) != nullptr)
        return (FT_ERR_SUCCESS);
    slot = ft_vox_find_free_chunk_slot(world);
    if (slot == nullptr)
        return (FT_ERR_NO_MEMORY);
    error_code = ft_vox_initialize_chunk(slot, chunk_x, chunk_z, world->seed);
    if (error_code == FT_ERR_SUCCESS)
        world->loaded_chunk_count = world->loaded_chunk_count + 1;
    return (error_code);
}

int32_t ft_vox_world_update_around(ft_vox_world *world,
    double camera_x, double camera_z, int32_t generation_budget)
{
    return (ft_vox_world_update_around(world, camera_x, camera_z,
        generation_budget, FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE));
}

int32_t ft_vox_world_update_around(ft_vox_world *world,
    double camera_x, double camera_z, int32_t generation_budget,
    int32_t render_distance)
{
    int32_t center_chunk_x;
    int32_t center_chunk_z;
    int32_t stream_chunk_radius;
    int32_t radius;
    int32_t offset_x;
    int32_t offset_z;
    int32_t generated_count;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    center_chunk_x = ft_vox_floor_divide(
        static_cast<int32_t>(std::floor(camera_x)), GAME_VOXEL_CHUNK_WIDTH);
    center_chunk_z = ft_vox_floor_divide(
        static_cast<int32_t>(std::floor(camera_z)), GAME_VOXEL_CHUNK_DEPTH);
    world->center_chunk_x = center_chunk_x;
    world->center_chunk_z = center_chunk_z;
    world->active_render_distance = ft_vox_clamp_int(render_distance,
        FT_VOX_WORLD_MIN_RENDER_DISTANCE,
        FT_VOX_WORLD_CACHE_CHUNK_RADIUS * GAME_VOXEL_CHUNK_WIDTH);
    stream_chunk_radius = ft_vox_render_distance_to_chunk_radius(
        world->active_render_distance);
    ft_vox_world_evict_far_chunks(world, center_chunk_x, center_chunk_z);
    generated_count = 0;
    radius = 0;
    while (radius <= stream_chunk_radius)
    {
        offset_z = -radius;
        while (offset_z <= radius)
        {
            offset_x = -radius;
            while (offset_x <= radius)
            {
                if ((offset_x == -radius || offset_x == radius
                        || offset_z == -radius || offset_z == radius)
                    && ft_vox_chunk_distance_squared(center_chunk_x + offset_x,
                        center_chunk_z + offset_z, center_chunk_x,
                        center_chunk_z) <= stream_chunk_radius
                        * stream_chunk_radius)
                {
                    if (ft_vox_find_chunk(world, center_chunk_x + offset_x,
                            center_chunk_z + offset_z) == nullptr)
                    {
                        error_code = ft_vox_world_generate_missing_chunk(world,
                            center_chunk_x + offset_x,
                            center_chunk_z + offset_z);
                        if (error_code != FT_ERR_SUCCESS)
                            return (error_code);
                        generated_count = generated_count + 1;
                        if (generation_budget > 0
                            && generated_count >= generation_budget)
                            return (FT_ERR_SUCCESS);
                    }
                }
                offset_x = offset_x + 1;
            }
            offset_z = offset_z + 1;
        }
        radius = radius + 1;
    }
    return (FT_ERR_SUCCESS);
}

bool ft_vox_world_validate_visible_distance(const ft_vox_world *world,
    double camera_x, double camera_z, double yaw, int32_t required_distance)
{
    double forward_x;
    double forward_z;
    double center_x;
    double center_z;
    double delta_x;
    double delta_z;
    double forward_distance;
    double best_distance;
    int32_t index;

    if (world == nullptr || required_distance <= 0)
        return (false);
    forward_x = std::sin(yaw);
    forward_z = std::cos(yaw);
    best_distance = 0.0;
    index = 0;
    while (index < world->chunk_count)
    {
        if (world->chunks[index].initialized == true)
        {
            center_x = static_cast<double>(world->chunks[index].world_x)
                + (static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5);
            center_z = static_cast<double>(world->chunks[index].world_z)
                + (static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5);
            delta_x = center_x - camera_x;
            delta_z = center_z - camera_z;
            forward_distance = (delta_x * forward_x) + (delta_z * forward_z);
            if (forward_distance > best_distance)
                best_distance = forward_distance;
        }
        index = index + 1;
    }
    return (best_distance >= static_cast<double>(required_distance));
}

static int32_t ft_vox_world_remesh_chunk(ft_vox_world *world,
    int32_t chunk_x, int32_t chunk_z)
{
    ft_vox_world_chunk *world_chunk;
    int32_t error_code;

    world_chunk = ft_vox_find_chunk_mutable(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_SUCCESS);
    error_code = chunk_mesh_clear(world_chunk->mesh);
    if (error_code == FT_ERR_SUCCESS)
        error_code = chunk_mesh_generate_from_chunk(world_chunk->mesh,
            world_chunk->chunk);
    return (error_code);
}

static int32_t ft_vox_world_remesh_edited_chunk_border(ft_vox_world *world,
    int32_t chunk_x, int32_t chunk_z, int32_t local_x, int32_t local_z)
{
    int32_t error_code;

    error_code = ft_vox_world_remesh_chunk(world, chunk_x, chunk_z);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (local_x == 0)
        error_code = ft_vox_world_remesh_chunk(world, chunk_x - 1, chunk_z);
    if (error_code == FT_ERR_SUCCESS && local_x == GAME_VOXEL_CHUNK_WIDTH - 1)
        error_code = ft_vox_world_remesh_chunk(world, chunk_x + 1, chunk_z);
    if (error_code == FT_ERR_SUCCESS && local_z == 0)
        error_code = ft_vox_world_remesh_chunk(world, chunk_x, chunk_z - 1);
    if (error_code == FT_ERR_SUCCESS && local_z == GAME_VOXEL_CHUNK_DEPTH - 1)
        error_code = ft_vox_world_remesh_chunk(world, chunk_x, chunk_z + 1);
    return (error_code);
}

bool ft_vox_world_surface_top_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_z, double *surface_top)
{
    const ft_vox_world_chunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t local_y;
    uint32_t block_id;

    if (world == nullptr || surface_top == nullptr)
        return (false);
    chunk_x = ft_vox_floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = ft_vox_floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = ft_vox_find_chunk(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = ft_vox_positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = ft_vox_positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    local_y = GAME_VOXEL_CHUNK_HEIGHT - 1;
    while (local_y >= 0)
    {
        if (world_chunk->chunk.read_block(local_x, local_y, local_z,
                &block_id) == FT_ERR_SUCCESS
            && block_id != GAME_VOXEL_AIR_BLOCK
            && terrain_block_is_solid(block_id) == FT_TRUE)
        {
            *surface_top = static_cast<double>(local_y + 1);
            return (true);
        }
        local_y = local_y - 1;
    }
    return (false);
}

bool ft_vox_world_solid_block_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z)
{
    const ft_vox_world_chunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    uint32_t block_id;

    if (world == nullptr || world_y < 0
        || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (false);
    chunk_x = ft_vox_floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = ft_vox_floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = ft_vox_find_chunk(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = ft_vox_positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = ft_vox_positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    if (world_chunk->chunk.read_block(local_x, world_y, local_z,
            &block_id) != FT_ERR_SUCCESS)
        return (false);
    return (block_id != GAME_VOXEL_AIR_BLOCK
        && terrain_block_is_solid(block_id) == FT_TRUE);
}

bool ft_vox_world_block_id_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z, uint32_t *block_id)
{
    const ft_vox_world_chunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;

    if (world == nullptr || block_id == nullptr || world_y < 0
        || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (false);
    chunk_x = ft_vox_floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = ft_vox_floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = ft_vox_find_chunk(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = ft_vox_positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = ft_vox_positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    return (world_chunk->chunk.read_block(local_x, world_y, local_z,
        block_id) == FT_ERR_SUCCESS);
}

int32_t ft_vox_world_delete_block_at(ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z)
{
    ft_vox_world_chunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t error_code;

    if (world == nullptr || world_y < 0
        || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (FT_ERR_INVALID_ARGUMENT);
    chunk_x = ft_vox_floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = ft_vox_floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = ft_vox_find_chunk_mutable(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    local_x = ft_vox_positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = ft_vox_positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    error_code = world_chunk->chunk.write_block(local_x, world_y, local_z,
        GAME_VOXEL_AIR_BLOCK);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    return (ft_vox_world_remesh_edited_chunk_border(world, chunk_x, chunk_z,
        local_x, local_z));
}

int32_t ft_vox_world_place_block_at(ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z, uint32_t block_id)
{
    ft_vox_world_chunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t error_code;
    uint32_t current_block_id;

    if (world == nullptr || world_y < 0
        || world_y >= GAME_VOXEL_CHUNK_HEIGHT
        || block_id == GAME_VOXEL_AIR_BLOCK
        || terrain_block_is_known(block_id) != FT_TRUE)
        return (FT_ERR_INVALID_ARGUMENT);
    chunk_x = ft_vox_floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = ft_vox_floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = ft_vox_find_chunk_mutable(world, chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_NOT_FOUND);
    local_x = ft_vox_positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = ft_vox_positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    error_code = world_chunk->chunk.read_block(local_x, world_y, local_z,
        &current_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (current_block_id != GAME_VOXEL_AIR_BLOCK)
        return (FT_ERR_ALREADY_EXISTS);
    error_code = world_chunk->chunk.write_block(local_x, world_y, local_z,
        block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    return (ft_vox_world_remesh_edited_chunk_border(world, chunk_x, chunk_z,
        local_x, local_z));
}

int32_t ft_vox_world_raycast_solid(const ft_vox_world *world,
    double origin_x, double origin_y, double origin_z,
    double direction_x, double direction_y, double direction_z,
    double max_distance, int32_t *block_x, int32_t *block_y,
    int32_t *block_z)
{
    const double step_distance = 0.0625;
    double distance;
    double sample_x;
    double sample_y;
    double sample_z;
    int32_t test_x;
    int32_t test_y;
    int32_t test_z;
    uint32_t block_id;

    if (world == nullptr || block_x == nullptr || block_y == nullptr
        || block_z == nullptr || max_distance <= 0.0)
        return (FT_ERR_INVALID_ARGUMENT);
    distance = step_distance;
    while (distance <= max_distance)
    {
        sample_x = origin_x + (direction_x * distance);
        sample_y = origin_y + (direction_y * distance);
        sample_z = origin_z + (direction_z * distance);
        test_x = static_cast<int32_t>(std::floor(sample_x));
        test_y = static_cast<int32_t>(std::floor(sample_y));
        test_z = static_cast<int32_t>(std::floor(sample_z));
        if (ft_vox_world_block_id_at(world, test_x, test_y, test_z, &block_id)
            == true && block_id != GAME_VOXEL_AIR_BLOCK)
        {
            *block_x = test_x;
            *block_y = test_y;
            *block_z = test_z;
            return (FT_ERR_SUCCESS);
        }
        distance = distance + step_distance;
    }
    return (FT_ERR_INVALID_ARGUMENT);
}

int32_t ft_vox_world_raycast_edit_target(const ft_vox_world *world,
    double origin_x, double origin_y, double origin_z,
    double direction_x, double direction_y, double direction_z,
    double max_distance, int32_t *hit_block_x, int32_t *hit_block_y,
    int32_t *hit_block_z, int32_t *place_block_x, int32_t *place_block_y,
    int32_t *place_block_z, uint32_t *hit_block_id)
{
    const double step_distance = 0.0625;
    double distance;
    double sample_x;
    double sample_y;
    double sample_z;
    int32_t test_x;
    int32_t test_y;
    int32_t test_z;
    int32_t previous_x;
    int32_t previous_y;
    int32_t previous_z;
    uint32_t block_id;

    if (world == nullptr || hit_block_x == nullptr || hit_block_y == nullptr
        || hit_block_z == nullptr || place_block_x == nullptr
        || place_block_y == nullptr || place_block_z == nullptr
        || hit_block_id == nullptr || max_distance <= 0.0)
        return (FT_ERR_INVALID_ARGUMENT);
    previous_x = static_cast<int32_t>(std::floor(origin_x));
    previous_y = static_cast<int32_t>(std::floor(origin_y));
    previous_z = static_cast<int32_t>(std::floor(origin_z));
    distance = step_distance;
    while (distance <= max_distance)
    {
        sample_x = origin_x + (direction_x * distance);
        sample_y = origin_y + (direction_y * distance);
        sample_z = origin_z + (direction_z * distance);
        test_x = static_cast<int32_t>(std::floor(sample_x));
        test_y = static_cast<int32_t>(std::floor(sample_y));
        test_z = static_cast<int32_t>(std::floor(sample_z));
        if (test_x == previous_x && test_y == previous_y
            && test_z == previous_z)
        {
            distance = distance + step_distance;
            continue ;
        }
        if (ft_vox_world_block_id_at(world, test_x, test_y, test_z, &block_id)
            == true && block_id != GAME_VOXEL_AIR_BLOCK)
        {
            *hit_block_x = test_x;
            *hit_block_y = test_y;
            *hit_block_z = test_z;
            *place_block_x = previous_x;
            *place_block_y = previous_y;
            *place_block_z = previous_z;
            *hit_block_id = block_id;
            return (FT_ERR_SUCCESS);
        }
        previous_x = test_x;
        previous_y = test_y;
        previous_z = test_z;
        distance = distance + step_distance;
    }
    return (FT_ERR_NOT_FOUND);
}
