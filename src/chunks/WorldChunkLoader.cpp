#include "../../src/chunks/WorldChunkLoader.hpp"

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

int32_t WorldChunkLoader::remesh_edited_chunk_border(WorldChunk *chunks,
    int32_t chunk_count, int32_t chunk_x, int32_t chunk_z,
    int32_t local_x, int32_t local_z)
{
    int32_t error_code;

    error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (local_x == 0)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x - 1, chunk_z);
    if (error_code == FT_ERR_SUCCESS && local_x == GAME_VOXEL_CHUNK_WIDTH - 1)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x + 1, chunk_z);
    if (error_code == FT_ERR_SUCCESS && local_z == 0)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z - 1);
    if (error_code == FT_ERR_SUCCESS && local_z == GAME_VOXEL_CHUNK_DEPTH - 1)
        error_code = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z + 1);
    return (error_code);
}
