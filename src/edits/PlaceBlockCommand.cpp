#include "../../src/edits/PlaceBlockCommand.hpp"

PlaceBlockCommand::PlaceBlockCommand()
{
    this->world_x = 0;
    this->world_y = 0;
    this->world_z = 0;
    this->block_id = GAME_VOXEL_AIR_BLOCK;
}

PlaceBlockCommand::PlaceBlockCommand(int32_t block_x, int32_t block_y,
    int32_t block_z, uint32_t selected_block_id)
{
    this->world_x = block_x;
    this->world_y = block_y;
    this->world_z = block_z;
    this->block_id = selected_block_id;
}

PlaceBlockCommand::PlaceBlockCommand(const PlaceBlockCommand &other)
    : WorldEditCommand(other)
{
    *this = other;
}

PlaceBlockCommand::~PlaceBlockCommand()
{
}

PlaceBlockCommand &PlaceBlockCommand::operator=(
    const PlaceBlockCommand &other)
{
    if (this != &other)
    {
        this->world_x = other.world_x;
        this->world_y = other.world_y;
        this->world_z = other.world_z;
        this->block_id = other.block_id;
    }
    return (*this);
}

int32_t PlaceBlockCommand::execute(World &world) const
{
    WorldChunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t error_code;
    uint32_t current_block_id;

    if (this->world_y < 0 || this->world_y >= GAME_VOXEL_CHUNK_HEIGHT
        || this->block_id == GAME_VOXEL_AIR_BLOCK
        || terrain_block_is_known(this->block_id) != FT_TRUE)
        return (FT_ERR_INVALID_ARGUMENT);
    chunk_x = WorldCoordinates::floor_divide(this->world_x,
        GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = WorldCoordinates::floor_divide(this->world_z,
        GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = world.find_chunk_mutable(chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_NOT_FOUND);
    local_x = WorldCoordinates::positive_modulo(this->world_x,
        GAME_VOXEL_CHUNK_WIDTH);
    local_z = WorldCoordinates::positive_modulo(this->world_z,
        GAME_VOXEL_CHUNK_DEPTH);
    error_code = world_chunk->chunk.read_block(local_x, this->world_y,
        local_z, &current_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    if (current_block_id != GAME_VOXEL_AIR_BLOCK)
        return (FT_ERR_ALREADY_EXISTS);
    error_code = world_chunk->chunk.write_block(local_x, this->world_y,
        local_z, this->block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    return (WorldChunkLoader::remesh_edited_chunk_border(world.chunks,
        world.chunk_count, chunk_x, chunk_z, local_x, local_z));
}
