#include "../../src/movement/PlayerMovement.hpp"
#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cmath>

PlayerMovement::PlayerMovement()
{
}

PlayerMovement::PlayerMovement(const PlayerMovement &other)
{
    *this = other;
}

PlayerMovement::~PlayerMovement()
{
}

PlayerMovement &PlayerMovement::operator=(const PlayerMovement &other)
{
    (void)other;
    return (*this);
}

bool PlayerMovement::player_in_water(const Camera &camera, const World &world)
{
    uint32_t blk;
    int32_t  ex;
    int32_t  ey;
    int32_t  ez;
    int32_t  fy;

    ex = static_cast<int32_t>(std::floor(camera.x));
    ey = static_cast<int32_t>(std::floor(camera.y - 0.1));
    ez = static_cast<int32_t>(std::floor(camera.z));
    blk = 0U;
    if (world.block_id_at(ex, ey, ez, &blk) && blk == TERRAIN_GENERATOR_WATER_BLOCK)
        return (true);
    fy = static_cast<int32_t>(std::floor(camera.y - 1.5));
    blk = 0U;
    return (world.block_id_at(ex, fy, ez, &blk) && blk == TERRAIN_GENERATOR_WATER_BLOCK);
}

void PlayerMovement::camera_horizontal_delta(const Camera &camera,
    const CameraInput &input, double delta_seconds, double speed_scale,
    double *delta_x, double *delta_z)
{
    double local_x;
    double local_z;
    double move_length;
    double distance;
    double yaw_cos;
    double yaw_sin;
    double speed;

    if (delta_x == nullptr || delta_z == nullptr)
        return ;
    *delta_x = 0.0;
    *delta_z = 0.0;
    if (delta_seconds <= 0.0)
        return ;
    local_x = 0.0;
    local_z = 0.0;
    if (input.move_forward == true)
        local_z = local_z + 1.0;
    if (input.move_backward == true)
        local_z = local_z - 1.0;
    if (input.move_right == true)
        local_x = local_x + 1.0;
    if (input.move_left == true)
        local_x = local_x - 1.0;
    move_length = (local_x * local_x) + (local_z * local_z);
    if (move_length > 1.0)
    {
        local_x = local_x * 0.7071067811865475;
        local_z = local_z * 0.7071067811865475;
    }
    speed = camera.speed * speed_scale;
    if (input.speed_boost == true)
        speed = speed * 20.0;
    distance = speed * delta_seconds;
    yaw_cos = std::cos(camera.yaw);
    yaw_sin = std::sin(camera.yaw);
    *delta_x = ((local_x * yaw_cos) + (local_z * yaw_sin)) * distance;
    *delta_z = ((local_z * yaw_cos) - (local_x * yaw_sin)) * distance;
}

void PlayerMovement::update_player_horizontal_motion(Camera *camera,
    const CameraInput &input, const World &world,
    double delta_seconds)
{
    double delta_x;
    double delta_z;
    bool   in_water;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    camera->update_orientation(input, delta_seconds);
    in_water = player_in_water(*camera, world);
    PlayerMovement::camera_horizontal_delta(*camera, input, delta_seconds,
        in_water ? 0.5 : 1.0, &delta_x, &delta_z);
    (void)PlayerCollision::move_player_axis(camera, world, 0, delta_x);
    (void)PlayerCollision::move_player_axis(camera, world, 2, delta_z);
}

void PlayerMovement::update_player_vertical_motion(Camera *camera,
    const CameraInput &input, const World &world,
    double delta_seconds)
{
    const double jump_speed    = 9.0;
    const double gravity       = 32.0;
    const double max_fall      = -60.0;
    const double water_gravity = 3.0;
    const double water_drag    = 4.0;
    const double water_swim    = 6.0;
    double delta_y;
    double previous_y;
    bool   collided;
    bool   in_water;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    in_water = player_in_water(*camera, world);
    if (in_water)
    {
        double accel = -water_gravity;
        if (input.jump_held == true)
            accel += water_swim;
        accel -= water_drag * camera->vertical_velocity;
        camera->vertical_velocity += accel * delta_seconds;
        if (camera->vertical_velocity >  2.0) camera->vertical_velocity =  2.0;
        if (camera->vertical_velocity < -2.0) camera->vertical_velocity = -2.0;
        camera->on_ground = false;
    }
    else
    {
        if (camera->on_ground == true && input.jump_pressed == true)
        {
            camera->vertical_velocity = jump_speed;
            camera->on_ground = false;
        }
        camera->vertical_velocity = camera->vertical_velocity
            - (gravity * delta_seconds);
        if (camera->vertical_velocity < max_fall)
            camera->vertical_velocity = max_fall;
    }
    delta_y    = camera->vertical_velocity * delta_seconds;
    previous_y = camera->y;
    collided   = PlayerCollision::move_player_axis(camera, world, 1, delta_y);
    if (collided == true)
    {
        camera->on_ground = !in_water && camera->vertical_velocity <= 0.0
            && camera->y >= previous_y + delta_y;
        camera->vertical_velocity = 0.0;
        return ;
    }
    camera->on_ground = false;
}
