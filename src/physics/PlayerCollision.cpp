#include "../../src/physics/PlayerCollision.hpp"

PlayerCollision::PlayerCollision()
{
}

PlayerCollision::PlayerCollision(const PlayerCollision &other)
{
    *this = other;
}

PlayerCollision::~PlayerCollision()
{
}

PlayerCollision &PlayerCollision::operator=(const PlayerCollision &other)
{
    (void)other;
    return (*this);
}

bool PlayerCollision::player_box_intersects_world(const World &world,
    double x, double eye_y, double z)
{
    const double foot_clearance = 0.05;
    const double head_clearance = 0.05;
    const double side_clearance = 0.02;
    double feet_y;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
    int32_t min_z;
    int32_t max_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;

    feet_y = eye_y - PlayerGeometry::PLAYER_EYE_HEIGHT;
    min_x = static_cast<int32_t>(std::floor(x - PlayerGeometry::PLAYER_HALF_WIDTH
        + side_clearance));
    max_x = static_cast<int32_t>(std::floor(x + PlayerGeometry::PLAYER_HALF_WIDTH
        - side_clearance));
    min_y = static_cast<int32_t>(std::floor(feet_y + foot_clearance));
    max_y = static_cast<int32_t>(std::floor(feet_y + PlayerGeometry::PLAYER_HEIGHT
        - head_clearance));
    min_z = static_cast<int32_t>(std::floor(z - PlayerGeometry::PLAYER_HALF_WIDTH
        + side_clearance));
    max_z = static_cast<int32_t>(std::floor(z + PlayerGeometry::PLAYER_HALF_WIDTH
        - side_clearance));
    block_y = min_y;
    while (block_y <= max_y)
    {
        block_z = min_z;
        while (block_z <= max_z)
        {
            block_x = min_x;
            while (block_x <= max_x)
            {
                if (world.solid_block_at(block_x, block_y,
                        block_z) == true)
                    return (true);
                block_x = block_x + 1;
            }
            block_z = block_z + 1;
        }
        block_y = block_y + 1;
    }
    return (false);
}

bool PlayerCollision::move_player_axis(Camera *camera,
    const World &world, int axis, double delta)
{
    const double collision_epsilon = 0.001;
    double next_x;
    double next_y;
    double next_z;
    double candidate;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
    int32_t min_z;
    int32_t max_z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
    bool collided;

    if (camera == nullptr || std::fabs(delta) < 0.000001)
        return (false);
    next_x = camera->x;
    next_y = camera->y;
    next_z = camera->z;
    if (axis == 0)
        next_x = next_x + delta;
    else if (axis == 1)
        next_y = next_y + delta;
    else
        next_z = next_z + delta;
    collided = false;
    PlayerGeometry::player_block_range(next_x, next_y, next_z, &min_x, &max_x,
        &min_y, &max_y, &min_z, &max_z);
    block_y = min_y;
    while (block_y <= max_y)
    {
        block_z = min_z;
        while (block_z <= max_z)
        {
            block_x = min_x;
            while (block_x <= max_x)
            {
                if (world.solid_block_at(block_x, block_y,
                        block_z) == true)
                {
                    collided = true;
                    if (axis == 0 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_x)
                            - PlayerGeometry::PLAYER_HALF_WIDTH - collision_epsilon;
                        if (candidate < next_x)
                            next_x = candidate;
                    }
                    else if (axis == 0)
                    {
                        candidate = static_cast<double>(block_x + 1)
                            + PlayerGeometry::PLAYER_HALF_WIDTH + collision_epsilon;
                        if (candidate > next_x)
                            next_x = candidate;
                    }
                    else if (axis == 1 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_y)
                            - PlayerGeometry::PLAYER_HEIGHT + PlayerGeometry::PLAYER_EYE_HEIGHT
                            - collision_epsilon;
                        if (candidate < next_y)
                            next_y = candidate;
                    }
                    else if (axis == 1)
                    {
                        candidate = static_cast<double>(block_y + 1)
                            + PlayerGeometry::PLAYER_EYE_HEIGHT + collision_epsilon;
                        if (candidate > next_y)
                            next_y = candidate;
                    }
                    else if (axis == 2 && delta > 0.0)
                    {
                        candidate = static_cast<double>(block_z)
                            - PlayerGeometry::PLAYER_HALF_WIDTH - collision_epsilon;
                        if (candidate < next_z)
                            next_z = candidate;
                    }
                    else
                    {
                        candidate = static_cast<double>(block_z + 1)
                            + PlayerGeometry::PLAYER_HALF_WIDTH + collision_epsilon;
                        if (candidate > next_z)
                            next_z = candidate;
                    }
                }
                block_x = block_x + 1;
            }
            block_z = block_z + 1;
        }
        block_y = block_y + 1;
    }
    camera->x = next_x;
    camera->y = next_y;
    camera->z = next_z;
    return (collided);
}

