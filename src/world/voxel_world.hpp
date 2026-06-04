#ifndef FT_VOX_VOXEL_WORLD_HPP
# define FT_VOX_VOXEL_WORLD_HPP

#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstdint>

#define FT_VOX_WORLD_CHUNK_RADIUS 3
#define FT_VOX_WORLD_CHUNK_COUNT \
    ((FT_VOX_WORLD_CHUNK_RADIUS * 2 + 1) * (FT_VOX_WORLD_CHUNK_RADIUS * 2 + 1))

struct ft_vox_world_chunk
{
    int32_t           chunk_x;
    int32_t           chunk_z;
    int32_t           world_x;
    int32_t           world_z;
    game_voxel_chunk  chunk;
    chunk_mesh        mesh;
    bool              initialized;
};

struct ft_vox_world
{
    ft_vox_world_chunk chunks[FT_VOX_WORLD_CHUNK_COUNT];
    int32_t            chunk_count;
};

int32_t ft_vox_world_initialize(ft_vox_world *world, const char *seed);
void    ft_vox_world_destroy(ft_vox_world *world);
bool    ft_vox_world_surface_top_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_z, double *surface_top);
bool    ft_vox_world_solid_block_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z);
int32_t ft_vox_world_delete_block_at(ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z);

#endif
