#include "../../src/queries/WorldRaycaster.hpp"

WorldRaycaster::WorldRaycaster()
{
}

WorldRaycaster::WorldRaycaster(const WorldRaycaster &other)
{
    *this = other;
}

WorldRaycaster::~WorldRaycaster()
{
}

WorldRaycaster &WorldRaycaster::operator=(const WorldRaycaster &other)
{
    (void)other;
    return (*this);
}

int32_t WorldRaycaster::raycast_solid(const World &world, double origin_x, double origin_y, double origin_z,
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

    if (block_x == nullptr || block_y == nullptr || block_z == nullptr
        || max_distance <= 0.0)
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
        if (WorldBlockQuery::block_id_at(world, test_x, test_y, test_z, &block_id) == true
            && block_id != GAME_VOXEL_AIR_BLOCK)
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

int32_t WorldRaycaster::raycast_edit_target(const World &world, double origin_x, double origin_y,
    double origin_z, double direction_x, double direction_y,
    double direction_z, double max_distance, int32_t *hit_block_x,
    int32_t *hit_block_y, int32_t *hit_block_z, int32_t *place_block_x,
    int32_t *place_block_y, int32_t *place_block_z,
    uint32_t *hit_block_id)
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

    if (hit_block_x == nullptr || hit_block_y == nullptr
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
        if (WorldBlockQuery::block_id_at(world, test_x, test_y, test_z, &block_id) == true
            && block_id != GAME_VOXEL_AIR_BLOCK)
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
