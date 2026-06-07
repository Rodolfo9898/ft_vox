#ifndef PLACE_BLOCK_COMMAND_HPP
# define PLACE_BLOCK_COMMAND_HPP

#include "../../src/world/World.hpp"

#include "../../src/edits/WorldEditCommand.hpp"
#include "../../src/chunks/WorldChunkLoader.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

class PlaceBlockCommand : public WorldEditCommand
{
    private:
        int32_t world_x;
        int32_t world_y;
        int32_t world_z;
        uint32_t block_id;

    public:
        PlaceBlockCommand();
        PlaceBlockCommand(int32_t block_x, int32_t block_y, int32_t block_z,
            uint32_t selected_block_id);
        PlaceBlockCommand(const PlaceBlockCommand &other);
        ~PlaceBlockCommand() override;
        PlaceBlockCommand &operator=(const PlaceBlockCommand &other);

        int32_t execute(World &world) const override;
};

#endif
