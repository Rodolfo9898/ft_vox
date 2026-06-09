#include "../../src/interaction/BlockInteractor.hpp"

BlockInteractor::BlockInteractor()
{
}

BlockInteractor::BlockInteractor(const BlockInteractor &other)
{
    *this = other;
}

BlockInteractor::~BlockInteractor()
{
}

BlockInteractor &BlockInteractor::operator=(const BlockInteractor &other)
{
    (void)other;
    return (*this);
}

static void spread_water(World *world, int32_t sx, int32_t sy, int32_t sz)
{
    static const int32_t HOPS_MAX = 7;
    static const int32_t FILL_MAX = 150;
    static const int32_t BUF     = FILL_MAX + 16;

    struct Pos { int32_t x, y, z, hd; };
    Pos     q[BUF];
    int32_t vx[BUF], vy[BUF], vz[BUF];
    int32_t q_front = 0, q_back = 0, v_count = 0, fills = 0;

    const int32_t DX[4] = {1, -1, 0, 0};
    const int32_t DZ[4] = {0, 0, 1, -1};

    auto visited = [&](int32_t x, int32_t y, int32_t z) -> bool {
        for (int32_t i = 0; i < v_count; ++i)
            if (vx[i] == x && vy[i] == y && vz[i] == z) return true;
        return false;
    };
    auto mark = [&](int32_t x, int32_t y, int32_t z) {
        if (v_count < BUF - 1) { vx[v_count] = x; vy[v_count] = y; vz[v_count] = z; ++v_count; }
    };
    auto enqueue = [&](int32_t x, int32_t y, int32_t z, int32_t hd) {
        if (q_back < BUF - 1) q[q_back++] = {x, y, z, hd};
    };

    mark(sx, sy, sz);
    enqueue(sx, sy, sz, 0);

    while (q_front < q_back && fills < FILL_MAX)
    {
        Pos cur = q[q_front++];

        if (!world->solid_block_at(cur.x, cur.y - 1, cur.z))
        {
            uint32_t b = 0;
            world->block_id_at(cur.x, cur.y - 1, cur.z, &b);
            if (b == GAME_VOXEL_AIR_BLOCK && !visited(cur.x, cur.y - 1, cur.z))
            {
                if (world->place_block_at(cur.x, cur.y - 1, cur.z,
                        TERRAIN_GENERATOR_WATER_BLOCK) == FT_ERR_SUCCESS)
                {
                    ++fills;
                    mark(cur.x, cur.y - 1, cur.z);
                    enqueue(cur.x, cur.y - 1, cur.z, 0);
                }
            }
        }

        if (cur.hd < HOPS_MAX)
        {
            for (int d = 0; d < 4; ++d)
            {
                int32_t nx = cur.x + DX[d];
                int32_t nz = cur.z + DZ[d];
                if (visited(nx, cur.y, nz)) continue;
                uint32_t b = 0;
                world->block_id_at(nx, cur.y, nz, &b);
                if (b == GAME_VOXEL_AIR_BLOCK)
                {
                    if (world->place_block_at(nx, cur.y, nz,
                            TERRAIN_GENERATOR_WATER_BLOCK) == FT_ERR_SUCCESS)
                    {
                        ++fills;
                        mark(nx, cur.y, nz);
                        enqueue(nx, cur.y, nz, cur.hd + 1);
                    }
                }
            }
        }
    }
}

int32_t BlockInteractor::try_delete_target_block(World *world,
    const Camera &camera)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
    int32_t error_code;

    if (world == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_solid(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &block_x,
        &block_y, &block_z);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    {
        DeleteBlockCommand cmd(block_x, block_y, block_z);
        return (cmd.execute(*world));
    }
}

int32_t BlockInteractor::try_pick_target_block(const World *world,
    const Camera &camera, uint32_t *selected_block_id)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t hit_x;
    int32_t hit_y;
    int32_t hit_z;
    int32_t place_x;
    int32_t place_y;
    int32_t place_z;
    uint32_t hit_block_id;
    int32_t error_code;

    if (world == nullptr || selected_block_id == nullptr)
        return (FT_ERR_INVALID_ARGUMENT);
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_edit_target(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (hit_block_id != GAME_VOXEL_AIR_BLOCK
        && terrain_block_is_known(hit_block_id) == FT_TRUE)
        *selected_block_id = hit_block_id;
    return (FT_ERR_SUCCESS);
}

int32_t BlockInteractor::try_place_selected_block(World *world,
    const Camera &camera, uint32_t selected_block_id)
{
    double direction_x;
    double direction_y;
    double direction_z;
    int32_t hit_x;
    int32_t hit_y;
    int32_t hit_z;
    int32_t place_x;
    int32_t place_y;
    int32_t place_z;
    uint32_t hit_block_id;
    int32_t error_code;

    if (world == nullptr || selected_block_id == GAME_VOXEL_AIR_BLOCK)
        return (FT_ERR_INVALID_ARGUMENT);
    PlayerController::camera_forward(camera, &direction_x, &direction_y, &direction_z);
    error_code = world->raycast_edit_target(camera.x, camera.y,
        camera.z, direction_x, direction_y, direction_z, 10.0, &hit_x, &hit_y,
        &hit_z, &place_x, &place_y, &place_z, &hit_block_id);
    if (error_code != FT_ERR_SUCCESS)
        return (FT_ERR_SUCCESS);
    if (PlayerController::block_overlaps_player(camera, place_x, place_y, place_z)
        == true)
        return (FT_ERR_SUCCESS);
    {
        PlaceBlockCommand cmd(place_x, place_y, place_z, selected_block_id);
        error_code = cmd.execute(*world);
    }
    if (error_code == FT_ERR_ALREADY_EXISTS || error_code == FT_ERR_NOT_FOUND)
        return (FT_ERR_SUCCESS);
    if (error_code == FT_ERR_SUCCESS
        && selected_block_id == TERRAIN_GENERATOR_WATER_BLOCK)
        spread_water(world, place_x, place_y, place_z);
    return (error_code);
}
