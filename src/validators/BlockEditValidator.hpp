#ifndef BLOCK_EDIT_VALIDATOR_HPP
# define BLOCK_EDIT_VALIDATOR_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/world/World.hpp"
#include "../../src/coordinates/WorldCoordinates.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cmath>
#include <cstdio>
#include <cstddef>
#include <cstdint>

class BlockEditValidator
{
    private:
        static size_t mesh_index_count_for_block(const World &world,
            int32_t block_x, int32_t block_z);

    public:
        BlockEditValidator();
        BlockEditValidator(const BlockEditValidator &other);
        ~BlockEditValidator();
        BlockEditValidator &operator=(const BlockEditValidator &other);

        static int validate();
};

#endif
