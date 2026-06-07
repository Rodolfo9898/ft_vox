#include "../../src/validators/WorldVisibilityValidator.hpp"

WorldVisibilityValidator::WorldVisibilityValidator()
{
}

WorldVisibilityValidator::WorldVisibilityValidator(const WorldVisibilityValidator &other)
{
    *this = other;
}

WorldVisibilityValidator::~WorldVisibilityValidator()
{
}

WorldVisibilityValidator &WorldVisibilityValidator::operator=(const WorldVisibilityValidator &other)
{
    (void)other;
    return (*this);
}

bool WorldVisibilityValidator::validate_visible_distance(const World &world,
    double camera_x, double camera_z, double yaw, int32_t required_distance)
{
    double forward_x;
    double forward_z;
    double center_x;
    double center_z;
    double delta_x;
    double delta_z;
    double forward_distance;
    double best_distance;
    int32_t index;

    if (required_distance <= 0)
        return (false);
    forward_x = std::sin(yaw);
    forward_z = std::cos(yaw);
    best_distance = 0.0;
    index = 0;
    while (index < world.chunk_count)
    {
        if (world.chunks[index].initialized == true)
        {
            center_x = static_cast<double>(world.chunks[index].world_x)
                + (static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5);
            center_z = static_cast<double>(world.chunks[index].world_z)
                + (static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5);
            delta_x = center_x - camera_x;
            delta_z = center_z - camera_z;
            forward_distance = (delta_x * forward_x) + (delta_z * forward_z);
            if (forward_distance > best_distance)
                best_distance = forward_distance;
        }
        index = index + 1;
    }
    return (best_distance >= static_cast<double>(required_distance));
}
