#ifndef TERRAIN_CAVE_VALIDATOR_HPP
# define TERRAIN_CAVE_VALIDATOR_HPP

#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>
#include <cstdio>

class TerrainCaveValidator
{
    private:
        static bool chunk_has_cave(const game_voxel_chunk &chunk);
        static int validate_origin(int32_t world_x, int32_t world_z,
            bool *found_cave);

    public:
        TerrainCaveValidator();
        TerrainCaveValidator(const TerrainCaveValidator &other);
        ~TerrainCaveValidator();
        TerrainCaveValidator &operator=(const TerrainCaveValidator &other);

        static int validate();
};

#endif
