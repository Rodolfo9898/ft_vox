#include "../../src/edits/DeleteBlockCommand.hpp"

DeleteBlockCommand::DeleteBlockCommand()
{
    this->world_x = 0;
    this->world_y = 0;
    this->world_z = 0;
}

DeleteBlockCommand::DeleteBlockCommand(int32_t block_x, int32_t block_y,
    int32_t block_z)
{
    this->world_x = block_x;
    this->world_y = block_y;
    this->world_z = block_z;
}

DeleteBlockCommand::DeleteBlockCommand(const DeleteBlockCommand &other)
    : WorldEditCommand(other)
{
    *this = other;
}

DeleteBlockCommand::~DeleteBlockCommand()
{
}

DeleteBlockCommand &DeleteBlockCommand::operator=(
    const DeleteBlockCommand &other)
{
    if (this != &other)
    {
        this->world_x = other.world_x;
        this->world_y = other.world_y;
        this->world_z = other.world_z;
    }
    return (*this);
}

int32_t DeleteBlockCommand::execute(World &world) const
{
    WorldChunk *world_chunk;
    int32_t chunk_x;
    int32_t chunk_z;
    int32_t local_x;
    int32_t local_z;
    int32_t error_code;

    if (this->world_y < 0 || this->world_y >= GAME_VOXEL_CHUNK_HEIGHT)
        return (FT_ERR_INVALID_ARGUMENT);
    chunk_x = WorldCoordinates::floor_divide(this->world_x,
        GAME_VOXEL_CHUNK_WIDTH);
    chunk_z = WorldCoordinates::floor_divide(this->world_z,
        GAME_VOXEL_CHUNK_DEPTH);
    world_chunk = world.find_chunk_mutable(chunk_x, chunk_z);
    if (world_chunk == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    local_x = WorldCoordinates::positive_modulo(this->world_x,
        GAME_VOXEL_CHUNK_WIDTH);
    local_z = WorldCoordinates::positive_modulo(this->world_z,
        GAME_VOXEL_CHUNK_DEPTH);
    error_code = world_chunk->chunk.write_block(local_x, this->world_y,
        local_z, GAME_VOXEL_AIR_BLOCK);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    return (WorldChunkLoader::remesh_edited_chunk_border(world.chunks,
        world.chunk_count, chunk_x, chunk_z, local_x, local_z));
}
