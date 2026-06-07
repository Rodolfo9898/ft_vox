#include "../../src/movement/PlayerMovement.hpp"

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

void PlayerMovement::camera_horizontal_delta(const Camera &camera,
    const CameraInput &input, double delta_seconds, double *delta_x,
    double *delta_z)
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
    speed = camera.speed;
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

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    camera->update_orientation(input, delta_seconds);
    PlayerMovement::camera_horizontal_delta(*camera, input, delta_seconds, &delta_x,
        &delta_z);
    (void)PlayerCollision::move_player_axis(camera, world, 0, delta_x);
    (void)PlayerCollision::move_player_axis(camera, world, 2, delta_z);
}

void PlayerMovement::update_player_vertical_motion(Camera *camera,
    const CameraInput &input, const World &world,
    double delta_seconds)
{
    const double jump_speed = 12.0;
    const double gravity = 22.0;
    const double max_fall_speed = -42.0;
    double delta_y;
    double previous_y;
    bool collided;

    if (camera == nullptr || delta_seconds <= 0.0)
        return ;
    if (camera->on_ground == true && input.jump_pressed == true)
    {
        camera->vertical_velocity = jump_speed;
        camera->on_ground = false;
    }
    camera->vertical_velocity = camera->vertical_velocity
        - (gravity * delta_seconds);
    if (camera->vertical_velocity < max_fall_speed)
        camera->vertical_velocity = max_fall_speed;
    delta_y = camera->vertical_velocity * delta_seconds;
    previous_y = camera->y;
    collided = PlayerCollision::move_player_axis(camera, world, 1, delta_y);
    if (collided == true)
    {
        camera->on_ground = camera->vertical_velocity <= 0.0
            && camera->y >= previous_y + delta_y;
        camera->vertical_velocity = 0.0;
        return ;
    }
    camera->on_ground = false;
}

