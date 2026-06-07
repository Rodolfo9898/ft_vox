#include "../../src/validators/BlockEditValidator.hpp"

BlockEditValidator::BlockEditValidator()
{
}

BlockEditValidator::BlockEditValidator(const BlockEditValidator &other)
{
    *this = other;
}

BlockEditValidator::~BlockEditValidator()
{
}

BlockEditValidator &BlockEditValidator::operator=(const BlockEditValidator &other)
{
    (void)other;
    return (*this);
}

size_t BlockEditValidator::mesh_index_count_for_block(const World &world,
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

int BlockEditValidator::validate()
{
    World world;
    double surface_top;
    int32_t edit_x;
    int32_t edit_y;
    int32_t edit_z;
    uint32_t block_id;
    size_t mesh_before;
    size_t mesh_after_place;
    size_t mesh_after_delete;
    int32_t error_code;

    error_code = world.initialize("integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("block-edit world initialization", error_code));
    edit_x = 2;
    edit_z = -6;
    if (world.surface_top_at(edit_x, edit_z,
            &surface_top) == false)
    {
        world.destroy();
        std::fprintf(stderr, "block-edit: failed surface lookup\n");
        return (1);
    }
    edit_y = static_cast<int32_t>(std::floor(surface_top + 1.0));
    mesh_before = mesh_index_count_for_block(world, edit_x, edit_z);
    error_code = world.place_block_at(edit_x, edit_y, edit_z,
        TERRAIN_GENERATOR_STONE_BLOCK);
    if (error_code != FT_ERR_SUCCESS)
    {
        world.destroy();
        return (ApplicationError::fail("block-edit place", error_code));
    }
    if (world.block_id_at(edit_x, edit_y, edit_z,
            &block_id) == false || block_id != TERRAIN_GENERATOR_STONE_BLOCK)
    {
        world.destroy();
        std::fprintf(stderr, "block-edit: placed block not visible in storage\n");
        return (1);
    }
    mesh_after_place = mesh_index_count_for_block(world, edit_x,
        edit_z);
    error_code = world.delete_block_at(edit_x, edit_y,
        edit_z);
    if (error_code != FT_ERR_SUCCESS)
    {
        world.destroy();
        return (ApplicationError::fail("block-edit delete", error_code));
    }
    if (world.block_id_at(edit_x, edit_y, edit_z,
            &block_id) == false || block_id != GAME_VOXEL_AIR_BLOCK)
    {
        world.destroy();
        std::fprintf(stderr, "block-edit: deleted block not visible in storage\n");
        return (1);
    }
    mesh_after_delete = mesh_index_count_for_block(world, edit_x,
        edit_z);
    if (mesh_after_place == mesh_before || mesh_after_delete != mesh_before)
    {
        world.destroy();
        std::fprintf(stderr,
            "block-edit: mesh counts did not update before=%zu place=%zu delete=%zu\n",
            mesh_before, mesh_after_place, mesh_after_delete);
        return (1);
    }
    std::printf("block-edit: ok before=%zu place=%zu delete=%zu\n",
        mesh_before, mesh_after_place, mesh_after_delete);
    world.destroy();
    return (0);
}
