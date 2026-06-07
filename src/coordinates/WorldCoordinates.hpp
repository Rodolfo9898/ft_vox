#ifndef WORLD_COORDINATES_HPP
# define WORLD_COORDINATES_HPP

#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

#define WORLD_STREAM_CHUNK_RADIUS 10
#define WORLD_CACHE_CHUNK_RADIUS 12
#define WORLD_CHUNK_RADIUS WORLD_CACHE_CHUNK_RADIUS
#define WORLD_CHUNK_COUNT     ((WORLD_CHUNK_RADIUS * 2 + 1) * (WORLD_CHUNK_RADIUS * 2 + 1))
#define WORLD_MIN_RENDER_DISTANCE 14
#define WORLD_REQUIRED_VISIBLE_DISTANCE 160

class WorldCoordinates
{
    public:
        WorldCoordinates();
        WorldCoordinates(const WorldCoordinates &other);
        ~WorldCoordinates();
        WorldCoordinates &operator=(const WorldCoordinates &other);

        static int32_t floor_divide(int32_t value, int32_t divisor);
        static int32_t positive_modulo(int32_t value, int32_t divisor);
        static int32_t clamp_int(int32_t value, int32_t minimum,
            int32_t maximum);
        static int32_t chunk_distance_squared(int32_t chunk_x,
            int32_t chunk_z, int32_t center_chunk_x, int32_t center_chunk_z);
        static int32_t render_distance_to_chunk_radius(int32_t render_distance);
};

#endif
