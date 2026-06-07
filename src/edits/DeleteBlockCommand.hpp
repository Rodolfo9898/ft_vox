#ifndef DELETE_BLOCK_COMMAND_HPP
# define DELETE_BLOCK_COMMAND_HPP

#include "../../src/world/World.hpp"

#include "../../src/edits/WorldEditCommand.hpp"
#include "../../src/chunks/WorldChunkLoader.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

class DeleteBlockCommand : public WorldEditCommand
{
    private:
        int32_t world_x;
        int32_t world_y;
        int32_t world_z;

    public:
        DeleteBlockCommand();
        DeleteBlockCommand(int32_t block_x, int32_t block_y,
            int32_t block_z);
        DeleteBlockCommand(const DeleteBlockCommand &other);
        ~DeleteBlockCommand() override;
        DeleteBlockCommand &operator=(const DeleteBlockCommand &other);

        int32_t execute(World &world) const override;
};

#endif
