#include "../../src/coordinates/WorldCoordinates.hpp"

WorldCoordinates::WorldCoordinates()
{
}

WorldCoordinates::WorldCoordinates(const WorldCoordinates &other)
{
    *this = other;
}

WorldCoordinates::~WorldCoordinates()
{
}

WorldCoordinates &WorldCoordinates::operator=(const WorldCoordinates &other)
{
    (void)other;
    return (*this);
}

int32_t WorldCoordinates::floor_divide(int32_t value, int32_t divisor)
{
    if (value >= 0)
        return (value / divisor);
    return (-(((-value) + divisor - 1) / divisor));
}

int32_t WorldCoordinates::positive_modulo(int32_t value, int32_t divisor)
{
    int32_t result;

    result = value % divisor;
    if (result < 0)
        result += divisor;
    return (result);
}

int32_t WorldCoordinates::clamp_int(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum)
        return (minimum);
    if (value > maximum)
        return (maximum);
    return (value);
}

int32_t WorldCoordinates::chunk_distance_squared(int32_t chunk_x, int32_t chunk_z,
    int32_t center_chunk_x, int32_t center_chunk_z)
{
    int32_t delta_x;
    int32_t delta_z;

    delta_x = chunk_x - center_chunk_x;
    delta_z = chunk_z - center_chunk_z;
    return ((delta_x * delta_x) + (delta_z * delta_z));
}

int32_t WorldCoordinates::render_distance_to_chunk_radius(int32_t render_distance)
{
    int32_t clamped_distance;
    int32_t chunk_radius;

    clamped_distance = WorldCoordinates::clamp_int(render_distance,
        WORLD_MIN_RENDER_DISTANCE,
        WORLD_CACHE_CHUNK_RADIUS * GAME_VOXEL_CHUNK_WIDTH);
    chunk_radius = (clamped_distance + GAME_VOXEL_CHUNK_WIDTH - 1)
        / GAME_VOXEL_CHUNK_WIDTH;
    if (chunk_radius < 1)
        chunk_radius = 1;
    if (chunk_radius > WORLD_CACHE_CHUNK_RADIUS)
        chunk_radius = WORLD_CACHE_CHUNK_RADIUS;
    return (chunk_radius);
}
