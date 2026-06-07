#ifndef WORLD_BLOCK_EDITOR_HPP
# define WORLD_BLOCK_EDITOR_HPP

#include "../../src/world/World.hpp"

#include "../../src/queries/WorldBlockQuery.hpp"
#include "../../src/edits/DeleteBlockCommand.hpp"
#include "../../src/edits/PlaceBlockCommand.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

class World;

class WorldBlockEditor
{
    public:
        WorldBlockEditor();
        WorldBlockEditor(const WorldBlockEditor &other);
        ~WorldBlockEditor();
        WorldBlockEditor &operator=(const WorldBlockEditor &other);

        static int32_t delete_block_at(World &world, int32_t world_x,
            int32_t world_y, int32_t world_z);
        static int32_t place_block_at(World &world, int32_t world_x,
            int32_t world_y, int32_t world_z, uint32_t block_id);
};

#endif
