#ifndef WORLD_RAYCASTER_HPP
# define WORLD_RAYCASTER_HPP

#include "../../src/world/World.hpp"

#include "../../src/queries/WorldBlockQuery.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cmath>
#include <cstdint>

class World;

class WorldRaycaster
{
    public:
        WorldRaycaster();
        WorldRaycaster(const WorldRaycaster &other);
        ~WorldRaycaster();
        WorldRaycaster &operator=(const WorldRaycaster &other);

        static int32_t raycast_solid(const World &world, double origin_x,
            double origin_y, double origin_z, double direction_x,
            double direction_y, double direction_z, double max_distance,
            int32_t *block_x, int32_t *block_y, int32_t *block_z);
        static int32_t raycast_edit_target(const World &world, double origin_x,
            double origin_y, double origin_z, double direction_x,
            double direction_y, double direction_z, double max_distance,
            int32_t *hit_block_x, int32_t *hit_block_y,
            int32_t *hit_block_z, int32_t *place_block_x,
            int32_t *place_block_y, int32_t *place_block_z,
            uint32_t *hit_block_id);
};

#endif
