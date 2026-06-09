#include "../../src/validators/CollisionValidator.hpp"

CollisionValidator::CollisionValidator()
{
}

CollisionValidator::CollisionValidator(const CollisionValidator &other)
{
    *this = other;
}

CollisionValidator::~CollisionValidator()
{
}

CollisionValidator &CollisionValidator::operator=(const CollisionValidator &other)
{
    (void)other;
    return (*this);
}

int CollisionValidator::validate()
{
    World world;
    Camera camera;
    CameraInput input;
    double start_x;
    double start_z;
    double climb_start_y;
    double before_wall_x;
    double tunnel_start_z;
    double tunnel_end_z;
    double drop_start_y;
    double movement_delta_z;
    double wall_delta_x;
    double climb_delta_y;
    int32_t step_x;
    int32_t step_y;
    int32_t step_z;
    int32_t tunnel_x;
    int32_t tunnel_y;
    int32_t tunnel_z;
    int32_t drop_x;
    int32_t drop_y;
    int32_t drop_z;
    int32_t frame;
    int32_t error_code;
    double step_ground_eye_y;
    bool climbed_step;

    error_code = world.initialize("integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("collision world initialization", error_code));
    camera.initialize();
    PlayerController::spawn_player_on_ground(&camera, world);
    start_x = camera.x;
    start_z = camera.z;
    input = InputReader::empty_camera_input();
    input.move_forward = true;
    PlayerController::update_player_horizontal_motion(&camera, input, world, 1.0);
    movement_delta_z = camera.z - start_z;
    if (std::fabs(camera.z - start_z) < 0.25)
    {
        world.destroy();
        std::fprintf(stderr, "collision: failed movement sanity check\n");
        return (1);
    }
    before_wall_x = camera.x;
    (void)world.place_block_at(
        static_cast<int32_t>(std::floor(camera.x + 1.0)),
        static_cast<int32_t>(std::floor(camera.y - 1.0)),
        static_cast<int32_t>(std::floor(camera.z)), TERRAIN_GENERATOR_STONE_BLOCK);
    input = InputReader::empty_camera_input();
    input.move_right = true;
    PlayerController::update_player_horizontal_motion(&camera, input, world, 0.25);
    wall_delta_x = camera.x - before_wall_x;
    if (camera.x > before_wall_x + 0.75)
    {
        world.destroy();
        std::fprintf(stderr, "collision: failed wall rejection check\n");
        return (1);
    }
    camera.initialize();
    PlayerController::spawn_player_on_ground(&camera, world);
    climb_start_y = camera.y;
    step_x = static_cast<int32_t>(std::floor(camera.x));
    step_z = static_cast<int32_t>(std::floor(camera.z + 1.0));
    if (PlayerController::ground_eye_y_at(world, static_cast<double>(step_x) + 0.5,
            static_cast<double>(step_z) + 0.5, &step_ground_eye_y) == false)
    {
        world.destroy();
        std::fprintf(stderr, "collision: failed step surface setup\n");
        return (1);
    }
    step_y = static_cast<int32_t>(std::floor(step_ground_eye_y
        - PlayerController::PLAYER_EYE_HEIGHT));
    frame = 0;
    while (frame < 4)
    {
        error_code = world.place_block_at(step_x, step_y,
            step_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        if (error_code != FT_ERR_SUCCESS && error_code != FT_ERR_ALREADY_EXISTS)
        {
            world.destroy();
            return (ApplicationError::fail("collision step setup", error_code));
        }
        frame = frame + 1;
    }
    input = InputReader::empty_camera_input();
    input.move_forward = true;
    input.jump_pressed = true;
    frame = 0;
    climbed_step = false;
    climb_delta_y = 0.0;
    while (frame < 120)
    {
        PlayerController::update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        PlayerController::update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        if (camera.y - climb_start_y > climb_delta_y)
            climb_delta_y = camera.y - climb_start_y;
        if (camera.y >= climb_start_y + 0.75 && camera.on_ground == true)
            climbed_step = true;
        input.jump_pressed = false;
        frame = frame + 1;
    }
    if (climbed_step == false)
    {
        world.destroy();
        std::fprintf(stderr, "collision: failed one-block climb check y=%.2f start=%.2f ground=%d\n",
            camera.y, climb_start_y, camera.on_ground == true ? 1 : 0);
        return (1);
    }
    camera.initialize();
    PlayerController::spawn_player_on_ground(&camera, world);
    tunnel_x = static_cast<int32_t>(std::floor(camera.x));
    tunnel_y = static_cast<int32_t>(std::floor(camera.y
        - PlayerController::PLAYER_EYE_HEIGHT));
    tunnel_start_z = camera.z;
    tunnel_z = static_cast<int32_t>(std::floor(camera.z));
    frame = 0;
    while (frame < 6)
    {
        (void)world.delete_block_at(tunnel_x, tunnel_y,
            tunnel_z + frame);
        (void)world.delete_block_at(tunnel_x, tunnel_y + 1,
            tunnel_z + frame);
        (void)world.place_block_at(tunnel_x - 1, tunnel_y,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)world.place_block_at(tunnel_x - 1, tunnel_y + 1,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)world.place_block_at(tunnel_x + 1, tunnel_y,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        (void)world.place_block_at(tunnel_x + 1, tunnel_y + 1,
            tunnel_z + frame, TERRAIN_GENERATOR_STONE_BLOCK);
        frame = frame + 1;
    }
    input = InputReader::empty_camera_input();
    input.move_forward = true;
    frame = 0;
    while (frame < 90)
    {
        PlayerController::update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        PlayerController::update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        frame = frame + 1;
    }
    tunnel_end_z = camera.z;
    if (tunnel_end_z < tunnel_start_z + 1.0)
    {
        world.destroy();
        std::fprintf(stderr,
            "collision: failed one-block-wide tunnel check start=%.2f end=%.2f\n",
            tunnel_start_z, tunnel_end_z);
        return (1);
    }
    camera.initialize();
    PlayerController::spawn_player_on_ground(&camera, world);
    drop_start_y = camera.y;
    drop_x = static_cast<int32_t>(std::floor(camera.x));
    drop_y = static_cast<int32_t>(std::floor(camera.y
        - PlayerController::PLAYER_EYE_HEIGHT)) - 1;
    drop_z = static_cast<int32_t>(std::floor(camera.z));
    frame = 0;
    while (frame < 5)
    {
        (void)world.delete_block_at(drop_x, drop_y - frame,
            drop_z);
        frame = frame + 1;
    }
    camera.on_ground = false;
    camera.vertical_velocity = 0.0;
    input = InputReader::empty_camera_input();
    frame = 0;
    while (frame < 90)
    {
        PlayerController::update_player_vertical_motion(&camera, input, world,
            1.0 / 60.0);
        PlayerController::update_player_horizontal_motion(&camera, input, world,
            1.0 / 60.0);
        frame = frame + 1;
    }
    if (camera.y > drop_start_y - 1.0)
    {
        world.destroy();
        std::fprintf(stderr,
            "collision: failed one-block drop shaft check start_y=%.2f end_y=%.2f\n",
            drop_start_y, camera.y);
        return (1);
    }
    std::printf("collision: ok start=(%.2f,%.2f) moved_z=%.2f wall_x=%.2f climb_y=%.2f tunnel_z=%.2f drop_y=%.2f\n",
        start_x, start_z, movement_delta_z, wall_delta_x, climb_delta_y,
        tunnel_end_z - tunnel_start_z, drop_start_y - camera.y);
    world.destroy();
    return (0);
}
