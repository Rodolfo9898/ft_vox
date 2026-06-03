#include "voxel_world.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"

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
