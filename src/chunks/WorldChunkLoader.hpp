#ifndef WORLD_CHUNK_LOADER_HPP
# define WORLD_CHUNK_LOADER_HPP

#include "../../src/chunks/WorldChunkStore.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cstdint>

class WorldChunkLoader
{
    public:
        WorldChunkLoader();
        WorldChunkLoader(const WorldChunkLoader &other);
        ~WorldChunkLoader();
        WorldChunkLoader &operator=(const WorldChunkLoader &other);

        static int32_t initialize_chunk(WorldChunk *world_chunk,
            int32_t chunk_x, int32_t chunk_z, const char *seed);
        static int32_t generate_missing_chunk(WorldChunk *chunks,
            int32_t chunk_count, int32_t *loaded_chunk_count,
            int32_t chunk_x, int32_t chunk_z, const char *seed);
        static int32_t remesh_chunk(WorldChunk *chunks, int32_t chunk_count,
            int32_t chunk_x, int32_t chunk_z);
        static int32_t remesh_edited_chunk_border(WorldChunk *chunks,
            int32_t chunk_count, int32_t chunk_x, int32_t chunk_z,
            int32_t local_x, int32_t local_z);
};

#endif
