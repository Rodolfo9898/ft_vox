#include "../../src/interaction/BlockInteractor.hpp"

BlockInteractor::BlockInteractor()
{
}

BlockInteractor::BlockInteractor(const BlockInteractor &other)
{
    *this = other;
}

BlockInteractor::~BlockInteractor()
{
}

BlockInteractor &BlockInteractor::operator=(const BlockInteractor &other)
{
    (void)other;
    return (*this);
}

int32_t BlockInteractor::try_delete_target_block(World *world,
    const Camera &camera)
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
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_solid(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &block_x,
        &block_y, &block_z);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    return (world->delete_block_at(block_x, block_y, block_z));
}

int32_t BlockInteractor::try_pick_target_block(const World *world,
    const Camera &camera, uint32_t *selected_block_id)
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
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_edit_target(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (hit_block_id != GAME_VOXEL_AIR_BLOCK
        && terrain_block_is_known(hit_block_id) == FT_TRUE)
        *selected_block_id = hit_block_id;
    return (FT_ERR_SUCCESS);
}

int32_t BlockInteractor::try_place_selected_block(World *world,
    const Camera &camera, uint32_t selected_block_id)
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
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_edit_target(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (PlayerController::block_overlaps_player(camera, place_x, place_y, place_z)
        == true)
        return (FT_ERR_SUCCESS);
    error_code = world->place_block_at(place_x, place_y, place_z,
        selected_block_id);
    if (error_code == FT_ERR_ALREADY_EXISTS || error_code == FT_ERR_NOT_FOUND)
        return (FT_ERR_SUCCESS);
    return (error_code);
}
