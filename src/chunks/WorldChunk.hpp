#ifndef WORLD_CHUNK_HPP
# define WORLD_CHUNK_HPP

#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstdint>

class WorldChunk
{
    public:
        int32_t           chunk_x;
        int32_t           chunk_z;
        int32_t           world_x;
        int32_t           world_z;
        game_voxel_chunk  chunk;
        chunk_mesh        mesh;
        bool              initialized;

        WorldChunk();
        WorldChunk(const WorldChunk &other);
        ~WorldChunk();
        WorldChunk &operator=(const WorldChunk &other);

        void reset_coordinates();
        void destroy();
};

#endif
