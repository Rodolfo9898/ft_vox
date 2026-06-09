#ifndef BLOCK_INTERACTOR_HPP
# define BLOCK_INTERACTOR_HPP

#include <cstddef>
#include "../../src/edits/PlaceBlockCommand.hpp"
#include "../../src/edits/DeleteBlockCommand.hpp"
#include "../../src/player/PlayerController.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

class BlockInteractor
{
    public:
        BlockInteractor();
        BlockInteractor(const BlockInteractor &other);
        ~BlockInteractor();
        BlockInteractor &operator=(const BlockInteractor &other);

        static int32_t try_delete_target_block(World *world,
            const Camera &camera);
        static int32_t try_pick_target_block(const World *world,
            const Camera &camera, uint32_t *selected_block_id);
        static int32_t try_place_selected_block(World *world,
            const Camera &camera, uint32_t selected_block_id);
};

#endif
