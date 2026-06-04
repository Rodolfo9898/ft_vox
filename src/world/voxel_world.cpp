#include "voxel_world.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"

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
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t index;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    world->chunk_count = 0;
    index = 0;
    chunk_z = -FT_VOX_WORLD_CHUNK_RADIUS;
    while (chunk_z <= FT_VOX_WORLD_CHUNK_RADIUS)
    {
        chunk_x = -FT_VOX_WORLD_CHUNK_RADIUS;
        while (chunk_x <= FT_VOX_WORLD_CHUNK_RADIUS)
        {
            error_code = ft_vox_initialize_chunk(&world->chunks[index],
                chunk_x, chunk_z, seed);
            if (error_code != FT_ERR_SUCCESS)
            {
                world->chunk_count = index;
                ft_vox_world_destroy(world);
                return (error_code);
            }
            index = index + 1;
            chunk_x = chunk_x + 1;
        }
        chunk_z = chunk_z + 1;
    }
    world->chunk_count = index;
    return (FT_ERR_SUCCESS);
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
            (void)chunk_mesh_destroy(world->chunks[index].mesh);
            (void)world->chunks[index].chunk.destroy();
            world->chunks[index].initialized = false;
        }
        index = index + 1;
    }
    world->chunk_count = 0;
    return ;
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
    error_code = chunk_mesh_clear(world_chunk->mesh);
    if (error_code == FT_ERR_SUCCESS)
        error_code = chunk_mesh_generate_from_chunk(world_chunk->mesh,
            world_chunk->chunk);
    return (error_code);
}
