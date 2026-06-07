#include "../../src/chunks/WorldChunkLoader.hpp"

namespace
{
    struct chunk_mesh_lookup_context
    {
        WorldChunk *chunks;
        int32_t chunk_count;
    };

    int32_t chunk_mesh_lookup_block(void *user_data, int32_t world_x,
        int32_t world_y, int32_t world_z, uint32_t *block_id) noexcept
    {
        chunk_mesh_lookup_context *context;
        const WorldChunk *world_chunk;
        int32_t chunk_x;
        int32_t chunk_z;
        int32_t local_x;
        int32_t local_y;
        int32_t local_z;

        if (user_data == nullptr || block_id == nullptr)
            return (FT_ERR_INVALID_ARGUMENT);
        context = static_cast<chunk_mesh_lookup_context *>(user_data);
        if (context->chunks == nullptr || context->chunk_count <= 0)
        {
            *block_id = GAME_VOXEL_AIR_BLOCK;
            return (FT_ERR_SUCCESS);
        }
        if (world_y < 0 || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        {
            *block_id = GAME_VOXEL_AIR_BLOCK;
            return (FT_ERR_SUCCESS);
        }
        chunk_x = WorldCoordinates::floor_divide(world_x,
            GAME_VOXEL_CHUNK_WIDTH);
        chunk_z = WorldCoordinates::floor_divide(world_z,
            GAME_VOXEL_CHUNK_DEPTH);
        world_chunk = WorldChunkStore::find_chunk(context->chunks,
            context->chunk_count, chunk_x, chunk_z);
        if (world_chunk == nullptr || world_chunk->initialized == false)
        {
            *block_id = GAME_VOXEL_AIR_BLOCK;
            return (FT_ERR_SUCCESS);
        }
        local_x = world_x - (chunk_x * GAME_VOXEL_CHUNK_WIDTH);
        local_y = world_y;
        local_z = world_z - (chunk_z * GAME_VOXEL_CHUNK_DEPTH);
        return (world_chunk->chunk.read_block(local_x, local_y, local_z,
            block_id));
    }
}

WorldChunkLoader::WorldChunkLoader()
{
}

WorldChunkLoader::WorldChunkLoader(const WorldChunkLoader &other)
{
    *this = other;
}

WorldChunkLoader::~WorldChunkLoader()
{
}

WorldChunkLoader &WorldChunkLoader::operator=(const WorldChunkLoader &other)
{
    (void)other;
    return (*this);
}

int32_t WorldChunkLoader::initialize_chunk(WorldChunk *world_chunk,
    int32_t chunk_x, int32_t chunk_z, const char *seed)
{
    int32_t error_code;

    if (world_chunk == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
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

int32_t WorldChunkLoader::initialize_chunk(WorldChunk *world_chunk,
    int32_t chunk_x, int32_t chunk_z, const char *seed, WorldChunk *chunks,
    int32_t chunk_count)
{
    int32_t error_code;
    chunk_mesh_lookup_context context;

    if (world_chunk == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
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
    {
        context.chunks = chunks;
        context.chunk_count = chunk_count;
        error_code = chunk_mesh_generate_from_chunk_with_neighbors(
            world_chunk->mesh, world_chunk->chunk, chunk_x, chunk_z,
            &chunk_mesh_lookup_block, &context);
    }
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)chunk_mesh_destroy(world_chunk->mesh);
        (void)world_chunk->chunk.destroy();
        return (error_code);
    }
    world_chunk->initialized = true;
    return (FT_ERR_SUCCESS);
}

int32_t WorldChunkLoader::generate_missing_chunk(WorldChunk *chunks,
    int32_t chunk_count, int32_t *loaded_chunk_count, int32_t chunk_x,
    int32_t chunk_z, const char *seed)
{
    WorldChunk *slot;
    int32_t error_code;

    if (WorldChunkStore::find_chunk(chunks, chunk_count, chunk_x, chunk_z)
        != nullptr)
        return (FT_ERR_SUCCESS);
    slot = WorldChunkStore::find_free_chunk_slot(chunks, chunk_count);
    if (slot == nullptr)
        return (FT_ERR_NO_MEMORY);
    error_code = initialize_chunk(slot, chunk_x, chunk_z, seed);
    if (error_code == FT_ERR_SUCCESS && loaded_chunk_count != nullptr)
        *loaded_chunk_count = *loaded_chunk_count + 1;
    return (error_code);
}

int32_t WorldChunkLoader::remesh_chunk(WorldChunk *chunks, int32_t chunk_count,
    int32_t chunk_x, int32_t chunk_z)
{
    WorldChunk *world_chunk;
    int32_t error_code;

    world_chunk = WorldChunkStore::find_chunk_mutable(chunks, chunk_count,
        chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_SUCCESS);
    error_code = chunk_mesh_clear(world_chunk->mesh);
    if (error_code == FT_ERR_SUCCESS)
        error_code = chunk_mesh_generate_from_chunk(world_chunk->mesh,
            world_chunk->chunk);
    return (error_code);
}

int32_t WorldChunkLoader::remesh_chunk(WorldChunk *chunks, int32_t chunk_count,
    int32_t chunk_x, int32_t chunk_z, bool use_neighbors)
{
    WorldChunk *world_chunk;
    int32_t error_code;
    chunk_mesh_lookup_context context;

    world_chunk = WorldChunkStore::find_chunk_mutable(chunks, chunk_count,
        chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_SUCCESS);
    error_code = chunk_mesh_clear(world_chunk->mesh);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (use_neighbors == false)
        return (chunk_mesh_generate_from_chunk(world_chunk->mesh,
            world_chunk->chunk));
    context.chunks = chunks;
    context.chunk_count = chunk_count;
    return (chunk_mesh_generate_from_chunk_with_neighbors(world_chunk->mesh,
        world_chunk->chunk, chunk_x, chunk_z, &chunk_mesh_lookup_block,
        &context));
}

int32_t WorldChunkLoader::remesh_edited_chunk_border(WorldChunk *chunks,
    int32_t chunk_count, int32_t chunk_x, int32_t chunk_z,
    int32_t local_x, int32_t local_z)
{
    int32_t error_code;

    error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z, true);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (local_x == 0)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x - 1, chunk_z,
            true);
    if (error_code == FT_ERR_SUCCESS && local_x == GAME_VOXEL_CHUNK_WIDTH - 1)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x + 1, chunk_z,
            true);
    if (error_code == FT_ERR_SUCCESS && local_z == 0)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z - 1,
            true);
    if (error_code == FT_ERR_SUCCESS && local_z == GAME_VOXEL_CHUNK_DEPTH - 1)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z + 1,
            true);
    return (error_code);
}
