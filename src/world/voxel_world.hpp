#ifndef FT_VOX_VOXEL_WORLD_HPP
# define FT_VOX_VOXEL_WORLD_HPP

#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstdint>

#define FT_VOX_WORLD_STREAM_CHUNK_RADIUS 10
#define FT_VOX_WORLD_CACHE_CHUNK_RADIUS 12
#define FT_VOX_WORLD_CHUNK_RADIUS FT_VOX_WORLD_CACHE_CHUNK_RADIUS
#define FT_VOX_WORLD_CHUNK_COUNT \
    ((FT_VOX_WORLD_CHUNK_RADIUS * 2 + 1) * (FT_VOX_WORLD_CHUNK_RADIUS * 2 + 1))
#define FT_VOX_WORLD_MIN_RENDER_DISTANCE 14
#define FT_VOX_WORLD_REQUIRED_VISIBLE_DISTANCE 160

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
    int32_t            loaded_chunk_count;
    int32_t            center_chunk_x;
    int32_t            center_chunk_z;
    int32_t            active_render_distance;
    char               seed[128];
};

int32_t ft_vox_world_initialize(ft_vox_world *world, const char *seed);
void    ft_vox_world_destroy(ft_vox_world *world);
int32_t ft_vox_world_update_around(ft_vox_world *world,
    double camera_x, double camera_z, int32_t generation_budget);
int32_t ft_vox_world_update_around(ft_vox_world *world,
    double camera_x, double camera_z, int32_t generation_budget,
    int32_t render_distance);
bool    ft_vox_world_validate_visible_distance(const ft_vox_world *world,
    double camera_x, double camera_z, double yaw, int32_t required_distance);
bool    ft_vox_world_surface_top_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_z, double *surface_top);
bool    ft_vox_world_solid_block_at(const ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z);
int32_t ft_vox_world_delete_block_at(ft_vox_world *world,
    int32_t world_x, int32_t world_y, int32_t world_z);
int32_t ft_vox_world_raycast_solid(const ft_vox_world *world,
    double origin_x, double origin_y, double origin_z,
    double direction_x, double direction_y, double direction_z,
    double max_distance, int32_t *block_x, int32_t *block_y,
    int32_t *block_z);

#endif
