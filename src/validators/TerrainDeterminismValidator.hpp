#ifndef TERRAIN_DETERMINISM_VALIDATOR_HPP
# define TERRAIN_DETERMINISM_VALIDATOR_HPP

#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>
#include <cstdio>

class TerrainDeterminismValidator
{
    private:
        static int compare_chunks(const game_voxel_chunk &left,
            const game_voxel_chunk &right);
        static int generate_pair(int32_t world_x, int32_t world_z,
            const char *seed);

    public:
        TerrainDeterminismValidator();
        TerrainDeterminismValidator(const TerrainDeterminismValidator &other);
        ~TerrainDeterminismValidator();
        TerrainDeterminismValidator &operator=(
            const TerrainDeterminismValidator &other);

        static int validate();
};

#endif
