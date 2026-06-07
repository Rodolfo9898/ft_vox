#include "../../src/queries/WorldBlockQuery.hpp"

WorldBlockQuery::WorldBlockQuery()
{
}

WorldBlockQuery::WorldBlockQuery(const WorldBlockQuery &other)
{
    *this = other;
}

WorldBlockQuery::~WorldBlockQuery()
{
}

WorldBlockQuery &WorldBlockQuery::operator=(const WorldBlockQuery &other)
{
    (void)other;
    return (*this);
}

bool WorldBlockQuery::surface_top_at(const World &world, int32_t world_x,
    int32_t world_z, double *surface_top)
{
    const WorldChunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t local_y;
    uint32_t block_id;

    if (surface_top == nullptr)
        return (false);
    chunk_x = WorldCoordinates::floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = WorldCoordinates::floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = world.find_chunk(chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = WorldCoordinates::positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = WorldCoordinates::positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
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

bool WorldBlockQuery::solid_block_at(const World &world, int32_t world_x,
    int32_t world_y, int32_t world_z)
{
    const WorldChunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    uint32_t block_id;

    if (world_y < 0 || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (false);
    chunk_x = WorldCoordinates::floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = WorldCoordinates::floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = world.find_chunk(chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = WorldCoordinates::positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = WorldCoordinates::positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    if (world_chunk->chunk.read_block(local_x, world_y, local_z,
            &block_id) != FT_ERR_SUCCESS)
        return (false);
    return (block_id != GAME_VOXEL_AIR_BLOCK
        && terrain_block_is_solid(block_id) == FT_TRUE);
}

bool WorldBlockQuery::block_id_at(const World &world, int32_t world_x,
    int32_t world_y, int32_t world_z, uint32_t *block_id)
{
    const WorldChunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;

    if (block_id == nullptr || world_y < 0 || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (false);
    chunk_x = WorldCoordinates::floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = WorldCoordinates::floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = world.find_chunk(chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (false);
    local_x = WorldCoordinates::positive_modulo(world_x, GAME_VOXEL_CHUNK_WIDTH);
    local_z = WorldCoordinates::positive_modulo(world_z, GAME_VOXEL_CHUNK_DEPTH);
    return (world_chunk->chunk.read_block(local_x, world_y, local_z,
        block_id) == FT_ERR_SUCCESS);
}
